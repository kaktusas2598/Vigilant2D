local M = {}

function M.create(self)
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

   -- Debug time controls 
    ui.create_button("hud.time_plus_1h", "hud")
    ui.set_button_text("hud.time_plus_1h", "+1h")
    ui.set_button_screen_anchor("hud.time_plus_1h", 1.0, 0.0)
    ui.set_button_screen_pivot("hud.time_plus_1h", 1.0, 0.0)
    ui.set_button_position("hud.time_plus_1h", -20, 56)
    ui.set_button_size("hud.time_plus_1h", 72, 32)

    ui.create_button("hud.time_plus_6h", "hud")
    ui.set_button_text("hud.time_plus_6h", "+6h")
    ui.set_button_screen_anchor("hud.time_plus_6h", 1.0, 0.0)
    ui.set_button_screen_pivot("hud.time_plus_6h", 1.0, 0.0)
    ui.set_button_position("hud.time_plus_6h", -100, 56)
    ui.set_button_size("hud.time_plus_6h", 72, 32)

    -- Tool use preview overlay
    ui.create_image("player.tool_preview", "hud")
    ui.set_image_render_space("player.tool_preview", "world")
    ui.set_image_size("player.tool_preview", 16, 16)
    ui.set_image_order("player.tool_preview", 20)
    ui.set_image_visible("player.tool_preview", false)
    ui.set_image_texture_rect("player.tool_preview", "ui_flat", 384, 32, 32, 32)
end

function M.update_player_world_ui(self)
    local playerX, playerY = engine.get_entity_position(self.id)
    if playerX ~= nil then
        local health = engine.get_entity_data(self.id, "health")
        ui.set_progress_bar_position("player.health", playerX + 6, playerY + 32)
        ui.set_progress_bar_value("player.health", health)
        ui.set_label_position("player.name", playerX, playerY + 42)
    end
end

function M.update_clock_ui()
    local day, hour, minute = engine.get_game_time()
    if day == nil then
        return
    end

    local hourText = string.format("%02d", hour)
    local minuteText = string.format("%02d", minute)
    ui.set_label_text("hud.clock", "Day " .. day .. " " .. hourText .. ":" .. minuteText)
end

function M.update_day_night_visuals()
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

return M