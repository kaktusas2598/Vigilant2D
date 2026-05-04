#pragma once

#include <string>

extern "C" {
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
}

class ScriptSystem {
    public:
        ScriptSystem() = default;
        ~ScriptSystem() {
            shutdown();
        };

        bool init();
        void shutdown();

        // Load Lua script into global stated owned by the engine
        bool loadScript(const std::string& fileName);
        // Call global Lua method
        bool callGlobal(const std::string& functionName);

        // bool callUpdate(float dt);


        lua_State* getState() const { return luaState; }
    private:
        bool reportError(int status, const std::string& context);

        lua_State* luaState = nullptr;
};