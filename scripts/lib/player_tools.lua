local inventory = require("scripts.lib.inventory")
local farmState = require("scripts.lib.farm_state")
local items = require("scripts.items")
local playerHotbar = require("scripts.lib.player_hotbar")

local M = {}

local function get_selected_item_def(self)
    if self.selected_tool == nil then
        return nil
    end

    return items[self.selected_tool]
end

function M.get_selected_item_number(self, key, defaultValue)
    local itemDef = get_selected_item_def(self)
    if itemDef == nil then
        return defaultValue
    end

    local value = itemDef[key]
    if type(value) == "number" then
        return value
    end

    return defaultValue
end

function M.get_selected_item_type(self)
    local itemDef = get_selected_item_def(self)
    if itemDef == nil then
        return nil
    end

    return itemDef.type
end

local function get_tile_centre(tileX, tileY)
    local worldX, worldY = engine.get_tile_world_position(tileX, tileY)
    if worldX == nil then
        return nil
    end

    return worldX + 8, worldY + 8
end

function M.is_tile_in_use_range(self, tileX, tileY)
    local playerX, playerY = engine.get_entity_centre(self.id)
    local tileCentreX, tileCentreY = get_tile_centre(tileX, tileY)
    if playerX == nil or tileCentreX == nil then
        return false
    end

    local dx = tileCentreX - playerX
    local dy = tileCentreY - playerY
    local distance = math.sqrt(dx * dx + dy * dy)

    local useRange = M.get_selected_item_number(self, "use_range", 0)
    return useRange > 0 and distance <= useRange
end

function M.can_preview_selected_item(self)
    local itemType = M.get_selected_item_type(self)
    return itemType == "tool" or itemType == "seed"
end

function M.can_use_selected_item_on_tile(self, tileX, tileY)
    if not M.can_preview_selected_item(self) then
        return false
    end

    -- TODO:
    -- Later, re-enable
    -- if not farmState.is_inside_farm_area(tileX, tileY) then
    --     return false
    -- end

    if not M.is_tile_in_use_range(self, tileX, tileY) then
        return false
    end

    if self.selected_tool == "shovel" then
        return not farmState.is_tilled(tileX, tileY)
    elseif self.selected_tool == "potato_seeds" then
        return farmState.is_tilled(tileX, tileY) and farmState.get_crop(tileX, tileY) == nil
    elseif self.selected_tool == "bucket" then
        return true
    end

    return false
end

function M.try_harvest_crop(tileX, tileY)
    if not farmState.is_crop_mature(tileX, tileY) then
        return false
    end

    local itemId = farmState.get_crop_harvest_item(tileX, tileY)
    if itemId == nil then
        return false
    end

    local worldX, worldY = engine.get_tile_world_position(tileX, tileY)
    farmState.clear_crop(tileX, tileY)

    local dropId = engine.spawn_entity(itemId, worldX + 8, worldY + 8)
    if dropId ~= nil then
        engine.set_entity_data(dropId, "type", "pickup")
        engine.set_entity_data(dropId, "count", 1)
        engine.set_entity_data(dropId, "item_type", itemId)
    end

    engine.play_sound("pickup_item", 0.7)
    engine.emit_particles("dust_puff_0", worldX + 8, worldY + 8, 10)
    return true
end

function M.commit_selected_tool_use(self, tileX, tileY, mouseX, mouseY)
    if self.selected_tool == "shovel" then
        farmState.till(tileX, tileY)
        engine.play_sound("shovel", 0.7)
        engine.emit_particles("dust_puff_0", mouseX, mouseY + 8, 14)

        self.action_locked = true
        engine.set_entity_animation_locked(self.id, true)
        engine.play_entity_animation(self.id, "player_hoe_right", true)
        return true

    elseif self.selected_tool == "potato_seeds" then
        local removed = inventory.remove_item("inventory", "potato_seeds", 1)
        if removed > 0 then
            local day, hour, minute = engine.get_game_time()
            farmState.plant(tileX, tileY, "potato_seeds", day, hour, minute)
            playerHotbar.refresh_ui()
            return true
        end
    end

    return false
end

return M