#pragma once

struct lua_State;
class FarmWorldState;

// NOTE: Custom game lua bindings, not sure if I like this
void registerFarmBindings(lua_State* luaState, FarmWorldState& farmWorldState);