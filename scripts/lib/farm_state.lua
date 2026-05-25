local M = {}

local FARM_MAP = "assets/farmMap.tmx"

local GROUND_TILESET = "cozyFarm"
local CROPS_TILESET = "cozy_farm_crops"

local GROUND_TILLED_TILE = 491
local FARMLAND_TILLED_TILE = 494

local POTATO_STAGE_0_TILE = 31
local POTATO_STAGE_1_TILE = 32
local POTATO_STAGE_2_TILE = 33
local POTATO_STAGE_3_TILE = 34

local planted_tiles = {}

local function tile_key(x, y)
    return tostring(x) .. "," .. tostring(y)
end

local function mark_planted(x, y)
    planted_tiles[tile_key(x, y)] = { x = x, y = y }
end

local function unmark_planted(x, y)
    planted_tiles[tile_key(x, y)] = nil
end

local function crop_stage_tile(cropId, stage)
    if cropId == "potato_seeds" then
        if stage == 0 then
            return POTATO_STAGE_0_TILE
        elseif stage == 1 then
            return POTATO_STAGE_1_TILE
        elseif stage == 2 then
            return POTATO_STAGE_2_TILE
        else
            return POTATO_STAGE_3_TILE
        end
    end

    return nil
end

local function to_total_minutes(day, hour, minute)
    return ((day - 1) * 1440) + hour * 60 + minute
end

function M.is_farm_map(mapPath)
    return mapPath == FARM_MAP
end

function M.ensure()
    if not grid.has("farm") then
        grid.create("farm", 120, 120)
    end
end

function M.is_tilled(tileX, tileY)
    M.ensure()
    return grid.get_data("farm", tileX, tileY, "tilled") == true
end

function M.get_crop(tileX, tileY)
    M.ensure()
    local crop = grid.get_data("farm", tileX, tileY, "crop")
    if crop == "" then
        return nil
    end
    return crop
end

function M.get_growth_stage(tileX, tileY)
    M.ensure()
    return grid.get_data("farm", tileX, tileY, "growth_stage") or 0
end

function M.set_growth_stage(tileX, tileY, stage)
    M.ensure()
    grid.set_data("farm", tileX, tileY, "growth_stage", stage)
end

function M.is_crop_mature(tileX, tileY)
    local crop = M.get_crop(tileX, tileY)
    if crop == "potato_seeds" then
        return M.get_growth_stage(tileX, tileY) >= 3
    end

    return false
end

function M.till(tileX, tileY)
    M.ensure()
    grid.set_data("farm", tileX, tileY, "tilled", true)

    engine.set_tile_tileset_override("Ground", tileX, tileY, GROUND_TILESET, GROUND_TILLED_TILE)
    engine.set_tile_tileset_override("Farmland", tileX, tileY, GROUND_TILESET, FARMLAND_TILLED_TILE)
end

function M.plant(tileX, tileY, cropId, day, hour, minute)
    M.ensure()
    grid.set_data("farm", tileX, tileY, "crop", cropId)
    grid.set_data("farm", tileX, tileY, "growth_stage", 0)
    grid.set_data("farm", tileX, tileY, "planted_day", day)
    grid.set_data("farm", tileX, tileY, "planted_hour", hour)
    grid.set_data("farm", tileX, tileY, "planted_minute", minute)
    mark_planted(tileX, tileY)

    -- TODO: need to make this general for all kinds of seeds
    M.apply_crop_visual(tileX, tileY)
end

function M.clear(tileX, tileY)
    M.ensure()
    grid.set_data("farm", tileX, tileY, "tilled", false)
    grid.set_data("farm", tileX, tileY, "crop", "")
    grid.set_data("farm", tileX, tileY, "growth_stage", 0)
    grid.set_data("farm", tileX, tileY, "planted_day", 0)
    grid.set_data("farm", tileX, tileY, "planted_hour", 0)
    grid.set_data("farm", tileX, tileY, "planted_minute", 0)
    unmark_planted(tileX, tileY)


    engine.clear_tile_override("Ground", tileX, tileY)
    engine.clear_tile_override("Farmland", tileX, tileY)
    engine.clear_tile_override("Crops", tileX, tileY)
end

function M.apply_crop_visual(tileX, tileY)
    local cropId = M.get_crop(tileX, tileY)
    if cropId == nil then
        engine.clear_tile_override("Crops", tileX, tileY)
        return
    end

    local stage = M.get_growth_stage(tileX, tileY)
    local tileId = crop_stage_tile(cropId, stage)
    if tileId ~= nil then
        engine.set_tile_tileset_override("Crops", tileX, tileY, CROPS_TILESET, tileId)
    end
end

function M.clear_crop(tileX, tileY)
    M.ensure()

    grid.set_data("farm", tileX, tileY, "crop", "")
    grid.set_data("farm", tileX, tileY, "growth_stage", 0)
    grid.set_data("farm", tileX, tileY, "planted_day", 0)
    grid.set_data("farm", tileX, tileY, "planted_hour", 0)
    grid.set_data("farm", tileX, tileY, "planted_minute", 0)
    unmark_planted(tileX, tileY)

    engine.clear_tile_override("Crops", tileX, tileY)
end

function M.get_crop_harvest_item(tileX, tileY)
    local crop = M.get_crop(tileX, tileY)
    if crop == "potato_seeds" then
        return "potato"
    end

    return nil
end

function M.update_growth_for_tile(tileX, tileY, currentDay, currentHour, currentMinute)
    local cropId = M.get_crop(tileX, tileY)
    if cropId == nil then
        return
    end

    local plantedDay = grid.get_data("farm", tileX, tileY, "planted_day") or 1
    local plantedHour = grid.get_data("farm", tileX, tileY, "planted_hour") or 0
    local plantedMinute = grid.get_data("farm", tileX, tileY, "planted_minute") or 0

    local plantedTotal = to_total_minutes(plantedDay, plantedHour, plantedMinute)
    local currentTotal = to_total_minutes(currentDay, currentHour, currentMinute)
    local elapsed = currentTotal - plantedTotal

    local newStage = 0
    if elapsed >= 480 then
        newStage = 3
    elseif elapsed >= 300 then
        newStage = 2
    elseif elapsed >= 120 then
        newStage = 1
    end

    local oldStage = M.get_growth_stage(tileX, tileY)
    if newStage ~= oldStage then
        M.set_growth_stage(tileX, tileY, newStage)
        M.apply_crop_visual(tileX, tileY)
    end
end

function M.update_growth(currentDay, currentHour, currentMinute)
    M.ensure()

    for _, tile in pairs(planted_tiles) do
        M.update_growth_for_tile(tile.x, tile.y, currentDay, currentHour, currentMinute)
    end
end

function M.restore()
    M.ensure()

    planted_tiles = {}

    for y = 0, 119 do
        for x = 0, 119 do
            local tilled = grid.get_data("farm", x, y, "tilled")
            local crop = grid.get_data("farm", x, y, "crop")

            if tilled == true then
                engine.set_tile_tileset_override("Ground", x, y, GROUND_TILESET, GROUND_TILLED_TILE)
                engine.set_tile_tileset_override("Farmland", x, y, GROUND_TILESET, FARMLAND_TILLED_TILE)
            end

            if crop ~= nil and crop ~= "" then
                mark_planted(x, y)
            end

            M.apply_crop_visual(x, y)
        end
    end
end

-- TODO: only allow farming in certain area of the map maybe in the beginning
function M.is_inside_farm_area(tileX, tileY)
    return tileX >= 20 and tileX <= 40 and tileY >= 18 and tileY <= 34
end

return M