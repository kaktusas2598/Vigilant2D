#pragma once

#include <string>
#include <vector>

extern "C" {
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
}

class Scene;

// Represents Lua coroutine allowing great many things to happen
struct ScriptTask {
    int threadRef = LUA_NOREF;
    std::string ownerEntityId;
    bool global = false;
    bool finished = false;

    enum class WaitMode {
        None,
        Seconds,
        NextFrame
    };

    WaitMode waitMode = WaitMode::None;
    float waitRemaining = 0.0f;
};

class ScriptTaskRunner {
    public:
        void init(lua_State* newLuaState);
        void shutdown();

        bool startEntityCoroutine(const std::string& ownerEntityId, int functionIndex, int selfIndex);
        bool startGlobalCoroutine(int functionIndex);
        void update(float dt, Scene* runtimeScene);
        void cancelTasksForEntity(const std::string& entityId);
        void cancelAllTasks();

    private:
        lua_State* getTaskThread(const ScriptTask& task) const;
        bool startTask(ScriptTask& task, int argumentCount);
        bool resumeTask(ScriptTask& task, float dt);
        bool configureTaskWait(ScriptTask& task, lua_State* thread, int resultCount);
        void releaseTask(ScriptTask& task);
        bool reportThreadError(lua_State* thread, const std::string& context);

        lua_State* luaState = nullptr;
        std::vector<ScriptTask> activeTasks;
};