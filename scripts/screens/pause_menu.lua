local M = {}

function M.build(self)
    local viewportWidth, viewportHeight = engine.get_viewport_size()

    ui.create_label("menu.pause.title", "menu.pause")
    ui.set_label_text("menu.pause.title", "Paused")
    ui.set_label_scale("menu.pause.title", 1.8)
    ui.set_label_position("menu.pause.title", viewportWidth * 0.5 - 55, viewportHeight * 0.5 - 60)

    ui.create_label("menu.pause.prompt", "menu.pause")
    ui.set_label_text("menu.pause.prompt", "Press Esc to Resume")
    ui.set_label_position("menu.pause.prompt", viewportWidth * 0.5 - 85, viewportHeight * 0.5)
end

function M.on_enter(self)
end

function M.on_exit(self)
end

function M.on_update(self, dt)
    if engine.is_key_pressed(256) then -- Escape
        screenflow.clear_overlay()
    end
end

return M