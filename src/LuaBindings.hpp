#pragma once

struct lua_State;
class ScriptSystem;

void registerEngineBindings(lua_State* luaState, ScriptSystem& scriptSystem);