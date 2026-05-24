local inventory = require("scripts.lib.inventory")
local M = {}

local function get_hotbar_slot(slotIndex)
    return inventory.get_slot("inventory", slotIndex)
end

local function get_item_display_name(itemId)
    local items = require("scripts.items")
    local itemDef = items[itemId]
    if itemDef ~= nil and itemDef.name ~= nil then
        return itemDef.name
    end

    return itemId or "Empty"
end

local function refresh_hotbar_ui()
    for i = 0, 7 do
        local slot = get_hotbar_slot(i)

        if slot ~= nil and slot.item_id ~= nil and slot.item_id ~= "" then
            ui.set_slot_strip_slot_from_entity_definition("hud.hotbar", i, slot.item_id)
        else
            ui.clear_slot_strip_slot("hud.hotbar", i)
        end
    end
end

local function apply_selected_slot(self)
    local slot = get_hotbar_slot(self.selected_slot - 1)

    if slot ~= nil and slot.item_id ~= nil and slot.item_id ~= "" then
        self.selected_tool = slot.item_id
        ui.set_label_text("hud.hotbar_label", get_item_display_name(slot.item_id))
    else
        self.selected_tool = nil
        ui.set_label_text("hud.hotbar_label", "Empty")
    end

    ui.set_slot_strip_selected("hud.hotbar", self.selected_slot - 1)
end

local function update_day_night_visuals()
    local t = engine.get_time_of_day_01()

    -- 0.0 = midnight, 0.25 = 6:00, 0.5 = noon, 0.75 = 18:00
    if t == nil then
        return
    end

    local brightness = 0.0
    local tintR = 1.0
    local tintG = 1.0
    local tintB = 1.0
    local saturation = 1.0

    if t < 0.20 or t > 0.85 then
        brightness = -0.18
        tintR, tintG, tintB = 0.72, 0.78, 1.0
        saturation = 0.82
    elseif t < 0.28 or t > 0.75 then
        brightness = -0.08
        tintR, tintG, tintB = 1.0, 0.92, 0.82
        saturation = 0.95
    end

    engine.set_post_brightness(brightness)
    engine.set_post_tint(tintR, tintG, tintB)
    engine.set_post_saturation(saturation)
end

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
end

local function update_clock_ui()
    local day, hour, minute = engine.get_game_time()
    if day == nil then
        return
    end

    local hourText = string.format("%02d", hour)
    local minuteText = string.format("%02d", minute)
    ui.set_label_text("hud.clock", "Day " .. day .. " " .. hourText .. ":" .. minuteText)
end

function M.on_create(self)
    print("[LUA] Player created")
    self.selected_slot = 1
    self.health = 72;
    self.max_health = 100;
    self.facing = "down"

    -- Create Quickbar UI
    ui.create_slot_strip("hud.hotbar", "hud")
    ui.set_slot_strip_slot_count("hud.hotbar", 8)
    ui.set_slot_strip_position("hud.hotbar", 20, 20)

    ui.create_label("hud.hotbar_label", "hud")
    ui.set_label_text("hud.hotbar_label", "Hotbar")
    ui.set_label_position("hud.hotbar_label", 20, 96)

    -- Create clock label
    ui.create_label("hud.clock", "hud")
    ui.set_label_text("hud.clock", "Day 1 06:00")
    ui.set_label_screen_anchor("hud.clock", 1.0, 1.0)
    ui.set_label_screen_pivot("hud.clock", 1.0, 0.0)
    ui.set_label_position("hud.clock", -20, -30)

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
    refresh_hotbar_ui()
    apply_selected_slot(self)
end

function M.on_update(self, dt)
    refresh_hotbar_ui()
    update_player_world_ui(self)
    update_clock_ui()
    update_day_night_visuals()

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

    -- Mouse wheel changes selected slot in hotbar
    local scrollY = engine.get_mouse_scroll_y()
    if scrollY > 0 then
        self.selected_slot = (self.selected_slot - 2) % 8 + 1
        apply_selected_slot(self)
    elseif scrollY < 0 then
        self.selected_slot = (self.selected_slot % 8) + 1
        apply_selected_slot(self)
    end

    -- Number keys 1..8 map directly to hotbar slots 1..8.
    for i = 1, 8 do
        if engine.is_key_pressed(48 + i) then
            self.selected_slot = i
            apply_selected_slot(self)
            break
        end
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

        elseif self.selected_tool == "potato_seeds" then
            -- add crop on top of ground an farmland layer
            local tilled = grid.get_data("farm", tileX, tileY, "tilled")
            if tilled == true then
                local removed = inventory.remove_item("inventory", "potato_seeds", 1)
                if removed > 0 then
                    engine.set_tile_tileset_override("Crops", tileX, tileY, "cozy_farm_crops", 31)
                end
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
