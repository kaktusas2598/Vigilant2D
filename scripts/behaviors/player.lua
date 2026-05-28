local farmState = require("scripts.lib.farm_state")
local farmClutter = require("scripts.lib.farm_clutter")
local playerHud = require("scripts.lib.player_hud")
local playerHotbar = require("scripts.lib.player_hotbar")
local playerTools = require("scripts.lib.player_tools")

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

local function update_tool_preview(self, tileX, tileY)
    if not playerTools.can_preview_selected_item(self) then
        ui.set_image_visible("player.tool_preview", false)
        return
    end

    local worldX, worldY = engine.get_tile_world_position(tileX, tileY)
    if worldX == nil then
        ui.set_image_visible("player.tool_preview", false)
        return
    end

    ui.set_image_position("player.tool_preview", worldX, worldY)

    if playerTools.can_use_selected_item_on_tile(self, tileX, tileY) then
        ui.set_image_tint("player.tool_preview", 0.45, 1.0, 0.45, 0.75)
    elseif playerTools.is_tile_in_use_range(self, tileX, tileY) then
        ui.set_image_tint("player.tool_preview", 1.0, 0.4, 0.4, 0.75)
    else
        ui.set_image_tint("player.tool_preview", 0.6, 0.6, 0.6, 0.55)
    end

    ui.set_image_visible("player.tool_preview", true)
end

function M.on_create(self)
    print("[LUA] Player created")
    self.selected_slot = 1
    self.health = 72;
    self.max_health = 100;
    self.facing = "down"
    -- Track last game clock time to not crash FPS with farm growth logic
    self.last_clock_day = -1
    self.last_clock_hour = -1
    self.last_clock_minute = -1
    self.use_hold_elapsed = 0.0
    self.use_hold_tile_x = nil
    self.use_hold_tile_y = nil

    playerHud.create(self)
    playerHud.update_player_world_ui(self)
    playerHotbar.refresh_ui()
    playerHotbar.apply_selected_slot(self)
end

function M.on_update(self, dt)
    local currentMapPath = engine.get_current_map_path()
    if farmState.is_farm_map(currentMapPath) then
        if not self.farm_restored then
            print("[Lua] Restoring farm data")
            farmState.restore()
            farmClutter.generate_once()
            self.farm_restored = true
            -- Defer actual clutter entity spawning to a bit later on to avoid segfault
            engine.start_entity_coroutine(self, function(self)
                engine.wait(0.1)
                farmClutter.restore()
            end)
        end
    else
        self.farm_restored = false
    end

    if engine.get_entity_data(self.id, "hotbar_dirty") == true then
        playerHotbar.refresh_ui()
        playerHotbar.apply_selected_slot(self)
        engine.set_entity_data(self.id, "hotbar_dirty", false)
    end

    playerHud.update_player_world_ui(self)
    playerHud.update_clock_ui()
    playerHud.update_day_night_visuals()

    if ui.was_button_clicked("hud.time_plus_1h") then
        engine.advance_game_time(60)
    end

    if ui.was_button_clicked("hud.time_plus_6h") then
        engine.advance_game_time(360)
    end

    local day, hour, minute = engine.get_game_time()
    if day ~= nil then
        if day ~= self.last_clock_day or hour ~= self.last_clock_hour or minute ~= self.last_clock_minute then
            farmState.handle_day_change(self.last_clock_day, day)
            farmState.update_growth(day, hour, minute)
            self.last_clock_day = day
            self.last_clock_hour = hour
            self.last_clock_minute = minute
        end
    end


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

    playerHotbar.update_selection_input(self)
    update_tool_preview(self, tileX, tileY)

    local mouseHeld = engine.is_mouse_button_down(0)
    local selectedHoldTime = playerTools.get_selected_item_number(self, "hold_time", 0.0)
    if mouseHeld and selectedHoldTime > 0.0 then
        if playerTools.can_use_selected_item_on_tile(self, tileX, tileY) then
            if self.use_hold_tile_x ~= tileX or self.use_hold_tile_y ~= tileY then
                self.use_hold_tile_x = tileX
                self.use_hold_tile_y = tileY
                self.use_hold_elapsed = 0.0
            end

            self.use_hold_elapsed = self.use_hold_elapsed + dt

            if self.use_hold_elapsed >= selectedHoldTime then
                playerTools.commit_selected_tool_use(self, tileX, tileY, mouseX, mouseY)
                self.use_hold_elapsed = 0.0
                self.use_hold_tile_x = nil
                self.use_hold_tile_y = nil
            end
        else
            self.use_hold_elapsed = 0.0
            self.use_hold_tile_x = nil
            self.use_hold_tile_y = nil
        end
    else
        self.use_hold_elapsed = 0.0
        self.use_hold_tile_x = nil
        self.use_hold_tile_y = nil
    end

    if engine.is_mouse_button_pressed(0) then -- LMB
        -- Any tool or empty with LMB click - try harvesting crop
        if playerTools.is_tile_in_use_range(self, tileX, tileY) and playerTools.try_harvest_crop(tileX, tileY) then
            return
        end

        if self.selected_tool == "potato_seeds" then
            if playerTools.can_use_selected_item_on_tile(self, tileX, tileY) then
                playerTools.commit_selected_tool_use(self, tileX, tileY, mouseX, mouseY)
            end
        elseif self.selected_tool == "sword" and not self.action_locked then
            self.action_locked = true
            engine.set_entity_animation_locked(self.id, true)
            play_attack_animation(self)
            engine.play_sound("sword_hit", 0.6)

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

    if engine.is_mouse_button_pressed(1) then -- RMB
        if self.selected_tool == "shovel" then
            engine.play_sound("clear_dirt", 0.7)
            farmState.clear(tileX, tileY)
        elseif playerTools.get_selected_item_type(self) == "consumable" then
            playerTools.consume_selected_item(self)
        end
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