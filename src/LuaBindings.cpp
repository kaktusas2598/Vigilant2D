#include "LuaBindings.hpp"

#include <cmath>

#include "ScriptSystem.hpp"
#include "Scene.hpp"
#include "AnimatedSprite.hpp"
#include "AnimationRegistry.hpp"
#include "ParticleEmitter.hpp"
#include "ParticleEmitterRegistry.hpp"
#include "Input.hpp"
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
        const glm::vec2 centre = boundPos + entity->getBoundsSize() * 0.5f;
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

static int l_is_mouse_button_pressed(lua_State* L) {
    ScriptSystem* scriptSystem = getScriptSystem(L);
    if (scriptSystem == nullptr || scriptSystem->getRuntimeInput() == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const int button = static_cast<int>(luaL_checkinteger(L, 1));
    lua_pushboolean(L, scriptSystem->getRuntimeInput()->isMouseButtonPressed(button));
    return 1;
}

static int l_is_key_pressed(lua_State* L) {
    ScriptSystem* scriptSystem = getScriptSystem(L);
    if (scriptSystem == nullptr || scriptSystem->getRuntimeInput() == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const int key = static_cast<int>(luaL_checkinteger(L, 1));
    lua_pushboolean(L, scriptSystem->getRuntimeInput()->isKeyPressed(key));
    return 1;
}

static int l_get_mouse_world_position(lua_State* L) {
    ScriptSystem* scriptSystem = getScriptSystem(L);
    if (scriptSystem == nullptr || 
        scriptSystem->getRuntimeInput() == nullptr ||
        scriptSystem->getRuntimeCamera() == nullptr) {
        lua_pushnil(L);
        return 1;
    }

    const float mouseX = static_cast<float>(scriptSystem->getRuntimeInput()->getMouseX());
    const float mouseY = static_cast<float>(scriptSystem->getRuntimeInput()->getMouseY());

    const glm::vec2 world = scriptSystem->getRuntimeCamera()->screenToWorld({mouseX, mouseY});
    lua_pushnumber(L, world.x);
    lua_pushnumber(L, world.y);
    return 2;
}

static int l_emit_particles(lua_State* L) {
    ScriptSystem* scriptSystem = getScriptSystem(L);
    if (scriptSystem == nullptr || scriptSystem->getParticleEmitterRegistry() == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* emitterId = luaL_checkstring(L, 1);
    const float x = static_cast<float>(luaL_checknumber(L, 2));
    const float y = static_cast<float>(luaL_checknumber(L, 3));
    const int count = lua_gettop(L) >= 4 ? static_cast<int>(luaL_checkinteger(L, 4)) : 1;

    ParticleEmitter* emitter = scriptSystem->getParticleEmitterRegistry()->getEmitter(emitterId);
    if (emitter == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    emitter->emit({x, y}, count);
    lua_pushboolean(L, 1);
    return 1;
}

static int l_get_mouse_tile(lua_State* L) {
    ScriptSystem* scriptSystem = getScriptSystem(L);
    if (scriptSystem == nullptr ||
        scriptSystem->getRuntimeInput() == nullptr ||
        scriptSystem->getRuntimeCamera() == nullptr ||
        scriptSystem->getRuntimeScene() == nullptr) {
        lua_pushnil(L);
        return 1;
    }

    const float mouseX = static_cast<float>(scriptSystem->getRuntimeInput()->getMouseX());
    const float mouseY = static_cast<float>(scriptSystem->getRuntimeInput()->getMouseY());

    const glm::vec2 world = scriptSystem->getRuntimeCamera()->screenToWorld({mouseX, mouseY});
    TileMap* map = scriptSystem->getRuntimeScene()->getTileMap();
    if (map == nullptr) {
        lua_pushnil(L);
        return 1;
    }

    const glm::ivec2 tile = map->worldToTile(world);
    if (!map->isTileInBounds(tile.x, tile.y)) {
        lua_pushnil(L);
        return 1;
    }

    lua_pushinteger(L, tile.x);
    lua_pushinteger(L, tile.y);
    return 2;
}

static int l_get_tile_world_position(lua_State* L) {
    ScriptSystem* scriptSystem = getScriptSystem(L);
    if (scriptSystem == nullptr || scriptSystem->getRuntimeScene() == nullptr) {
        lua_pushnil(L);
        return 1;
    }

    TileMap* map = scriptSystem->getRuntimeScene()->getTileMap();
    if (map == nullptr) {
        lua_pushnil(L);
        return 1;
    }

    const int tileX = static_cast<int>(luaL_checkinteger(L, 1));
    const int tileY = static_cast<int>(luaL_checkinteger(L, 2));

    if (!map->isTileInBounds(tileX, tileY)) {
        lua_pushnil(L);
        return 1;
    }

    const glm::vec2 world = map->tileToWorld(tileX, tileY);
    lua_pushnumber(L, world.x);
    lua_pushnumber(L, world.y);
    return 2;
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

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_is_mouse_button_pressed, 1);
    lua_setfield(luaState, -2, "is_mouse_button_pressed");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_is_key_pressed, 1);
    lua_setfield(luaState, -2, "is_key_pressed");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_get_mouse_world_position, 1);
    lua_setfield(luaState, -2, "get_mouse_world_position");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_emit_particles, 1);
    lua_setfield(luaState, -2, "emit_particles");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_get_mouse_tile, 1);
    lua_setfield(luaState, -2, "get_mouse_tile");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_get_tile_world_position, 1);
    lua_setfield(luaState, -2, "get_tile_world_position");

    lua_setglobal(luaState, "engine");
}