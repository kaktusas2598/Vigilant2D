#include "ScriptSystem.hpp"

#include "Logger.hpp"

static bool readVec2Field(lua_State* L, int tableIndex, const char* fieldName, glm::vec2& outVec) {
    lua_getfield(L, tableIndex, fieldName);
    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        return false;
    }

    const int vecIndex = lua_gettop(L);
    lua_geti(L, vecIndex, 1);
    lua_geti(L, vecIndex, 2);
    if (!lua_isnumber(L, -2) || !lua_isnumber(L, -1)) {
        lua_pop(L, 3);
        return false;
    }

    outVec.x = static_cast<float>(lua_tonumber(L, -2));
    outVec.y = static_cast<float>(lua_tonumber(L, -1));
    lua_pop(L, 3);
    return true;
}

bool ScriptSystem::init() {
    if (luaState != nullptr)
        return true;

    luaState = luaL_newstate();
    if (luaState == nullptr) {
        VG_ERROR("[Lua] Failed to create lua_State.");
        return false;
    }

    luaL_openlibs(luaState);
    return true;
}

void ScriptSystem::shutdown() {
    if (luaState != nullptr) {
        lua_close(luaState);
        luaState = nullptr;
    }
}

bool ScriptSystem::loadScript(const std::string &fileName) {
    if (luaState == nullptr && !init())
        return false;

    const int status = luaL_dofile(luaState, fileName.c_str());
    return reportError(status, "loadScript(" + fileName + ")");
}

bool ScriptSystem::loadEntityDefinition(const std::string& fileName, EntityDefinition& outDefinition) {
    if (luaState == nullptr && !init())
        return false;

    const int loadStatus = luaL_loadfile(luaState, fileName.c_str());
    if (!reportError(loadStatus, "luaL_loadfile(" + fileName + ")"))
        return false;

    const int callStatus = lua_pcall(luaState, 0, 1, 0);
    if (!reportError(callStatus, "execute(" + fileName + ")"))
        return false;

    if (!lua_istable(luaState, -1)) {
        VG_ERROR("[Lua] Entity definition '" + fileName + "' must return a table.");
        lua_pop(luaState, 1);
        return false;
    }

    const int tableIndex = lua_gettop(luaState);

    lua_getfield(luaState, tableIndex, "texture");
    if (lua_isstring(luaState, -1)) {
        outDefinition.texture = lua_tostring(luaState, -1);
    }
    lua_pop(luaState, 1);

    lua_getfield(luaState, tableIndex, "animation");
    if (lua_isstring(luaState, -1)) {
        outDefinition.animation = lua_tostring(luaState, -1);
    }
    lua_pop(luaState, 1);

    readVec2Field(luaState, tableIndex, "scale", outDefinition.scale);

    glm::vec2 bounds;
    if (readVec2Field(luaState, tableIndex, "bounds_offset", outDefinition.boundsOffset)) {
        outDefinition.hasBounds = true;
    }

    if (readVec2Field(luaState, tableIndex, "bounds_size", outDefinition.boundsSize)) {
        outDefinition.hasBounds = true;
    }

    lua_getfield(luaState, tableIndex, "script");
    if (lua_isstring(luaState, -1)) {
        outDefinition.behaviorScript = lua_tostring(luaState, -1);
    }
    lua_pop(luaState, 1);

    lua_getfield(luaState, tableIndex, "physics_enabled");
    if (lua_isboolean(luaState, -1)) {
        outDefinition.physicsEnabled = lua_toboolean(luaState, -1) != 0;
    }
    lua_pop(luaState, 1);

    lua_pop(luaState, 1); // pop returned table
    return true;
}

bool ScriptSystem::callGlobal(const std::string &functionName) {
     if (luaState == nullptr)
        return false;

    lua_getglobal(luaState, functionName.c_str());
    if (!lua_isfunction(luaState, -1)) {
        lua_pop(luaState, 1);
        VG_ERROR("[Lua] Global '" + functionName + "' is not a function.");
        return false;
    }

    const int status = lua_pcall(luaState, 0, 0, 0);
    return reportError(status, "callGlobal(" + functionName + ")");
}

bool ScriptSystem::callGlobal(const std::string& functionName, float dt) {
    if (luaState == nullptr)
        return false;

    lua_getglobal(luaState, functionName.c_str());
    if (!lua_isfunction(luaState, -1)) {
        lua_pop(luaState, 1);
        VG_ERROR("[Lua] Global '" + functionName + "' is not a function.");
        return false;
    }

    lua_pushnumber(luaState, dt);
    const int status = lua_pcall(luaState, 1, 0, 0);
    return reportError(status, "callGlobal(" + functionName + ")");

}

ScriptInstance ScriptSystem::loadBehavior(const std::string& fileName) {
    ScriptInstance instance;
    instance.fileName = fileName;

    if (luaState == nullptr && !init())
        return instance;

    const int loadStatus = luaL_loadfile(luaState, fileName.c_str());
    if (!reportError(loadStatus, "luaL_loadfile(" + fileName + ")"))
        return instance;

    const int callStatus = lua_pcall(luaState, 0, 1, 0);
    if (!reportError(callStatus, "execute(" + fileName + ")"))
        return instance;

    if (!lua_istable(luaState, -1)) {
        VG_ERROR("[Lua] Script '" + fileName + "' must return a table.");
        lua_pop(luaState, 1);
        return instance;
    }

    instance.tableRef = luaL_ref(luaState, LUA_REGISTRYINDEX);
    return instance;
}

bool ScriptSystem::callOnCreate(const ScriptInstance& instance) {
    if(luaState == nullptr || instance.tableRef == LUA_NOREF)
        return false;

    lua_rawgeti(luaState, LUA_REGISTRYINDEX, instance.tableRef); // push table
    lua_getfield(luaState, -1, "on_create"); // push field - function in this case

    if (!lua_isfunction(luaState, -1)) {
        lua_pop(luaState, 2);
        return true;
    }

    // Remove table at -2, leaving function at the top of the stack
    lua_remove(luaState, -2);

    const int status = lua_pcall(luaState, 0, 0, 0);
    return reportError(status, "callOnCreate(" + instance.fileName + ")");
}

bool ScriptSystem::callOnUpdate(const ScriptInstance& instance, float dt) {
    if(luaState == nullptr || instance.tableRef == LUA_NOREF)
        return false;

    lua_rawgeti(luaState, LUA_REGISTRYINDEX, instance.tableRef); // push table
    lua_getfield(luaState, -1, "on_update"); // push field - function in this case

    if (!lua_isfunction(luaState, -1)) {
        lua_pop(luaState, 2);
        return true;
    }

    // Remove table at -2, leaving function at the top of the stack
    lua_remove(luaState, -2);
    lua_pushnumber(luaState, dt);

    const int status = lua_pcall(luaState, 1, 0, 0);
    return reportError(status, "callOnUpdate(" + instance.fileName + ")");

}

void ScriptSystem::releaseInstance(ScriptInstance& instance) {
    if (luaState != nullptr && instance.tableRef != LUA_NOREF) {
        luaL_unref(luaState, LUA_REGISTRYINDEX, instance.tableRef);
    }

    instance.tableRef = LUA_NOREF;
    instance.fileName.clear();
}

bool ScriptSystem::attachToEntity(const Entity& entity) {
    if (!entity.hasScript())
        return false;

    auto it = entityScripts.find(entity.getID());
    if (it != entityScripts.end()) {
        releaseInstance(it->second);
        entityScripts.erase(it);
    }

    ScriptInstance instance = loadBehavior(entity.getScriptName());
    if (instance.tableRef == LUA_NOREF)
        return false;

    entityScripts[entity.getID()] = std::move(instance);
    return true;
}

bool ScriptSystem::callEntityOnCreate(const Entity& entity) {
    auto it = entityScripts.find(entity.getID());
    if (it == entityScripts.end())
        return false;
    return callOnCreate(it->second);
}

bool ScriptSystem::callEntityOnUpdate(const Entity& entity, float dt) {
    auto it = entityScripts.find(entity.getID());
    if (it == entityScripts.end())
        return false;
    return callOnUpdate(it->second, dt);
}

void ScriptSystem::detachFromEntity(const Entity& entity) {
    auto it = entityScripts.find(entity.getID());
    if (it == entityScripts.end())
        return;

    releaseInstance(it->second);
    entityScripts.erase(it);
}

bool ScriptSystem::reportError(int status, const std::string &context) {
    if (status == LUA_OK)
        return true;
    
    const char* message = lua_tostring(luaState, -1);
    VG_ERROR("[Lua] " + context + " failed: " + (message != nullptr ? message : "unkown error"));
    lua_pop(luaState, 1);
    return false;
}