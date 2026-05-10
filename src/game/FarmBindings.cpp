#include "FarmBindings.hpp"

#include "FarmWorldState.hpp"

extern "C" {
    #include <lua.h>
    #include <lauxlib.h>
}

static FarmWorldState* getFarmWorldState(lua_State* L) {
    return static_cast<FarmWorldState*>(lua_touserdata(L, lua_upvalueindex(1)));
}

static int l_is_tilled(lua_State* L) {
    FarmWorldState* farm = getFarmWorldState(L);
    if (farm == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const int tileX = static_cast<int>(luaL_checkinteger(L, 1));
    const int tileY = static_cast<int>(luaL_checkinteger(L, 2));
    lua_pushboolean(L, farm->isTilled(tileX, tileY) ? 1 : 0);
    return 1;
}

static int l_set_tilled(lua_State* L) {
    FarmWorldState* farm = getFarmWorldState(L);
    if (farm == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const int tileX = static_cast<int>(luaL_checkinteger(L, 1));
    const int tileY = static_cast<int>(luaL_checkinteger(L, 2));
    const bool value = lua_toboolean(L, 3) != 0;

    lua_pushboolean(L, farm->setTilled(tileX, tileY, value) ? 1 : 0);
    return 1;
}

static int l_is_watered(lua_State* L) {
    FarmWorldState* farm = getFarmWorldState(L);
    if (farm == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const int tileX = static_cast<int>(luaL_checkinteger(L, 1));
    const int tileY = static_cast<int>(luaL_checkinteger(L, 2));
    lua_pushboolean(L, farm->isWatered(tileX, tileY) ? 1 : 0);
    return 1;
}

static int l_set_watered(lua_State* L) {
    FarmWorldState* farm = getFarmWorldState(L);
    if (farm == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const int tileX = static_cast<int>(luaL_checkinteger(L, 1));
    const int tileY = static_cast<int>(luaL_checkinteger(L, 2));
    const bool value = lua_toboolean(L, 3) != 0;

    lua_pushboolean(L, farm->setWatered(tileX, tileY, value) ? 1 : 0);
    return 1;
}

static int l_is_occupied(lua_State* L) {
    FarmWorldState* farm = getFarmWorldState(L);
    if (farm == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const int tileX = static_cast<int>(luaL_checkinteger(L, 1));
    const int tileY = static_cast<int>(luaL_checkinteger(L, 2));
    lua_pushboolean(L, farm->isOccupied(tileX, tileY) ? 1 : 0);
    return 1;
}

static int l_set_occupied(lua_State* L) {
    FarmWorldState* farm = getFarmWorldState(L);
    if (farm == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const int tileX = static_cast<int>(luaL_checkinteger(L, 1));
    const int tileY = static_cast<int>(luaL_checkinteger(L, 2));
    const bool value = lua_toboolean(L, 3) != 0;

    lua_pushboolean(L, farm->setOccupied(tileX, tileY, value) ? 1 : 0);
    return 1;
}

void registerFarmBindings(lua_State* luaState, FarmWorldState& farmWorldState) {
    lua_newtable(luaState);

    lua_pushlightuserdata(luaState, &farmWorldState);
    lua_pushcclosure(luaState, l_is_tilled, 1);
    lua_setfield(luaState, -2, "is_tilled");

    lua_pushlightuserdata(luaState, &farmWorldState);
    lua_pushcclosure(luaState, l_set_tilled, 1);
    lua_setfield(luaState, -2, "set_tilled");

    lua_pushlightuserdata(luaState, &farmWorldState);
    lua_pushcclosure(luaState, l_is_watered, 1);
    lua_setfield(luaState, -2, "is_watered");

    lua_pushlightuserdata(luaState, &farmWorldState);
    lua_pushcclosure(luaState, l_set_watered, 1);
    lua_setfield(luaState, -2, "set_watered");

    lua_pushlightuserdata(luaState, &farmWorldState);
    lua_pushcclosure(luaState, l_is_occupied, 1);
    lua_setfield(luaState, -2, "is_occupied");

    lua_pushlightuserdata(luaState, &farmWorldState);
    lua_pushcclosure(luaState, l_set_occupied, 1);
    lua_setfield(luaState, -2, "set_occupied");

    lua_setglobal(luaState, "farm");
}
