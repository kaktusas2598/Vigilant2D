local M = {}

local FOLLOW_DISTANCE = 100.0
local STOP_DISTANCE = 5.0
local MOVE_SPEED = 20.0

function M.on_create(self)
    print("[LUA] Player created")
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

    if engine.is_mouse_button_pressed(0) then
        engine.emit_particles("blood_0", mouseX, mouseY, 256)


        -- engine.set_tile_region_from_grid("Ground", tileX, tileY, "cozyFarm", 80, 144, 54, 54)
        engine.set_tile_tileset_override("Ground", tileX, tileY, "cozyFarm", 491)
        

        farm.set_tilled(tileX, tileY, true) -- custom game bindings
        local tilled = farm.is_tilled(tileX, tileY)

        print("Tile "..tileX..","..tileY.." tilled = "..tostring(tilled))
    end

    if engine.is_mouse_button_pressed(1) then
        engine.clear_tile_override("Ground", tileX, tileY)
    end
end

return M
