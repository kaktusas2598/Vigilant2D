local M = {}

function M.build(self)
    local viewportWidth, viewportHeight = engine.get_viewport_size()

    ui.create_label("menu.main.title", "menu.main")
    ui.set_label_text("menu.main.title", "Vigilant2D")
    ui.set_label_scale("menu.main.title", 2.0)
    ui.set_label_position("menu.main.title", viewportWidth * 0.5 - 110, viewportHeight * 0.5 - 80)

    ui.create_label("menu.main.prompt", "menu.main")
    ui.set_label_text("menu.main.prompt", "Press Enter to Start")
    ui.set_label_position("menu.main.prompt", viewportWidth * 0.5 - 90, viewportHeight * 0.5)
end

function M.on_enter(self)
end

function M.on_exit(self)
end

function M.on_update(self, dt)
    if engine.is_key_pressed(257) then -- Enter
        screenflow.show_base("gameplay")
    end
end

return M