local M = {}

local GRID_ID = "farm_clutter"
local FARM_MAP = "assets/farmMap.tmx"

local REGION_X0 = 2
local REGION_Y0 = 100
local REGION_X1 = 20
local REGION_Y1 = 114

local CLUTTER_DEFS = {
    weed = {
        entity = "weed",
        tool = "shovel",
        drop_entity = "fiber",
        footprint_w = 1,
        footprint_h = 1,
        chance = 0.16
    },
    trunk = {
        entity = "trunk",
        tool = "axe",
        drop_entity = "wood",
        footprint_w = 1,
        footprint_h = 1,
        chance = 0.08
    },
    stone = {
        entity = "rocks",
        tool = "pickaxe",
        drop_entity = "stone",
        footprint_w = 1,
        footprint_h = 1,
        chance = 0.08
    },
    oak_tree = {
        entity = "oak_tree",
        tool = "axe",
        drop_entity = "wood",
        footprint_w = 2,
        footprint_h = 3,
        chance = 0.03
    },
    pine_tree = {
        entity = "pine_tree",
        tool = "axe",
        drop_entity = "wood",
        footprint_w = 2,
        footprint_h = 3,
        chance = 0.03
    }
}

local function key(x, y)
    return tostring(x) .. "," .. tostring(y)
end

local function get_def(id)
    return CLUTTER_DEFS[id]
end

function M.is_farm_map(mapPath)
    return mapPath == FARM_MAP
end

function M.ensure()
    if not grid.has(GRID_ID) then
        grid.create(GRID_ID, 120, 120)
    end
end

local function is_occupied(tileX, tileY)
    return grid.get_data(GRID_ID, tileX, tileY, "occupied") == true
end

local function set_cell(tileX, tileY, clutterId, anchorX, anchorY, occupied, isAnchor)
    grid.set_data(GRID_ID, tileX, tileY, "clutter_id", clutterId or "")
    grid.set_data(GRID_ID, tileX, tileY, "anchor_x", anchorX or -1)
    grid.set_data(GRID_ID, tileX, tileY, "anchor_y", anchorY or -1)
    grid.set_data(GRID_ID, tileX, tileY, "occupied", occupied == true)
    grid.set_data(GRID_ID, tileX, tileY, "is_anchor", isAnchor == true)
end

local function clear_footprint(tileX, tileY)
    local clutterId = grid.get_data(GRID_ID, tileX, tileY, "clutter_id")
    if clutterId == nil or clutterId == "" then
        return
    end

    local anchorX = grid.get_data(GRID_ID, tileX, tileY, "anchor_x")
    local anchorY = grid.get_data(GRID_ID, tileX, tileY, "anchor_y")
    if anchorX == nil or anchorY == nil or anchorX < 0 or anchorY < 0 then
        return
    end

    local def = get_def(clutterId)
    if def == nil then
        return
    end

    for y = anchorY, anchorY + def.footprint_h - 1 do
        for x = anchorX, anchorX + def.footprint_w - 1 do
            set_cell(x, y, "", -1, -1, false, false)
        end
    end
end

local function can_place(clutterId, tileX, tileY)
    local def = get_def(clutterId)
    if def == nil then
        return false
    end

    for y = tileY, tileY + def.footprint_h - 1 do
        for x = tileX, tileX + def.footprint_w - 1 do
            if x > REGION_X1 or y > REGION_Y1 or is_occupied(x, y) then
                return false
            end
        end
    end

    return true
end

local function place_clutter(clutterId, tileX, tileY)
    local def = get_def(clutterId)
    if def == nil then
        return
    end

    for y = tileY, tileY + def.footprint_h - 1 do
        for x = tileX, tileX + def.footprint_w - 1 do
            set_cell(x, y, clutterId, tileX, tileY, true, x == tileX and y == tileY)
        end
    end
end

function M.generate_once()
    M.ensure()

    if grid.get_data(GRID_ID, 0, 0, "generated") == true then
        return
    end

    math.randomseed(1337)

    local order = { "oak_tree", "pine_tree", "stone", "trunk", "weed" }

    for y = REGION_Y0, REGION_Y1 do
        for x = REGION_X0, REGION_X1 do
            if not is_occupied(x, y) then
                for _, clutterId in ipairs(order) do
                    local def = CLUTTER_DEFS[clutterId]
                    if math.random() < def.chance and can_place(clutterId, x, y) then
                        place_clutter(clutterId, x, y)
                        break
                    end
                end
            end
        end
    end

    grid.set_data(GRID_ID, 0, 0, "generated", true)
end

function M.restore()
    M.ensure()

    for y = REGION_Y0, REGION_Y1 do
        for x = REGION_X0, REGION_X1 do
            if grid.get_data(GRID_ID, x, y, "is_anchor") == true then
                local clutterId = grid.get_data(GRID_ID, x, y, "clutter_id")
                local def = get_def(clutterId)
                if def ~= nil then
                    local worldX, worldY = engine.get_tile_world_position(x, y)
                    local entityId = engine.spawn_entity(def.entity, worldX, worldY)
                    if entityId ~= nil then
                        engine.set_entity_data(entityId, "breakable_id", clutterId)
                        engine.set_entity_data(entityId, "anchor_x", x)
                        engine.set_entity_data(entityId, "anchor_y", y)
                        grid.set_data(GRID_ID, x, y, "entity_id", entityId)
                    end
                end
            end
        end
    end
end

function M.can_break_with_tool(tileX, tileY, toolId)
    M.ensure()

    local clutterId = grid.get_data(GRID_ID, tileX, tileY, "clutter_id")
    if clutterId == nil or clutterId == "" then
        return false
    end

    local def = get_def(clutterId)
    return def ~= nil and def.tool == toolId
end

function M.break_at(tileX, tileY)
    M.ensure()

    local clutterId = grid.get_data(GRID_ID, tileX, tileY, "clutter_id")
    if clutterId == nil or clutterId == "" then
        return false
    end

    local anchorX = grid.get_data(GRID_ID, tileX, tileY, "anchor_x")
    local anchorY = grid.get_data(GRID_ID, tileX, tileY, "anchor_y")
    local def = get_def(clutterId)
    if def == nil or anchorX == nil or anchorY == nil then
        return false
    end

    local entityId = grid.get_data(GRID_ID, anchorX, anchorY, "entity_id")
    if entityId ~= nil and entityId ~= "" then
        engine.destroy_entity(entityId)
    end

    local worldX, worldY = engine.get_tile_world_position(anchorX, anchorY)
    local dropId = engine.spawn_entity(def.drop_entity, worldX + 8, worldY + 8)
    if dropId ~= nil then
        engine.set_entity_data(dropId, "type", "pickup")
    end

    clear_footprint(anchorX, anchorY)
    engine.emit_particles("dust_puff_0", worldX + 8, worldY + 8, 12)
    return true
end

return M