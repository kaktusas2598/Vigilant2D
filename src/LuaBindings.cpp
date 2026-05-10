#include "LuaBindings.hpp"

#include <cmath>

#include "ScriptSystem.hpp"
#include "Scene.hpp"
#include "AnimatedSprite.hpp"
#include "AssetManager.hpp"
#include "AnimationRegistry.hpp"
#include "ParticleEmitter.hpp"
#include "ParticleEmitterRegistry.hpp"
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

// --------- INPUT BINDINGS
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

void registerEngineBindings(lua_State* luaState, ScriptSystem& scriptSystem) {

    // Setup engine global table
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
    lua_pushcclosure(luaState, l_ui_set_label_position, 1);
    lua_setfield(luaState, -2, "set_label_position");

    lua_pushlightuserdata(luaState, &scriptSystem);
    lua_pushcclosure(luaState, l_ui_set_label_visible, 1);
    lua_setfield(luaState, -2, "set_label_visible");

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
    lua_pushcclosure(luaState, l_ui_set_group_visible, 1);
    lua_setfield(luaState, -2, "set_group_visible");

    lua_setglobal(luaState, "ui");

}