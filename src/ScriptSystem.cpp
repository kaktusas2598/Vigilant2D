#include "ScriptSystem.hpp"

#include "Logger.hpp"

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
    if (luaState != nullptr && !init())
        return false;

    const int status = luaL_dofile(luaState, fileName.c_str());
    return reportError(status, "loadScript(" + fileName + ")");
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

bool ScriptSystem::reportError(int status, const std::string &context) {
    if (status == LUA_OK)
        return true;
    
    const char* message = lua_tostring(luaState, -1);
    VG_ERROR("[Lua] " + context + " failed: " + (message != nullptr ? message : "unkown error"));
    lua_pop(luaState, 1);
    return false;
}