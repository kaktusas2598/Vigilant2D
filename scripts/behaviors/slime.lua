local M = {}

local FOLLOW_DISTANCE = 100.0
local STOP_DISTANCE = 5.0
local MOVE_SPEED = 20.0

function M.on_create(self)
    print("[LUA] Slime created")
    self.target_id = "player"
end

function M.on_update(self, dt)
    local dx, dy, distance = engine.get_direction_to_entity(self.id, self.target_id);
    if dx == nil then
        return
    end

    if distance <= FOLLOW_DISTANCE and distance > STOP_DISTANCE then
        local x, y = engine.get_entity_position(self.id)
        if x == nil then
            return
        end

        engine.set_entity_position(
            self.id,
            x + dx * MOVE_SPEED * dt,
            y + dy * MOVE_SPEED * dt
        )

        if math.abs(dx) > math.abs(dy) then
            engine.set_entity_flip_x(self.id, dx < 0.0)
        end

        engine.play_entity_animation(self.id, "slime_jump", false)
    else
        engine.play_entity_animation(self.id, "slime_idle", false)
    end
end

return M
