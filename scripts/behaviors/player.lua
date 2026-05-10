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

    if engine.is_mouse_button_pressed(0) then
        engine.emit_particles("blood_0", mouseX, mouseY, 256)

        local tileX, tileY = engine.get_mouse_tile()
        if tileX ~= nul then
            farm.set_tilled(tileX, tileY, true) -- custom game bindings
            
            local tilled = farm.is_tilled(tileX, tileY)
            print("Tile "..tileX..","..tileY.." tilled = "..tostring(tilled))
        end
        print("Tile X: "..tileX..", Y: "..tileY)
    end

    if engine.is_mouse_button_pressed(1) then
        engine.emit_particles("crates_0", mouseX, mouseY, 128)
    end
end

return M
