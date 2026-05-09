#include "LuaBindings.hpp"

#include <cmath>

#include "ScriptSystem.hpp"
#include "Scene.hpp"
#include "AnimatedSprite.hpp"
#include "AnimationRegistry.hpp"
#include "glm/glm.hpp"

static ScriptSystem* getScriptSystem(lua_State* L) {
    return static_cast<ScriptSystem*>(lua_touserdata(L, lua_upvalueindex(1)));
}

static Entity* getEntityFromArg(lua_State* L, ScriptSystem* scriptSystem, int argIndex) {
    if (scriptSystem == nullptr || scriptSystem->getRuntimeScene() == nullptr)
        return nullptr;

    const char* entityId = luaL_checkstring(L, argIndex);
    return scriptSystem->getRuntimeScene()->findEntityByID(entityId);
}

static int l_get_entity_position(lua_State* L) {
    ScriptSystem* scriptSystem = getScriptSystem(L);
    Entity* entity = getEntityFromArg(L, scriptSystem, 1);
    if (entity == nullptr) {
        lua_pushnil(L);
        return 1;
    }

    lua_pushnumber(L, entity->transform.position.x);
    lua_pushnumber(L, entity->transform.position.y);
    return 2;
}

static int l_set_entity_position(lua_State* L) {
    ScriptSystem* scriptSystem = getScriptSystem(L);
    Entity* entity = getEntityFromArg(L, scriptSystem, 1);
    if (entity == nullptr || scriptSystem->getRuntimeScene() == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const float x = static_cast<float>(luaL_checknumber(L, 2));
    const float y = static_cast<float>(luaL_checknumber(L, 3));
    const glm::vec2 position{x, y};

    if (entity->hasPhysicsBody()) {
        const glm::vec2 boundPos = position + entity->getBoundsOffset();
        const glm::vec2 centre = position + entity->getBoundsSize() * 0.5f;
        scriptSystem->getRuntimeScene()->getPhysicsWorld()
            .setBodyPositionPixels(entity->getPhysicsBody(), centre);
    } else {
        entity->transform.position = position;
    }

    lua_pushboolean(L, 1);
    return 1;
}

static int l_get_direction_to_entity(lua_State* L) {
    ScriptSystem* scriptSystem = getScriptSystem(L);
    Entity* source = getEntityFromArg(L, scriptSystem, 1);
    Entity* target = getEntityFromArg(L, scriptSystem, 2);
    if (source == nullptr || target == nullptr) {
        lua_pushnil(L);
        return 1;
    }

    const glm::vec2 delta = target->transform.position - source->transform.position;
    const float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);
    
    if (distance < 0.0001f) {
        lua_pushnumber(L, 0.0f);
        lua_pushnumber(L, 0.0f);
        lua_pushnumber(L, 0.0f);
        return 3;
    }

    const glm::vec2 direction = delta / distance;
    lua_pushnumber(L, direction.x);
    lua_pushnumber(L, direction.y);
    lua_pushnumber(L, distance);
    return 3;
}

static int l_play_entity_animation(lua_State* L) {
    ScriptSystem* scriptSystem = getScriptSystem(L);
    Entity* entity = getEntityFromArg(L, scriptSystem, 1);
    if (entity == nullptr || scriptSystem->getAnimationRegistry() == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* animationId = luaL_checkstring(L, 2);
    const bool restart = lua_gettop(L) >= 3 ? lua_toboolean(L, 3) != 0 : true;

    AnimatedSprite* animatedSprite = entity->getAnimatedSprite();
    if (animatedSprite == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const AnimationClip* clip = scriptSystem->getAnimationRegistry()->getClip(animationId);
    if (clip == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    animatedSprite->play(clip, restart);
    lua_pushboolean(L, 1);
    return 1;
}

static int l_set_entity_flip_x(lua_State* L) {
    ScriptSystem* scriptSystem = getScriptSystem(L);
    Entity* entity = getEntityFromArg(L, scriptSystem, 1);
    if (entity == nullptr || entity->getSprite() == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const bool flipX = lua_toboolean(L, 2) != 0; 
    entity->getSprite()->setFlipX(flipX);

    lua_pushboolean(L, 1);
    return 1;
}

void registerEngineBindings(lua_State* luaState, ScriptSystem& scriptSystem) {
    lua_newtable(luaState);

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_get_entity_position, 1);
    lua_setfield(luaState, -2, "get_entity_position");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_set_entity_position, 1);
    lua_setfield(luaState, -2, "set_entity_position");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_get_direction_to_entity, 1);
    lua_setfield(luaState, -2, "get_direction_to_entity");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_play_entity_animation, 1);
    lua_setfield(luaState, -2, "play_entity_animation");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_set_entity_flip_x, 1);
    lua_setfield(luaState, -2, "set_entity_flip_x");

    lua_setglobal(luaState, "engine");
}