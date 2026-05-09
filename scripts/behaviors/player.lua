local M = {}

local FOLLOW_DISTANCE = 100.0
local STOP_DISTANCE = 5.0
local MOVE_SPEED = 20.0

function M.on_create(self)
    print("[LUA] Player created")
end

function M.on_update(self, dt)
    -- Input check for mouse clicks, keys
    -- Get mouse pos
    -- Fire of emitters, change animations
end

return M
