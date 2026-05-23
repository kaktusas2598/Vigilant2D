local inventory = require("scripts.lib.inventory")
local M = {}

-- Calculate sword hitbox based on player facing direction
local function get_sword_hit_box(self)
    local centreX, centreY = engine.get_entity_centre(self.id)
    if centreX == nil then
        return nil
    end

    local width = 24
    local height = 24
    local reach = 18

    if self.facing == "up" then
        return centreX - width * 0.5, centreY + reach - height * 0.5, width, height
    elseif self.facing == "down" then
        return centreX - width * 0.5, centreY - reach - height * 0.5, width, height
    elseif self.facing == "left" then
        return centreX - reach - width * 0.5, centreY - height * 0.5, width, height
    else
        return centreX + reach - width * 0.5, centreY - height * 0.5, width, height
    end
end

local function play_attack_animation(self)
    if self.facing == "up" then
        engine.set_entity_flip_x(self.id, false)
        engine.play_entity_animation(self.id, "player_fight_up", true)
    elseif self.facing == "down" then
        engine.set_entity_flip_x(self.id, false)
        engine.play_entity_animation(self.id, "player_fight_down", true)
    elseif self.facing == "left" then
        engine.set_entity_flip_x(self.id, true)
        engine.play_entity_animation(self.id, "player_fight_right", true)
    else
        engine.set_entity_flip_x(self.id, false)
        engine.play_entity_animation(self.id, "player_fight_right", true)
    end
end

local function update_player_world_ui(self)
    local playerX, playerY = engine.get_entity_position(self.id)
    if playerX ~= nil then
        local health = engine.get_entity_data(self.id, "health")
        ui.set_progress_bar_position("player.health", playerX + 6, playerY + 32)
        ui.set_progress_bar_value("player.health", health)
        ui.set_label_position("player.name", playerX, playerY + 42)
    end

    -- local coinCount = inventory.count_item("inventory", "coin")
    -- ui.set_label_text("hud.coins", "Coins: " .. tostring(coinCount))
end

local FOLLOW_DISTANCE = 100.0
local STOP_DISTANCE = 5.0
local MOVE_SPEED = 20.0

function M.on_create(self)
    print("[LUA] Player created")
    self.selected_tool = "shovel"
    self.health = 72;
    self.max_health = 100;
    self.facing = "down"

    -- Create Quickbar UI
    ui.create_slot_strip("hud.hotbar", "hud")
    ui.set_slot_strip_slot_count("hud.hotbar", 8)
    ui.set_slot_strip_position("hud.hotbar", 20, 20)
    ui.set_slot_strip_slot_texture("hud.hotbar", 0, "shovel")
    ui.set_slot_strip_slot_texture_grid("hud.hotbar", 1, "cozy_farm_seeds", 5, 0, 7, 6)
    ui.set_slot_strip_slot_texture("hud.hotbar", 2, "sword")
    ui.set_slot_strip_slot_texture("hud.hotbar", 3, "bucket")
    ui.set_slot_strip_selected("hud.hotbar", 0)
    ui.set_slot_strip_slot_texture_grid("hud.hotbar", 5, "cozy_farm_seeds", 0, 0, 8, 6)

    ui.create_label("hud.hotbar_label", "hud")
    ui.set_label_text("hud.hotbar_label", "Hotbar")
    ui.set_label_position("hud.hotbar_label", 20, 96)

    -- Create player HUD
    ui.create_progress_bar("player.health", "world")
    ui.set_progress_bar_render_space("player.health", "world")
    ui.set_progress_bar_size("player.health", 32, 5)
    ui.set_progress_bar_range("player.health", 0, self.max_health)
    ui.set_progress_bar_value("player.health", self.health)

    ui.create_label("player.name", "world")
    ui.set_label_render_space("player.name", "world")
    ui.set_label_text("player.name", "Player")
    ui.set_label_scale("player.name", 0.35)
    update_player_world_ui(self)
end

function M.on_update(self, dt)
    update_player_world_ui(self)

    if self.action_locked and engine.is_entity_animation_finished(self.id) then
        engine.set_entity_animation_locked(self.id, false)
        self.action_locked = false
    end

    local moveX = 0
    local moveY = 0

    -- TODO: Entity facing direction could probably be provided by engine here!
    if engine.is_key_down(87) then moveY = moveY + 1 end -- W
    if engine.is_key_down(83) then moveY = moveY - 1 end -- S
    if engine.is_key_down(65) then moveX = moveX - 1 end -- A
    if engine.is_key_down(68) then moveX = moveX + 1 end -- D

    if moveX ~= 0 or moveY ~= 0 then
        if math.abs(moveX) > math.abs(moveY) then
            if moveX > 0 then
                self.facing = "right"
            else
                self.facing = "left"
            end
        else
            if moveY > 0 then
                self.facing = "up"
            else
                self.facing = "down"
            end
        end
    end

    local mouseX, mouseY = engine.get_mouse_world_position()
    if mouseX == nil then
        return
    end

    local tileX, tileY = engine.get_mouse_tile()
    if tileX == nil then
        return
    end

    -- Particle emitter test
    if engine.is_key_pressed(80) then -- 'p'
        engine.emit_particles("crates_0", mouseX, mouseY, 128)
    elseif engine.is_key_pressed(66) then -- 'b'
        engine.emit_particles("blood_0", mouseX, mouseY, 256)
    end

    -- Hotbar/selected tool update based on input
    if engine.is_key_pressed(49) then -- '1'
        self.selected_tool = "shovel"
        ui.set_slot_strip_selected("hud.hotbar", 0)
        ui.set_label_text("hud.hotbar_label", "Shovel")
    elseif engine.is_key_pressed(50) then -- '2'
        self.selected_tool = "seeds"
        ui.set_slot_strip_selected("hud.hotbar", 1)
        ui.set_label_text("hud.hotbar_label", "Potato seeds")
    elseif engine.is_key_pressed(51) then -- '3'
        self.selected_tool = "sword"
        ui.set_slot_strip_selected("hud.hotbar", 2)
        ui.set_label_text("hud.hotbar_label", "Sword")
    elseif engine.is_key_pressed(52) then -- '4'
        self.selected_tool = "bucket"
        ui.set_label_text("hud.hotbar_label", "Bucket")
        ui.set_slot_strip_selected("hud.hotbar", 3)
    end

    if engine.is_mouse_button_pressed(0) then -- LMB
        if self.selected_tool == "shovel" then
            -- Replace grass tile on grounds layer with ground tile
            engine.set_tile_tileset_override("Ground", tileX, tileY, "cozyFarm", 491)
            -- Place tilled ground tile in farmland layer above
            engine.set_tile_tileset_override("Farmland", tileX, tileY, "cozyFarm", 494)

            engine.play_sound("shovel", 0.7)
            engine.emit_particles("dust_puff_0", mouseX, mouseY + 8, 14)
            grid.set_data("farm", tileX, tileY, "tilled", true)

        -- different animation texture test
        self.action_locked = true
        engine.set_entity_animation_locked(self.id, true)
        engine.play_entity_animation(self.id, "player_hoe_right", true)

        elseif self.selected_tool == "seeds" then
            -- add crop on top of ground an farmland layer
            local tilled = grid.get_data("farm", tileX, tileY, "tilled")
            if tilled == true then
                engine.set_tile_tileset_override("Crops", tileX, tileY, "cozy_farm_crops", 31)
            end
        elseif self.selected_tool == "sword" and not self.action_locked then
            self.action_locked = true
            engine.set_entity_animation_locked(self.id, true)
            play_attack_animation(self)
            engine.play_sound("sword_hit", 0.7)

            local hitX, hitY, hitW, hitH = get_sword_hit_box(self)
            if hitX ~= nil then
                local hits = engine.get_entities_in_box(hitX, hitY, hitW, hitH)
                for _, entityId in ipairs(hits) do
                    if entityId ~= self.id then
                        -- No friendly fire!
                        local type = engine.get_entity_data(entityId, "type")
                        if type == "enemy" then
                            local health = engine.get_entity_data(entityId, "health")
                            local damage = engine.get_entity_data(self.id, "damage")
                            engine.set_entity_data(entityId, "health", health - damage)
                            print("Player hit "..entityId.." HP: "..health - damage)
                            local ex, ey = engine.get_entity_position(entityId)
                            if ex~= nil then
                                engine.emit_particles("blood_0", ex + 8, ey + 8, 32)
                            end
                        end
                    end
                end
            end
        end
    end

    -- Clear tile overrides with RMB
    if engine.is_mouse_button_pressed(1) then -- RMB
        engine.play_sound("clear_dirt", 0.7)
        engine.clear_tile_override("Ground", tileX, tileY)
        engine.clear_tile_override("Farmland", tileX, tileY)
        engine.clear_tile_override("Crops", tileX, tileY)
        grid.set_data("farm", tileX, tileY, "tilled", false)
    end

    local playerHealth = engine.get_entity_data(self.id, "health")
    if playerHealth ~= nil and playerHealth < 0 and not self.game_over then
        self.game_over = true
        engine.start_entity_coroutine(self, function(self)
            engine.wait(1.0)
            engine.set_entity_animation_locked(self.id, true)
            engine.play_entity_animation(self.id, "player_fall", true)
            engine.wait(0.4)
        end)
        screenflow.show_base("game_over")
    end

end

return M
