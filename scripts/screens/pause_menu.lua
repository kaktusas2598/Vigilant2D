local M = {}

function M.build(self)
    local viewportWidth, viewportHeight = engine.get_viewport_size()

    ui.create_label("menu.pause.title", "menu.pause")
    ui.set_label_text("menu.pause.title", "Paused")
    ui.set_label_scale("menu.pause.title", 1.8)
    ui.set_label_screen_anchor("menu.pause.title", 0.5, 0.5)
    ui.set_label_screen_pivot("menu.pause.title", 0.5, 0.5)
    ui.set_label_position("menu.pause.title", 0, -60)

    ui.create_label("menu.pause.prompt", "menu.pause")
    ui.set_label_text("menu.pause.prompt", "Press Esc to Resume")
    ui.set_label_screen_anchor("menu.pause.prompt", 0.5, 0.5)
    ui.set_label_screen_pivot("menu.pause.prompt", 0.5, 0.5)
    ui.set_label_position("menu.pause.prompt", 0, 0)
end

function M.on_enter(self)
    engine.set_post_fade_amount(0.5)
end

function M.on_exit(self)
    engine.set_post_fade_amount(0.0)
end

function M.on_update(self, dt)
    if engine.is_key_pressed(256) then -- Escape
        screenflow.clear_overlay()
    end
end

return M