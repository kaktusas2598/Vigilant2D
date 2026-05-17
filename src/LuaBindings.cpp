#include "LuaBindings.hpp"

#include <cmath>

#include "ScriptSystem.hpp"
#include "Scene.hpp"
#include "AnimatedSprite.hpp"
#include "AssetManager.hpp"
#include "AnimationRegistry.hpp"
#include "ParticleEmitter.hpp"
#include "ParticleEmitterRegistry.hpp"
#include "CameraFollowState.hpp"
#include "ScreenFlowSystem.hpp"
#include "AudioSystem.hpp"
#include "Input.hpp"
#include "UISystem.hpp"
#include "glm/glm.hpp"

// --------- STATIC HELPERS
static ScriptSystem* getScriptSystem(lua_State* L) {
    return static_cast<ScriptSystem*>(lua_touserdata(L, lua_upvalueindex(1)));
}

static Entity* getEntityFromArg(lua_State* L, ScriptSystem* scriptSystem, int argIndex) {
    if (scriptSystem == nullptr || scriptSystem->getRuntimeScene() == nullptr)
        return nullptr;

    const char* entityId = luaL_checkstring(L, argIndex);
    return scriptSystem->getRuntimeScene()->findEntityByID(entityId);
}

static UISystem* getUISystem(lua_State* L) {
    ScriptSystem* scriptSystem = getScriptSystem(L);
    return scriptSystem != nullptr ? scriptSystem->getUISystem() : nullptr;
}

static bool parseUIRenderSpace(const char* value, UIRenderSpace& outSpace) {
    if (value == nullptr)
        return false;
    
    const std::string s(value);
    if (s == "screen") {
        outSpace = UIRenderSpace::Screen;
        return true;
    }
    if (s == "world") {
        outSpace = UIRenderSpace::World;
        return true;
    }
    return false;
}

static const char* getEntityIdFromSelf(lua_State* L, int argIndex) {
    argIndex = lua_absindex(L, argIndex);

    if (!lua_istable(L, argIndex))
        return nullptr;

    lua_getfield(L, argIndex, "id");
    if (!lua_isstring(L, -1)) {
        lua_pop(L, 1);
        return nullptr;
    }

    const char* entityId = lua_tostring(L, -1);
    lua_pop(L, 1);
    return entityId;
}

// --------- CAMERA BINDINGS
static int l_get_camera_position(lua_State* L) {
    ScriptSystem* scriptSystem = getScriptSystem(L);
    if (scriptSystem == nullptr || scriptSystem->getRuntimeCamera() == nullptr) {
        lua_pushnil(L);
        return 1;
    }

    const glm::vec2 position = scriptSystem->getRuntimeCamera()->getPosition();
    lua_pushnumber(L, position.x);
    lua_pushnumber(L, position.y);
    return 2;
}

static int l_set_camera_position(lua_State* L) {
    ScriptSystem* scriptSystem = getScriptSystem(L);
    if (scriptSystem == nullptr || scriptSystem->getRuntimeCamera() == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const float x = static_cast<float>(luaL_checknumber(L, 1));
    const float y = static_cast<float>(luaL_checknumber(L, 2));
    scriptSystem->getRuntimeCamera()->setPosition({x, y});

    lua_pushboolean(L, 1);
    return 1;
}

static int l_set_camera_target_entity(lua_State* L) {
    ScriptSystem* scriptSystem = getScriptSystem(L);
    if (scriptSystem == nullptr ||
        scriptSystem->getRuntimeCameraFollowState() == nullptr ||
        scriptSystem->getRuntimeScene() == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* entityId = luaL_checkstring(L, 1);
    Entity* entity = scriptSystem->getRuntimeScene()->findEntityByID(entityId);
    if (entity == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    CameraFollowState* followState = scriptSystem->getRuntimeCameraFollowState();
    followState->followEntity = true;
    followState->targetEntityId = entityId;

    lua_pushboolean(L, 1);
    return 1;
}

static int l_clear_camera_target(lua_State* L) {
    ScriptSystem* scriptSystem = getScriptSystem(L);
    if (scriptSystem == nullptr || scriptSystem->getRuntimeCamera() == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    scriptSystem->getRuntimeCamera()->clearTargetPosition();

    CameraFollowState* followState = scriptSystem->getRuntimeCameraFollowState();
    if (followState != nullptr) {
        followState->followEntity = false;
        followState->targetEntityId.clear();
    }

    lua_pushboolean(L, 1);
    return 1;
}

static int l_get_viewport_size(lua_State* L) {
    ScriptSystem* scriptSystem = getScriptSystem(L);
    if (scriptSystem == nullptr || scriptSystem->getRuntimeCamera() == nullptr) {
        lua_pushnil(L);
        return 1;
    }

    Camera2D* camera = scriptSystem->getRuntimeCamera();
    lua_pushnumber(L, camera->getViewportWidth());
    lua_pushnumber(L, camera->getViewportHeight());
    return 2;
}

// --------- POST-FX BINDINGS
static int l_set_post_fade_amount(lua_State* L) {
    ScriptSystem* scriptSystem = getScriptSystem(L);
    if (scriptSystem == nullptr || scriptSystem->getPostFadeAmount() == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const float amount = static_cast<float>(luaL_checknumber(L, 1));
    *scriptSystem->getPostFadeAmount() = amount;

    lua_pushboolean(L, 1);
    return 1;
}

// --------- AUDIO BINDINGS
static int l_play_sound(lua_State* L) {
    ScriptSystem* scriptSystem = getScriptSystem(L);
    if (scriptSystem == nullptr || scriptSystem->getRuntimeAudioSystem() == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* soundId = luaL_checkstring(L, 1);
    // Optional volume param
    const float volume = lua_gettop(L) >= 2
        ? static_cast<float>(luaL_checknumber(L, 2))
        : 1.0f;

    const bool ok = scriptSystem->getRuntimeAudioSystem()->playSound(soundId, volume);
    lua_pushboolean(L, ok ? 1 : 0);
    return 1;
}

static int l_set_master_volume(lua_State* L) {
    ScriptSystem* scriptSystem = getScriptSystem(L);
    if (scriptSystem == nullptr || scriptSystem->getRuntimeAudioSystem() == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const float volume = static_cast<float>(luaL_checknumber(L, 1));
    scriptSystem->getRuntimeAudioSystem()->setMasterVolume(volume);

    lua_pushboolean(L, 1);
    return 1;
}

// --------- ENTITY BINDINGS
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

static int l_get_entity_centre(lua_State* L) {
    ScriptSystem* scriptSystem = getScriptSystem(L);
    Entity* entity = getEntityFromArg(L, scriptSystem, 1);
    if (entity == nullptr) {
        lua_pushnil(L);
        return 1;
    }

    const glm::vec2 centre = entity->transform.position + entity->transform.scale * 0.5f;
    lua_pushnumber(L, centre.x);
    lua_pushnumber(L, centre.y);
    return 2;
}

static int l_destroy_entity(lua_State* L) {
    ScriptSystem* scriptSystem = getScriptSystem(L);
    if (scriptSystem == nullptr || scriptSystem->getRuntimeScene() == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* entityId = luaL_checkstring(L, 1);
    Entity* entity = scriptSystem->getRuntimeScene()->findEntityByID(entityId);
    if (entity == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    scriptSystem->getRuntimeScene()->destroyEntity(entityId);

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

static int l_get_entities_in_box(lua_State* L) {
    ScriptSystem* scriptSystem = getScriptSystem(L);
    if (scriptSystem == nullptr || scriptSystem->getRuntimeScene() == nullptr) {
        lua_newtable(L);
        return 1;
    }

    const float x = static_cast<float>(luaL_checknumber(L, 1));
    const float y = static_cast<float>(luaL_checknumber(L, 2));
    const float width = static_cast<float>(luaL_checknumber(L, 3));
    const float height = static_cast<float>(luaL_checknumber(L, 4));

    const float minX = x;
    const float minY = y;
    const float maxX = x + width;
    const float maxY = y + height;

    lua_newtable(L);
    int outIndex = 1;

    for (const auto& entityPtr : scriptSystem->getRuntimeScene()->getEntities()) {
        if (entityPtr == nullptr)
            continue;

        const glm::vec2 boundsPos = entityPtr->getBoundsPosition();
        const glm::vec2 boundsSize = entityPtr->getBoundsSize();

        const float entityMinX = boundsPos.x;
        const float entityMinY = boundsPos.y;
        const float entityMaxX = boundsPos.x + boundsSize.x;
        const float entityMaxY = boundsPos.y + boundsSize.y;

        const bool overlaps =
            entityMaxX >= minX &&
            entityMinX <= maxX &&
            entityMaxY >= minY &&
            entityMinY <= maxY;

        if (!overlaps)
            continue;

        lua_pushstring(L, entityPtr->getID().c_str());
        lua_seti(L, -2, outIndex++);
    }

    return 1;
}

static int l_get_entity_data(lua_State* L) {
    ScriptSystem* scriptSystem = getScriptSystem(L);
    Entity* entity = getEntityFromArg(L, scriptSystem, 1);
    if (entity == nullptr) {
        lua_pushnil(L);
        return 1;
    }

    const char* key = luaL_checkstring(L, 2);
    const CustomValue* value = entity->getCustomData().get(key);
    if (value == nullptr) {
        lua_pushnil(L);
        return 1;
    }

    if (std::holds_alternative<bool>(*value)) {
        lua_pushboolean(L, std::get<bool>(*value) ? 1 : 0);
        return 1;
    }

    if (std::holds_alternative<int>(*value)) {
        lua_pushinteger(L, std::get<int>(*value));
        return 1;
    }

    if (std::holds_alternative<float>(*value)) {
        lua_pushnumber(L, std::get<float>(*value));
        return 1;
    }

    if (std::holds_alternative<std::string>(*value)) {
        lua_pushstring(L, std::get<std::string>(*value).c_str());
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

static int l_set_entity_data(lua_State* L) {
    ScriptSystem* scriptSystem = getScriptSystem(L);
    Entity* entity = getEntityFromArg(L, scriptSystem, 1);
    if (entity == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* key = luaL_checkstring(L, 2);

    if (lua_isboolean(L, 3)) {
        entity->getCustomData().set(key, lua_toboolean(L, 3) != 0);
    } else if (lua_isinteger(L, 3)) {
        entity->getCustomData().set(key, static_cast<int>(lua_tointeger(L, 3)));
    } else if (lua_isnumber(L, 3)) {
        entity->getCustomData().set(key, static_cast<float>(lua_tonumber(L, 3)));
    } else if (lua_isstring(L, 3)) {
        entity->getCustomData().set(key, std::string(lua_tostring(L, 3)));
    } else {
        lua_pushboolean(L, 0);
        return 1;
    }

    lua_pushboolean(L, 1);
    return 1;
}

// --------- INPUT BINDINGS
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

// --------- PARTICLE BINDINGS
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

// --------- TILE MAP BINDINGS
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

static int l_set_tile_region_from_grid(lua_State* L) {
    ScriptSystem* scriptSystem = getScriptSystem(L);
    if (scriptSystem == nullptr ||
        scriptSystem->getRuntimeScene() == nullptr ||
        scriptSystem->getAssetManager() == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    TileMap* map = scriptSystem->getRuntimeScene()->getTileMap();
    if (map == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* layerName = luaL_checkstring(L, 1);
    const int tileX = static_cast<int>(luaL_checkinteger(L, 2));
    const int tileY = static_cast<int>(luaL_checkinteger(L, 3));
    const char* textureId = luaL_checkstring(L, 4);
    const int frameX = static_cast<int>(luaL_checkinteger(L, 5));
    const int frameY = static_cast<int>(luaL_checkinteger(L, 6));
    const int columns = static_cast<int>(luaL_checkinteger(L, 7));
    const int rows = static_cast<int>(luaL_checkinteger(L, 8));

    if (!map->isTileInBounds(tileX, tileY)) {
        lua_pushboolean(L, 0);
        return 1;
    }

    Texture* texture = scriptSystem->getAssetManager()->getTexture(textureId);
    if (texture == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    TileVisualOverrideLayer* overrideLayer = map->getRuntime().getOverrideLayer(layerName);
    if (overrideLayer == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const TextureRegion region = makeRegionFromGrid(
        texture,
        frameX,
        frameY,
        columns,
        rows
    );

    const bool result = overrideLayer->set(tileX, tileY, TileVisual::fromRegion(region));
    lua_pushboolean(L, result ? 1 : 0);
    return 1;
}

static int l_clear_tile_override(lua_State* L) {
    ScriptSystem* scriptSystem = getScriptSystem(L);
    if (scriptSystem == nullptr || scriptSystem->getRuntimeScene() == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    TileMap* map = scriptSystem->getRuntimeScene()->getTileMap();
    if (map == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* layerName = luaL_checkstring(L, 1);
    const int tileX = static_cast<int>(luaL_checkinteger(L, 2));
    const int tileY = static_cast<int>(luaL_checkinteger(L, 3));

    TileVisualOverrideLayer* overrideLayer = map->getRuntime().getOverrideLayer(layerName);
    if (overrideLayer == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const bool result = overrideLayer->clearAt(tileX, tileY);
    lua_pushboolean(L, result ? 1 : 0);
    return 1;
}

static int l_set_tile_gid_override(lua_State* L) {
    ScriptSystem* scriptSystem = getScriptSystem(L);
    if (scriptSystem == nullptr || scriptSystem->getRuntimeScene() == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    TileMap* map = scriptSystem->getRuntimeScene()->getTileMap();
    if (map == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* layerName = luaL_checkstring(L, 1);
    const int tileX = static_cast<int>(luaL_checkinteger(L, 2));
    const int tileY = static_cast<int>(luaL_checkinteger(L, 3));
    const int gid = static_cast<int>(luaL_checkinteger(L, 4));

    if (!map->isTileInBounds(tileX, tileY)) {
        lua_pushboolean(L, 0);
        return 1;
    }

    TileVisualOverrideLayer* overrideLayer = map->getRuntime().getOverrideLayer(layerName);
    if (overrideLayer == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    TextureRegion region;
    if (!map->tryMakeRegionForGid(gid, region)) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const bool result = overrideLayer->set(tileX, tileY, TileVisual::fromRegion(region));
    lua_pushboolean(L, result ? 1 : 0);
    return 1;
}

static int l_set_tile_tileset_override(lua_State* L) {
    ScriptSystem* scriptSystem = getScriptSystem(L);
    if (scriptSystem == nullptr || scriptSystem->getRuntimeScene() == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    TileMap* map = scriptSystem->getRuntimeScene()->getTileMap();
    if (map == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* layerName = luaL_checkstring(L, 1);
    const int tileX = static_cast<int>(luaL_checkinteger(L, 2));
    const int tileY = static_cast<int>(luaL_checkinteger(L, 3));
    const char* tilesetName = luaL_checkstring(L, 4);
    const int localTileId = static_cast<int>(luaL_checkinteger(L, 5));

    if (!map->isTileInBounds(tileX, tileY)) {
        lua_pushboolean(L, 0);
        return 1;
    }

    TileVisualOverrideLayer* overrideLayer = map->getRuntime().getOverrideLayer(layerName);
    if (overrideLayer == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    TextureRegion region;
    if (!map->tryMakeRegionForTilesetTileId(tilesetName, localTileId, region)) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const bool result = overrideLayer->set(tileX, tileY, TileVisual::fromRegion(region));
    lua_pushboolean(L, result ? 1 : 0);
    return 1;
}

// --------- COROUTINE BINDINGS
static int l_wait_continue(lua_State* L, int status, lua_KContext ctx) {
    return 0;
}

static int l_wait(lua_State* L) {
    const float seconds = static_cast<float>(luaL_checknumber(L, 1));
    lua_pushstring(L, "wait_seconds");
    lua_pushnumber(L, seconds);
    return lua_yieldk(L, 2, 0, l_wait_continue);
}

static int l_wait_frame_continue(lua_State* L, int status, lua_KContext ctx) {
    return lua_gettop(L);
}

static int l_wait_frame(lua_State* L) {
    lua_pushstring(L, "wait_frame");
    return lua_yieldk(L, 1, 0, l_wait_frame_continue);
}

static int l_start_entity_coroutine(lua_State* L) {
    ScriptSystem* scriptSystem = getScriptSystem(L);
    if (scriptSystem == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* ownerEntityId = getEntityIdFromSelf(L, 1);
    luaL_checktype(L, 2, LUA_TFUNCTION);

    if (ownerEntityId == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const bool ok = scriptSystem->startEntityCoroutine(ownerEntityId, 2, 1);
    lua_pushboolean(L, ok ? 1 : 0);
    return 1;
}

static int l_start_global_coroutine(lua_State* L) {
    ScriptSystem* scriptSystem = getScriptSystem(L);
    if (scriptSystem == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    luaL_checktype(L, 1, LUA_TFUNCTION);

    const bool ok = scriptSystem->startGlobalCoroutine(1);
    lua_pushboolean(L, ok ? 1 : 0);
    return 1;
}


// --------- UI LABEL BINDINGS
static int l_ui_create_label(lua_State* L) {
    UISystem* uiSystem = getUISystem(L);
    if (uiSystem == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* id = luaL_checkstring(L, 1);
    UILabelRecord& label = uiSystem->createLabel(id);

    if (lua_gettop(L) >= 2 && lua_isstring(L, 2)) {
        label.group = lua_tostring(L, 2);
    }

    lua_pushboolean(L, 1);
    return 1;
}

static int l_ui_set_label_text(lua_State* L) {
    UISystem* uiSystem = getUISystem(L);
    if (uiSystem == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* id = luaL_checkstring(L, 1);
    const char* text = luaL_checkstring(L, 2);

    UILabelRecord* label = uiSystem->getLabel(id);
    if (label == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    label->text = text;
    lua_pushboolean(L, 1);
    return 1;
}

static int l_ui_set_label_position(lua_State* L) {
    UISystem* uiSystem = getUISystem(L);
    if (uiSystem == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* id = luaL_checkstring(L, 1);
    const float x = static_cast<float>(luaL_checknumber(L, 2));
    const float y = static_cast<float>(luaL_checknumber(L, 3));

    UILabelRecord* label = uiSystem->getLabel(id);
    if (label == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    label->position = {x, y};
    lua_pushboolean(L, 1);
    return 1;
}

static int l_ui_set_label_scale(lua_State* L) {
    UISystem* uiSystem = getUISystem(L);
    if (uiSystem == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* id = luaL_checkstring(L, 1);
    const float scale = static_cast<float>(luaL_checknumber(L, 2));

    UILabelRecord* label = uiSystem->getLabel(id);
    if (label == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    label->scale = scale;
    lua_pushboolean(L, 1);
    return 1;
}

static int l_ui_set_label_text_color(lua_State* L) {
    UISystem* uiSystem = getUISystem(L);
    if (uiSystem == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* id = luaL_checkstring(L, 1);
    const float r = static_cast<float>(luaL_checknumber(L, 2));
    const float g = static_cast<float>(luaL_checknumber(L, 3));
    const float b = static_cast<float>(luaL_checknumber(L, 4));
    const float a = static_cast<float>(luaL_optnumber(L, 5, 1.0));

    UILabelRecord* label = uiSystem->getLabel(id);
    if (label == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    label->textColor = {r, g, b, a};
    lua_pushboolean(L, 1);
    return 1;
}

static int l_ui_set_label_visible(lua_State* L) {
    UISystem* uiSystem = getUISystem(L);
    if (uiSystem == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* id = luaL_checkstring(L, 1);
    const bool visible = lua_toboolean(L, 2) != 0;

    UILabelRecord* label = uiSystem->getLabel(id);
    if (label == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    label->visible = visible;
    lua_pushboolean(L, 1);
    return 1;
}

static int l_ui_set_label_render_space(lua_State* L) {
    UISystem* uiSystem = getUISystem(L);
    if (uiSystem == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* id = luaL_checkstring(L, 1);
    const char* renderSpaceValue = luaL_checkstring(L, 2);

    UILabelRecord* label = uiSystem->getLabel(id);
    if (label == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    UIRenderSpace renderSpace;
    if (!parseUIRenderSpace(renderSpaceValue, renderSpace)) {
        lua_pushboolean(L, 0);
        return 1;
    }

    label->renderSpace = renderSpace;
    lua_pushboolean(L, 1);
    return 1;
}

static int l_ui_set_label_screen_anchor(lua_State* L) {
    UISystem* uiSystem = getUISystem(L);
    if (uiSystem == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* id = luaL_checkstring(L, 1);
    const float x = static_cast<float>(luaL_checknumber(L, 2));
    const float y = static_cast<float>(luaL_checknumber(L, 3));

    UILabelRecord* label = uiSystem->getLabel(id);
    if (label == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    label->screenLayout.enabled = true;
    label->screenLayout.anchor = {x, y};
    lua_pushboolean(L, 1);
    return 1;
}

static int l_ui_set_label_screen_pivot(lua_State* L) {
    UISystem* uiSystem = getUISystem(L);
    if (uiSystem == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* id = luaL_checkstring(L, 1);
    const float x = static_cast<float>(luaL_checknumber(L, 2));
    const float y = static_cast<float>(luaL_checknumber(L, 3));

    UILabelRecord* label = uiSystem->getLabel(id);
    if (label == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    label->screenLayout.enabled = true;
    label->screenLayout.pivot = {x, y};
    lua_pushboolean(L, 1);
    return 1;
}

static int l_ui_clear_label_screen_layout(lua_State* L) {
    UISystem* uiSystem = getUISystem(L);
    if (uiSystem == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* id = luaL_checkstring(L, 1);
    UILabelRecord* label = uiSystem->getLabel(id);
    if (label == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    label->screenLayout.enabled = false;
    lua_pushboolean(L, 1);
    return 1;
}

// --------- UI BUTTON BINDINGS
static int l_ui_create_button(lua_State* L) {
    UISystem* uiSystem = getUISystem(L);
    if (uiSystem == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* id = luaL_checkstring(L, 1);
    UIButtonRecord& button = uiSystem->createButton(id);

    if (lua_gettop(L) >= 2 && lua_isstring(L, 2)) {
        button.group = lua_tostring(L, 2);
    }

    lua_pushboolean(L, 1);
    return 1;
}

static int l_ui_set_button_text(lua_State* L) {
    UISystem* uiSystem = getUISystem(L);
    if (uiSystem == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* id = luaL_checkstring(L, 1);
    const char* text = luaL_checkstring(L, 2);

    UIButtonRecord* button = uiSystem->getButton(id);
    if (button == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    button->text = text;
    lua_pushboolean(L, 1);
    return 1;
}

static int l_ui_set_button_position(lua_State* L) {
    UISystem* uiSystem = getUISystem(L);
    if (uiSystem == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* id = luaL_checkstring(L, 1);
    const float x = static_cast<float>(luaL_checknumber(L, 2));
    const float y = static_cast<float>(luaL_checknumber(L, 3));

    UIButtonRecord* button = uiSystem->getButton(id);
    if (button == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    button->position = {x, y};
    lua_pushboolean(L, 1);
    return 1;
}

static int l_ui_set_button_size(lua_State* L) {
    UISystem* uiSystem = getUISystem(L);
    if (uiSystem == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* id = luaL_checkstring(L, 1);
    const float width = static_cast<float>(luaL_checknumber(L, 2));
    const float height = static_cast<float>(luaL_checknumber(L, 3));

    UIButtonRecord* button = uiSystem->getButton(id);
    if (button == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    button->size = {width, height};
    lua_pushboolean(L, 1);
    return 1;
}

static int l_ui_set_button_scale(lua_State* L) {
    UISystem* uiSystem = getUISystem(L);
    if (uiSystem == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* id = luaL_checkstring(L, 1);
    const float scale = static_cast<float>(luaL_checknumber(L, 2));

    UIButtonRecord* button = uiSystem->getButton(id);
    if (button == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    button->textScale = scale;
    lua_pushboolean(L, 1);
    return 1;
}

static int l_ui_set_button_visible(lua_State* L) {
    UISystem* uiSystem = getUISystem(L);
    if (uiSystem == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* id = luaL_checkstring(L, 1);
    const bool visible = lua_toboolean(L, 2) != 0;

    UIButtonRecord* button = uiSystem->getButton(id);
    if (button == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    button->visible = visible;
    lua_pushboolean(L, 1);
    return 1;
}

static int l_ui_set_button_screen_anchor(lua_State* L) {
    UISystem* uiSystem = getUISystem(L);
    if (uiSystem == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* id = luaL_checkstring(L, 1);
    const float x = static_cast<float>(luaL_checknumber(L, 2));
    const float y = static_cast<float>(luaL_checknumber(L, 3));

    UIButtonRecord* button = uiSystem->getButton(id);
    if (button == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    button->screenLayout.enabled = true;
    button->screenLayout.anchor = {x, y};
    lua_pushboolean(L, 1);
    return 1;
}

static int l_ui_set_button_screen_pivot(lua_State* L) {
    UISystem* uiSystem = getUISystem(L);
    if (uiSystem == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* id = luaL_checkstring(L, 1);
    const float x = static_cast<float>(luaL_checknumber(L, 2));
    const float y = static_cast<float>(luaL_checknumber(L, 3));

    UIButtonRecord* button = uiSystem->getButton(id);
    if (button == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    button->screenLayout.enabled = true;
    button->screenLayout.pivot = {x, y};
    lua_pushboolean(L, 1);
    return 1;
}

static int l_ui_set_button_background_color(lua_State* L) {
    UISystem* uiSystem = getUISystem(L);
    if (uiSystem == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* id = luaL_checkstring(L, 1);
    const float r = static_cast<float>(luaL_checknumber(L, 2));
    const float g = static_cast<float>(luaL_checknumber(L, 3));
    const float b = static_cast<float>(luaL_checknumber(L, 4));
    const float a = static_cast<float>(luaL_optnumber(L, 5, 1.0));

    UIButtonRecord* button = uiSystem->getButton(id);
    if (button == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    button->normal.backgroundColor = {r, g, b, a};
    lua_pushboolean(L, 1);
    return 1;
}

static int l_ui_set_button_hover_background_color(lua_State* L) {
    UISystem* uiSystem = getUISystem(L);
    if (uiSystem == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* id = luaL_checkstring(L, 1);
    const float r = static_cast<float>(luaL_checknumber(L, 2));
    const float g = static_cast<float>(luaL_checknumber(L, 3));
    const float b = static_cast<float>(luaL_checknumber(L, 4));
    const float a = static_cast<float>(luaL_optnumber(L, 5, 1.0));

    UIButtonRecord* button = uiSystem->getButton(id);
    if (button == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    button->hovered.backgroundColor = {r, g, b, a};
    lua_pushboolean(L, 1);
    return 1;
}

static int l_ui_set_button_pressed_background_color(lua_State* L) {
    UISystem* uiSystem = getUISystem(L);
    if (uiSystem == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* id = luaL_checkstring(L, 1);
    const float r = static_cast<float>(luaL_checknumber(L, 2));
    const float g = static_cast<float>(luaL_checknumber(L, 3));
    const float b = static_cast<float>(luaL_checknumber(L, 4));
    const float a = static_cast<float>(luaL_optnumber(L, 5, 1.0));

    UIButtonRecord* button = uiSystem->getButton(id);
    if (button == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    button->pressed.backgroundColor = {r, g, b, a};
    lua_pushboolean(L, 1);
    return 1;
}

static int l_ui_was_button_clicked(lua_State* L) {
    UISystem* uiSystem = getUISystem(L);
    if (uiSystem == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* id = luaL_checkstring(L, 1);
    UIButtonRecord* button = uiSystem->getButton(id);
    if (button == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    lua_pushboolean(L, button->clicked ? 1 : 0);
    return 1;
}

static int l_ui_is_button_hovered(lua_State* L) {
    UISystem* uiSystem = getUISystem(L);
    if (uiSystem == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* id = luaL_checkstring(L, 1);
    UIButtonRecord* button = uiSystem->getButton(id);
    if (button == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    lua_pushboolean(L, button->hoveredNow ? 1 : 0);
    return 1;
}

// --------- UI SLOT STRIP BINDINGS
static int l_ui_create_slot_strip(lua_State* L) {
    UISystem* uiSystem = getUISystem(L);
    if (uiSystem == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* id = luaL_checkstring(L, 1);
    UISlotStripRecord& strip = uiSystem->createSlotStrip(id);

    if (lua_gettop(L) >= 2 && lua_isstring(L, 2)) {
        strip.group = lua_tostring(L, 2);
    }

    lua_pushboolean(L, 1);
    return 1;
}

static int l_ui_set_slot_strip_position(lua_State* L) {
    UISystem* uiSystem = getUISystem(L);
    if (uiSystem == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* id = luaL_checkstring(L, 1);
    const float x = static_cast<float>(luaL_checknumber(L, 2));
    const float y = static_cast<float>(luaL_checknumber(L, 3));

    UISlotStripRecord* strip = uiSystem->getSlotStrip(id);
    if (strip == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    strip->position = {x, y};
    lua_pushboolean(L, 1);
    return 1;
}

static int l_ui_set_slot_strip_slot_count(lua_State* L) {
    UISystem* uiSystem = getUISystem(L);
    if (uiSystem == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* id = luaL_checkstring(L, 1);
    const int count = static_cast<int>(luaL_checkinteger(L, 2));

    UISlotStripRecord* strip = uiSystem->getSlotStrip(id);
    if (strip == nullptr || count < 0) {
        lua_pushboolean(L, 0);
        return 1;
    }

    strip->slots.resize(static_cast<size_t>(count));
    if (strip->selectedIndex >= count) {
        strip->selectedIndex = count > 0 ? count - 1 : 0;
    }

    lua_pushboolean(L, 1);
    return 1;
}

static int l_ui_set_slot_strip_selected(lua_State* L) {
    UISystem* uiSystem = getUISystem(L);
    if (uiSystem == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* id = luaL_checkstring(L, 1);
    const int index = static_cast<int>(luaL_checkinteger(L, 2));

    UISlotStripRecord* strip = uiSystem->getSlotStrip(id);
    if (strip == nullptr || strip->slots.empty()) {
        lua_pushboolean(L, 0);
        return 1;
    }

    strip->selectedIndex = std::max(0, std::min(index, static_cast<int>(strip->slots.size()) - 1));
    lua_pushboolean(L, 1);
    return 1;
}

static int l_ui_set_slot_strip_slot_texture(lua_State* L) {
    ScriptSystem* scriptSystem = getScriptSystem(L);
    UISystem* uiSystem = getUISystem(L);
    if (scriptSystem == nullptr || uiSystem == nullptr || scriptSystem->getAssetManager() == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* id = luaL_checkstring(L, 1);
    const int slotIndex = static_cast<int>(luaL_checkinteger(L, 2));
    const char* textureId = luaL_checkstring(L, 3);

    UISlotStripRecord* strip = uiSystem->getSlotStrip(id);
    if (strip == nullptr || slotIndex < 0 || slotIndex >= static_cast<int>(strip->slots.size())) {
        lua_pushboolean(L, 0);
        return 1;
    }

    Texture* texture = scriptSystem->getAssetManager()->getTexture(textureId);
    if (texture == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    UISlotStripItemRecord& slot = strip->slots[static_cast<size_t>(slotIndex)];
    slot.occupied = true;
    slot.icon = TextureRegion::full(texture);
    slot.tint = {1.0f, 1.0f, 1.0f, 1.0f};

    lua_pushboolean(L, 1);
    return 1;
}

static int l_ui_set_slot_strip_slot_tileset_tile(lua_State* L) {
    ScriptSystem* scriptSystem = getScriptSystem(L);
    UISystem* uiSystem = getUISystem(L);
    if (scriptSystem == nullptr || uiSystem == nullptr || scriptSystem->getRuntimeScene() == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    TileMap* map = scriptSystem->getRuntimeScene()->getTileMap();
    if (map == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* id = luaL_checkstring(L, 1);
    const int slotIndex = static_cast<int>(luaL_checkinteger(L, 2));
    const char* tilesetName = luaL_checkstring(L, 3);
    const int localTileId = static_cast<int>(luaL_checkinteger(L, 4));

    UISlotStripRecord* strip = uiSystem->getSlotStrip(id);
    if (strip == nullptr || slotIndex < 0 || slotIndex >= static_cast<int>(strip->slots.size())) {
        lua_pushboolean(L, 0);
        return 1;
    }

    TextureRegion region;
    if (!map->tryMakeRegionForTilesetTileId(tilesetName, localTileId, region)) {
        lua_pushboolean(L, 0);
        return 1;
    }

    UISlotStripItemRecord& slot = strip->slots[static_cast<size_t>(slotIndex)];
    slot.occupied = true;
    slot.icon = region;
    slot.tint = {1.0f, 1.0f, 1.0f, 1.0f};

    lua_pushboolean(L, 1);
    return 1;
}

static int l_ui_set_slot_strip_visible(lua_State* L) {
    UISystem* uiSystem = getUISystem(L);
    if (uiSystem == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* id = luaL_checkstring(L, 1);
    const bool visible = lua_toboolean(L, 2) != 0;

    UISlotStripRecord* strip = uiSystem->getSlotStrip(id);
    if (strip == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    strip->visible = visible;
    lua_pushboolean(L, 1);
    return 1;
}

static int l_ui_set_slot_strip_render_space(lua_State* L) {
    UISystem* uiSystem = getUISystem(L);
    if (uiSystem == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* id = luaL_checkstring(L, 1);
    const char* renderSpaceValue = luaL_checkstring(L, 2);

    UISlotStripRecord* strip = uiSystem->getSlotStrip(id);
    if (strip == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    UIRenderSpace renderSpace;
    if (!parseUIRenderSpace(renderSpaceValue, renderSpace)) {
        lua_pushboolean(L, 0);
        return 1;
    }

    strip->renderSpace = renderSpace;
    lua_pushboolean(L, 1);
    return 1;
}

// --------- UI PROGRESS BAR BINDINGS
static int l_ui_create_progress_bar(lua_State* L) {
    UISystem* uiSystem = getUISystem(L);
    if (uiSystem == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* id = luaL_checkstring(L, 1);
    UIProgressBarRecord& bar = uiSystem->createProgressBar(id);

    if (lua_gettop(L) >= 2 && lua_isstring(L, 2)) {
        bar.group = lua_tostring(L, 2);
    }

    lua_pushboolean(L, 1);
    return 1;
}

static int l_ui_set_progress_bar_render_space(lua_State* L) {
    UISystem* uiSystem = getUISystem(L);
    if (uiSystem == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* id = luaL_checkstring(L, 1);
    const char* renderSpaceValue = luaL_checkstring(L, 2);

    UIProgressBarRecord* bar = uiSystem->getProgressBar(id);
    if (bar == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    UIRenderSpace renderSpace;
    if (!parseUIRenderSpace(renderSpaceValue, renderSpace)) {
        lua_pushboolean(L, 0);
        return 1;
    }

    bar->renderSpace = renderSpace;
    lua_pushboolean(L, 1);
    return 1;
}

static int l_ui_set_progress_bar_position(lua_State* L) {
    UISystem* uiSystem = getUISystem(L);
    if (uiSystem == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* id = luaL_checkstring(L, 1);
    const float x = static_cast<float>(luaL_checknumber(L, 2));
    const float y = static_cast<float>(luaL_checknumber(L, 3));

    UIProgressBarRecord* bar = uiSystem->getProgressBar(id);
    if (bar == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    bar->position = {x, y};
    lua_pushboolean(L, 1);
    return 1;
}

static int l_ui_set_progress_bar_size(lua_State* L) {
    UISystem* uiSystem = getUISystem(L);
    if (uiSystem == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* id = luaL_checkstring(L, 1);
    const float width = static_cast<float>(luaL_checknumber(L, 2));
    const float height = static_cast<float>(luaL_checknumber(L, 3));

    UIProgressBarRecord* bar = uiSystem->getProgressBar(id);
    if (bar == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    bar->size = {width, height};
    lua_pushboolean(L, 1);
    return 1;
}

static int l_ui_set_progress_bar_value(lua_State* L) {
    UISystem* uiSystem = getUISystem(L);
    if (uiSystem == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* id = luaL_checkstring(L, 1);
    const float value = static_cast<float>(luaL_checknumber(L, 2));

    UIProgressBarRecord* bar = uiSystem->getProgressBar(id);
    if (bar == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    bar->value = value;
    lua_pushboolean(L, 1);
    return 1;
}

static int l_ui_set_progress_bar_range(lua_State* L) {
    UISystem* uiSystem = getUISystem(L);
    if (uiSystem == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* id = luaL_checkstring(L, 1);
    const float minValue = static_cast<float>(luaL_checknumber(L, 2));
    const float maxValue = static_cast<float>(luaL_checknumber(L, 3));

    UIProgressBarRecord* bar = uiSystem->getProgressBar(id);
    if (bar == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    bar->minValue = minValue;
    bar->maxValue = maxValue;
    lua_pushboolean(L, 1);
    return 1;
}

static int l_ui_set_progress_bar_visible(lua_State* L) {
    UISystem* uiSystem = getUISystem(L);
    if (uiSystem == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* id = luaL_checkstring(L, 1);
    const bool visible = lua_toboolean(L, 2) != 0;

    UIProgressBarRecord* bar = uiSystem->getProgressBar(id);
    if (bar == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    bar->visible = visible;
    lua_pushboolean(L, 1);
    return 1;
}

// --------- UI GENERAL BINDINGS
static int l_ui_set_group_visible(lua_State* L) {
    UISystem* uiSystem = getUISystem(L);
    if (uiSystem == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* group = luaL_checkstring(L, 1);
    const bool visible = lua_toboolean(L, 2) != 0;

    uiSystem->setGroupVisible(group, visible);
    lua_pushboolean(L, 1);
    return 1;
}
// --------- SCREEN STATE FLOW BINDINGS
static int l_screenflow_show_base(lua_State* L) {
    ScriptSystem* scriptSystem = getScriptSystem(L);
    if (scriptSystem == nullptr || scriptSystem->getRuntimeScreenFlowSystem() == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* screenId = luaL_checkstring(L, 1);
    const bool ok = scriptSystem->getRuntimeScreenFlowSystem()->showBase(screenId);
    lua_pushboolean(L, ok ? 1 : 0);
    return 1;
}

static int l_screenflow_show_overlay(lua_State* L) {
    ScriptSystem* scriptSystem = getScriptSystem(L);
    if (scriptSystem == nullptr || scriptSystem->getRuntimeScreenFlowSystem() == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* screenId = luaL_checkstring(L, 1);
    const bool ok = scriptSystem->getRuntimeScreenFlowSystem()->showOverlay(screenId);
    lua_pushboolean(L, ok ? 1 : 0);
    return 1;
}

static int l_screenflow_clear_overlay(lua_State* L) {
    ScriptSystem* scriptSystem = getScriptSystem(L);
    if (scriptSystem == nullptr || scriptSystem->getRuntimeScreenFlowSystem() == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const bool ok = scriptSystem->getRuntimeScreenFlowSystem()->clearOverlay();
    lua_pushboolean(L, ok ? 1 : 0);
    return 1;
}

static int l_screenflow_toggle_overlay(lua_State* L) {
    ScriptSystem* scriptSystem = getScriptSystem(L);
    if (scriptSystem == nullptr || scriptSystem->getRuntimeScreenFlowSystem() == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* screenId = luaL_checkstring(L, 1);
    const bool ok = scriptSystem->getRuntimeScreenFlowSystem()->toggleOverlay(screenId);
    lua_pushboolean(L, ok ? 1 : 0);
    return 1;
}

void registerEngineBindings(lua_State* luaState, ScriptSystem& scriptSystem) {
    // Setup engine global table
    lua_newtable(luaState);

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_get_camera_position, 1);
    lua_setfield(luaState, -2, "get_camera_position");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_set_camera_position, 1);
    lua_setfield(luaState, -2, "set_camera_position");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_set_camera_target_entity, 1);
    lua_setfield(luaState, -2, "set_camera_target_entity");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_clear_camera_target, 1);
    lua_setfield(luaState, -2, "clear_camera_target");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_get_viewport_size, 1);
    lua_setfield(luaState, -2, "get_viewport_size");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_set_post_fade_amount, 1);
    lua_setfield(luaState, -2, "set_post_fade_amount");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_play_sound, 1);
    lua_setfield(luaState, -2, "play_sound");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_set_master_volume, 1);
    lua_setfield(luaState, -2, "set_master_volume");

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
    lua_pushcclosure(luaState, l_get_entity_centre, 1);
    lua_setfield(luaState, -2, "get_entity_centre");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_destroy_entity, 1);
    lua_setfield(luaState, -2, "destroy_entity");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_play_entity_animation, 1);
    lua_setfield(luaState, -2, "play_entity_animation");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_set_entity_flip_x, 1);
    lua_setfield(luaState, -2, "set_entity_flip_x");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_get_entities_in_box, 1);
    lua_setfield(luaState, -2, "get_entities_in_box");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_get_entity_data, 1);
    lua_setfield(luaState, -2, "get_entity_data");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_set_entity_data, 1);
    lua_setfield(luaState, -2, "set_entity_data");

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

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_set_tile_region_from_grid, 1);
    lua_setfield(luaState, -2, "set_tile_region_from_grid");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_clear_tile_override, 1);
    lua_setfield(luaState, -2, "clear_tile_override");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_set_tile_gid_override, 1);
    lua_setfield(luaState, -2, "set_tile_gid_override");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_set_tile_tileset_override, 1);
    lua_setfield(luaState, -2, "set_tile_tileset_override");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_start_entity_coroutine, 1);
    lua_setfield(luaState, -2, "start_entity_coroutine");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_start_global_coroutine, 1);
    lua_setfield(luaState, -2, "start_global_coroutine");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_wait, 1);
    lua_setfield(luaState, -2, "wait");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_wait_frame, 1);
    lua_setfield(luaState, -2, "wait_frame");

    lua_setglobal(luaState, "engine");

    // Setup UI global table
    lua_newtable(luaState);

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_ui_create_label, 1);
    lua_setfield(luaState, -2, "create_label");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_ui_set_label_text, 1);
    lua_setfield(luaState, -2, "set_label_text");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_ui_set_label_text_color, 1);
    lua_setfield(luaState, -2, "set_label_text_color");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_ui_set_label_position, 1);
    lua_setfield(luaState, -2, "set_label_position");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_ui_set_label_scale, 1);
    lua_setfield(luaState, -2, "set_label_scale");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_ui_set_label_visible, 1);
    lua_setfield(luaState, -2, "set_label_visible");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_ui_set_label_render_space, 1);
    lua_setfield(luaState, -2, "set_label_render_space");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_ui_set_label_screen_anchor, 1);
    lua_setfield(luaState, -2, "set_label_screen_anchor");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_ui_set_label_screen_pivot, 1);
    lua_setfield(luaState, -2, "set_label_screen_pivot");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_ui_clear_label_screen_layout, 1);
    lua_setfield(luaState, -2, "clear_label_screen_layout");

        lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_ui_create_button, 1);
    lua_setfield(luaState, -2, "create_button");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_ui_set_button_text, 1);
    lua_setfield(luaState, -2, "set_button_text");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_ui_set_button_position, 1);
    lua_setfield(luaState, -2, "set_button_position");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_ui_set_button_size, 1);
    lua_setfield(luaState, -2, "set_button_size");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_ui_set_button_scale, 1);
    lua_setfield(luaState, -2, "set_button_scale");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_ui_set_button_visible, 1);
    lua_setfield(luaState, -2, "set_button_visible");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_ui_set_button_screen_anchor, 1);
    lua_setfield(luaState, -2, "set_button_screen_anchor");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_ui_set_button_screen_pivot, 1);
    lua_setfield(luaState, -2, "set_button_screen_pivot");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_ui_set_button_background_color, 1);
    lua_setfield(luaState, -2, "set_button_background_color");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_ui_set_button_hover_background_color, 1);
    lua_setfield(luaState, -2, "set_button_hover_background_color");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_ui_set_button_pressed_background_color, 1);
    lua_setfield(luaState, -2, "set_button_pressed_background_color");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_ui_was_button_clicked, 1);
    lua_setfield(luaState, -2, "was_button_clicked");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_ui_is_button_hovered, 1);
    lua_setfield(luaState, -2, "is_button_hovered");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_ui_create_slot_strip, 1);
    lua_setfield(luaState, -2, "create_slot_strip");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_ui_set_slot_strip_position, 1);
    lua_setfield(luaState, -2, "set_slot_strip_position");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_ui_set_slot_strip_slot_count, 1);
    lua_setfield(luaState, -2, "set_slot_strip_slot_count");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_ui_set_slot_strip_selected, 1);
    lua_setfield(luaState, -2, "set_slot_strip_selected");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_ui_set_slot_strip_slot_texture, 1);
    lua_setfield(luaState, -2, "set_slot_strip_slot_texture");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_ui_set_slot_strip_slot_tileset_tile, 1);
    lua_setfield(luaState, -2, "set_slot_strip_slot_tileset_tile");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_ui_set_slot_strip_visible, 1);
    lua_setfield(luaState, -2, "set_slot_strip_visible");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_ui_set_slot_strip_render_space, 1);
    lua_setfield(luaState, -2, "set_slot_strip_render_space");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_ui_create_progress_bar, 1);
    lua_setfield(luaState, -2, "create_progress_bar");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_ui_set_progress_bar_render_space, 1);
    lua_setfield(luaState, -2, "set_progress_bar_render_space");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_ui_set_progress_bar_position, 1);
    lua_setfield(luaState, -2, "set_progress_bar_position");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_ui_set_progress_bar_size, 1);
    lua_setfield(luaState, -2, "set_progress_bar_size");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_ui_set_progress_bar_value, 1);
    lua_setfield(luaState, -2, "set_progress_bar_value");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_ui_set_progress_bar_range, 1);
    lua_setfield(luaState, -2, "set_progress_bar_range");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_ui_set_progress_bar_visible, 1);
    lua_setfield(luaState, -2, "set_progress_bar_visible");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_ui_set_group_visible, 1);
    lua_setfield(luaState, -2, "set_group_visible");

    lua_setglobal(luaState, "ui");

    // Setup screenflow global table
    lua_newtable(luaState);

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_screenflow_show_base, 1);
    lua_setfield(luaState, -2, "show_base");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_screenflow_show_overlay, 1);
    lua_setfield(luaState, -2, "show_overlay");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_screenflow_clear_overlay, 1);
    lua_setfield(luaState, -2, "clear_overlay");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_screenflow_toggle_overlay, 1);
    lua_setfield(luaState, -2, "toggle_overlay");

    lua_setglobal(luaState, "screenflow");
}