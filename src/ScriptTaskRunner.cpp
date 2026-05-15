#include "ScriptTaskRunner.hpp"

#include <algorithm>
#include "Logger.hpp"
#include "Scene.hpp"

void ScriptTaskRunner::init(lua_State* newLuaState) {
    luaState = newLuaState;
}

void ScriptTaskRunner::shutdown() {
    for (ScriptTask& task : activeTasks)
        releaseTask(task);

    activeTasks.clear();
    luaState = nullptr;
}

bool ScriptTaskRunner::startEntityCoroutine(const std::string& ownerEntityId, int functionIndex, int selfIndex) {
    if (luaState == nullptr)
        return false;
    
    functionIndex = lua_absindex(luaState, functionIndex);
    selfIndex = lua_absindex(luaState, selfIndex);
    lua_State* thread = lua_newthread(luaState);
    ScriptTask task;
    task.threadRef = luaL_ref(luaState, LUA_REGISTRYINDEX);
    task.ownerEntityId = ownerEntityId;
    task.global = false;

    lua_pushvalue(luaState, functionIndex);
    lua_xmove(luaState, thread, 1);
    lua_pushvalue(luaState, selfIndex);
    lua_xmove(luaState, thread, 1);

    const bool ok = startTask(task, 1);
    if (task.finished || !ok) {
        releaseTask(task);
        return ok;
    }

    activeTasks.push_back(std::move(task));
    return true;
}

bool ScriptTaskRunner::startGlobalCoroutine(int functionIndex) {
    if (luaState == nullptr)
        return false;
    
    functionIndex = lua_absindex(luaState, functionIndex);
    lua_State* thread = lua_newthread(luaState);
    ScriptTask task;
    task.threadRef = luaL_ref(luaState, LUA_REGISTRYINDEX);
    task.global = true;

    lua_pushvalue(luaState, functionIndex);
    lua_xmove(luaState, thread, 1);

    const bool ok = startTask(task, 0);
    if (task.finished || !ok) {
        releaseTask(task);
        return ok;
    }

    activeTasks.push_back(std::move(task));
    return true;
}

void ScriptTaskRunner::update(float dt, Scene* runtimeScene) {
    for (ScriptTask& task : activeTasks) {
        if (task.finished)
            continue;
        
        if (!task.global && runtimeScene != nullptr) {
            if (runtimeScene->findEntityByID(task.ownerEntityId) == nullptr) {
                task.finished = true;
                continue;
            }
        }

        if (task.waitMode == ScriptTask::WaitMode::Seconds) {
            task.waitRemaining -= dt;
            if (task.waitRemaining > 0.0f)
                continue;
            
        } else if (task.waitMode != ScriptTask::WaitMode::NextFrame) {
            continue;
        }

        resumeTask(task, dt);
    }

    for (size_t i = 0; i < activeTasks.size();) {
        if (!activeTasks[i].finished) {
            ++i;
            continue;
        }

        releaseTask(activeTasks[i]);
        activeTasks.erase(activeTasks.begin() + static_cast<long long>(i));
    }
}

void ScriptTaskRunner::cancelTasksForEntity(const std::string& entityId) {
    for (ScriptTask& task : activeTasks) {
        if (!task.global && task.ownerEntityId == entityId)
            task.finished = true;
    }
}

lua_State* ScriptTaskRunner::getTaskThread(const ScriptTask& task) const {
    if (luaState == nullptr || task.threadRef == LUA_NOREF)
        return nullptr;
    
    lua_rawgeti(luaState, LUA_REGISTRYINDEX, task.threadRef);
    lua_State* thread = lua_tothread(luaState, -1);
    lua_pop(luaState, 1);
    return thread;
}

bool ScriptTaskRunner::startTask(ScriptTask& task, int argumentCount) {
    lua_State* thread = getTaskThread(task);
    if (thread == nullptr) {
        task.finished = true;
        return false;
    }

    int resultCount = 0;
    const int status = lua_resume(thread, nullptr, argumentCount, &resultCount);

    if (status == LUA_OK) {
        task.finished = true;
        lua_settop(thread, 0);
        return true;
    }

    if (status == LUA_YIELD)
        return configureTaskWait(task, thread, resultCount);

    reportThreadError(thread, "start coroutine");
    task.finished = true;
    lua_settop(thread, 0);
    return false;
}

bool ScriptTaskRunner::resumeTask(ScriptTask& task, float dt) {
    lua_State* thread = getTaskThread(task);
    if (thread == nullptr) {
        task.finished = true;
        return false;
    }

    int argumentCount = 0;
    if (task.waitMode == ScriptTask::WaitMode::NextFrame) {
        lua_pushnumber(thread, dt);
        argumentCount = 1;
    }

    int resultCount = 0;
    const int status = lua_resume(thread, nullptr, argumentCount, &resultCount);

    if (status == LUA_OK) {
        task.finished = true;
        lua_settop(thread, 0);
        return true;
    }

    if (status == LUA_YIELD)
        return configureTaskWait(task, thread, resultCount);

    reportThreadError(thread, "resume coroutine");
    task.finished = true;
    lua_settop(thread, 0);
    return false;
}

bool ScriptTaskRunner::configureTaskWait(ScriptTask& task, lua_State* thread, int resultCount) {
    task.waitMode = ScriptTask::WaitMode::None;
    task.waitRemaining = 0.0f;

    if (resultCount <= 0 || !lua_isstring(thread, -resultCount)) {
        VG_ERROR("[Lua] Coroutine yielded without a valid wait request.");
        lua_settop(thread, 0);
        return false;
    }

    const std::string waitType = lua_tostring(thread, -resultCount);
    if (waitType == "wait_seconds") {
        if (resultCount < 2 || !lua_isnumber(thread, -resultCount + 1)) {
            VG_ERROR("[Lua] wait_seconds coroutine yield missing duration value.");
            lua_settop(thread, 0);
            return false;
        }
        task.waitMode = ScriptTask::WaitMode::Seconds;
        task.waitRemaining = std::max(0.0f, static_cast<float>(lua_tonumber(thread, -resultCount + 1)));
    } else if (waitType == "wait_frame") {
        task.waitMode = ScriptTask::WaitMode::NextFrame;
    } else {
        VG_ERROR("[Lua] Unrecognised coroutine wait request: " + waitType);
        lua_settop(thread, 0);
        return false;
    }

    lua_settop(thread, 0);
    return true;
}

void ScriptTaskRunner::releaseTask(ScriptTask& task) {
    if (luaState != nullptr && task.threadRef != LUA_NOREF)
        luaL_unref(luaState, LUA_REGISTRYINDEX, task.threadRef);
    
    task.threadRef = LUA_NOREF;
    task.ownerEntityId.clear();
    task.finished = true;
    task.waitMode = ScriptTask::WaitMode::None;
    task.waitRemaining = 0.0f;
}

bool ScriptTaskRunner::reportThreadError(lua_State* thread, const std::string& context) {
    const char* message = lua_tostring(thread, -1);
    VG_ERROR("[Lua] " + context + " failed: " + (message != nullptr ? message : "unkown error"));
    lua_pop(thread, 1);
    return false;

}