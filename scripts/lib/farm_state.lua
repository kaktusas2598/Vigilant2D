local M = {}

local FARM_MAP = "assets/farmMap.tmx"

local GROUND_TILESET = "cozyFarm"
local CROPS_TILESET = "cozy_farm_crops"

local GROUND_TILLED_TILE = 491
local FARMLAND_TILLED_TILE = 494

local POTATO_SEEDS_TILE = 31

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

function M.till(tileX, tileY)
    M.ensure()
    grid.set_data("farm", tileX, tileY, "tilled", true)

    engine.set_tile_tileset_override("Ground", tileX, tileY, GROUND_TILESET, GROUND_TILLED_TILE)
    engine.set_tile_tileset_override("Farmland", tileX, tileY, GROUND_TILESET, FARMLAND_TILLED_TILE)
end

function M.plant(tileX, tileY, cropId)
    M.ensure()
    grid.set_data("farm", tileX, tileY, "crop", cropId)

    -- TODO: need to make this general for all kinds of seeds
    if cropId == "potato_seeds" then
        engine.set_tile_tileset_override("Crops", tileX, tileY, CROPS_TILESET, POTATO_SEEDS_TILE)
    end
end

function M.clear(tileX, tileY)
    M.ensure()
    grid.set_data("farm", tileX, tileY, "tilled", false)
    grid.set_data("farm", tileX, tileY, "crop", "")

    engine.clear_tile_override("Ground", tileX, tileY)
    engine.clear_tile_override("Farmland", tileX, tileY)
    engine.clear_tile_override("Crops", tileX, tileY)
end

function M.restore()
    M.ensure()

    for y = 0, 119 do
        for x = 0, 119 do
            local tilled = grid.get_data("farm", x, y, "tilled")
            local crop = grid.get_data("farm", x, y, "crop")

            if tilled == true then
                engine.set_tile_tileset_override("Ground", x, y, GROUND_TILESET, GROUND_TILLED_TILE)
                engine.set_tile_tileset_override("Farmland", x, y, GROUND_TILESET, FARMLAND_TILLED_TILE)
            end

            if crop == "potato_seeds" then
                engine.set_tile_tileset_override("Crops", x, y, CROPS_TILESET, POTATO_SEEDS_TILE)
            end
        end
    end
end

-- TODO: only allow farming in certain area of the map maybe in the beginning
function M.is_inside_farm_area(tileX, tileY)
    return tileX >= 20 and tileX <= 40 and tileY >= 18 and tileY <= 34
end

return M