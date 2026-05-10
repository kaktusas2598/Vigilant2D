local M = {}

local FOLLOW_DISTANCE = 100.0
local STOP_DISTANCE = 5.0
local MOVE_SPEED = 20.0

function M.on_create(self)
    print("[LUA] Player created")
    self.selected_tool = "shovel"
end

function M.on_update(self, dt)
    local mouseX, mouseY = engine.get_mouse_world_position()
    if mouseX == nil then
        return
    end

    local tileX, tileY = engine.get_mouse_tile()
    if tileX == nil then
        return
    end

    if engine.is_key_pressed(80) then -- 'p'
        engine.emit_particles("crates_0", mouseX, mouseY, 128)
    end


    if engine.is_key_pressed(49) then -- '1'
        self.selected_tool = "shovel"
    end

    if engine.is_key_pressed(50) then -- '2'
        self.selected_tool = "seeds"
    end

    if engine.is_mouse_button_pressed(0) then -- LMB
        if self.selected_tool == "shovel" then
            -- engine.set_tile_region_from_grid("Ground", tileX, tileY, "cozyFarm", 80, 144, 54, 54)
            -- Replace grass tile on grounds layer with ground tile
            engine.set_tile_tileset_override("Ground", tileX, tileY, "cozyFarm", 491)
                -- place tilled ground tile in farmland layer above
            engine.set_tile_tileset_override("Farmland", tileX, tileY, "cozyFarm", 494)

            farm.set_tilled(tileX, tileY, true) -- custom game bindings
        elseif self.selected_tool == "seeds" then
            -- add crop on top of ground an farmland layer
            if farm.is_tilled(tileX, tileY) then
                engine.set_tile_tileset_override("Crops", tileX, tileY, "cozy_farm_free_version", 110)
            end
        end

        engine.emit_particles("blood_0", mouseX, mouseY, 256)

        local tilled = farm.is_tilled(tileX, tileY)

        print("Tile "..tileX..","..tileY.." tilled = "..tostring(tilled))
    end

    if engine.is_mouse_button_pressed(1) then -- RMB
        engine.clear_tile_override("Ground", tileX, tileY)
        engine.clear_tile_override("Farmland", tileX, tileY)
        engine.clear_tile_override("Crops", tileX, tileY)
    end
end

return M
