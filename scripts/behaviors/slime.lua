local M = {}

local FOLLOW_DISTANCE = 100.0
local STOP_DISTANCE = 15.0
local MOVE_SPEED = 20.0

function M.on_create(self)
    print("[LUA] Slime created")
    self.target_id = "player"
    self.attack_cooldown = 0.0
    self.death_started = false
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

        if not self.death_started then
            engine.play_entity_animation(self.id, "slime_jump", false)
        end
    else
        if not self.death_started then
            engine.play_entity_animation(self.id, "slime_idle", false)
        end
    end

    -- Attack!!
    if distance < 10.0 then
        local health = engine.get_entity_data(self.target_id, "health")
        local damage = engine.get_entity_data(self.id, "damage")
        engine.set_entity_data(self.target_id, "health", health - damage)
        print("Slime hit Player! Player HP: "..health - damage)
    end

    -- Death automation
    local health = engine.get_entity_data(self.id, "health")
    if health ~= nil and health <= 0 and not self.death_started then
        self.death_started = true
        engine.start_entity_coroutine(self, function(self)
            local x, y = engine.get_entity_position(self.id)
            if x ~= nil then
                engine.emit_particles("blood_0", x + 8, y + 8, 32)
            end

            engine.wait(1.0)
            engine.play_entity_animation(self.id, "slime_death", true)
            engine.wait(0.4)
            engine.destroy_entity(self.id)
        end)
    end
end

return M
