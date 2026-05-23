local M = {}

local STOP_DISTANCE = 15.0
local MOVE_SPEED = 20.0

local function update_slime_world_ui(self)
    local slimeX, slimeY = engine.get_entity_position(self.id)
    if slimeX ~= nil then
        local health = engine.get_entity_data(self.id, "health")
        ui.set_progress_bar_position(self.health_bar_id, slimeX + 6, slimeY + 32)
        ui.set_progress_bar_value(self.health_bar_id, health)
    end
end

local function get_centre_distance(a, b)
    local ax, ay = engine.get_entity_centre(a)
    local bx, by = engine.get_entity_centre(b)
    if ax == nil or bx == nil then
        return nil
    end

    local dx = bx - ax
    local dy = by - ay
    return math.sqrt(dx * dx + dy * dy)
end

function M.on_create(self)
    print("[LUA] Slime created")
    self.target_id = "player"
    self.attack_cooldown = 0.5
    self.death_started = false
    self.health = 20
    self.max_health = 20

    self.health_bar_id = self.id..".health"
    ui.create_progress_bar(self.health_bar_id, "world")
    ui.set_progress_bar_render_space(self.health_bar_id, "world")
    ui.set_progress_bar_size(self.health_bar_id, 32, 5)
    ui.set_progress_bar_range(self.health_bar_id, 0, self.max_health)
    ui.set_progress_bar_value(self.health_bar_id, self.health)
    update_slime_world_ui(self)
end

function M.on_update(self, dt)
    update_slime_world_ui(self)

    if self.attack_cooldown > 0.0 then
        self.attack_cooldown = math.max(0.0, self.attack_cooldown - dt)
    end

    local dx, dy, distance = engine.get_direction_to_entity(self.id, self.target_id);
    if dx == nil then
        return
    end

    local follow_distance = engine.get_entity_data(self.id, "aggro_range")
    if distance <= follow_distance and distance > STOP_DISTANCE then
        local x, y = engine.get_entity_position(self.id)
        engine.set_entity_linear_velocity(
            self.id,
            dx * MOVE_SPEED,
            dy * MOVE_SPEED
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

    local distanceToPlayer = get_centre_distance(self.id, self.target_id)
    if distanceToPlayer == nil then
        return
    end


    -- Attack!!
    if distanceToPlayer < 16.0  and self.attack_cooldown <= 0.0 then
        local health = engine.get_entity_data(self.target_id, "health")
        local damage = engine.get_entity_data(self.id, "damage")
        engine.set_entity_data(self.target_id, "health", health - damage)
        engine.play_sound("slime_squish", 0.7)
        self.attack_cooldown = 0.2
    end

    -- Death automation
    local health = engine.get_entity_data(self.id, "health")
    if health ~= nil and health <= 0 and not self.death_started then
        self.death_started = true
        ui.set_progress_bar_visible(self.health_bar_id, false)
        engine.set_entity_linear_velocity(self.id, 0.0, 0.0)

        engine.start_entity_coroutine(self, function(self)
            local x, y = engine.get_entity_position(self.id)
            if x ~= nil then
                engine.emit_particles("blood_0", x + 8, y + 8, 32)
                engine.emit_particles("slime_pop_0", x + 8, y + 8, 26)
            end

            engine.wait(0.4)
            engine.play_entity_animation(self.id, "slime_death", true)
            engine.wait(0.4)

            local dropId = engine.spawn_entity("item_drop", x, y)
            if dropId ~= nil then
                engine.set_entity_data(dropId, "type", "pickup")
                engine.set_entity_data(dropId, "count", 1)
                engine.set_entity_data(dropId, "item_type", "item_drop")
            end
            local potato = engine.spawn_entity("potato", x + 4, y + 3)

            engine.destroy_entity(self.id)
        end)
    end
end

return M
