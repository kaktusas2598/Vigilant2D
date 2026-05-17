local M = {}

function M.build(self)
    local viewportWidth, viewportHeight = engine.get_viewport_size()

    ui.create_label("menu.main.title", "menu.main")
    ui.set_label_text("menu.main.title", "Vigilant2D")
    ui.set_label_scale("menu.main.title", 2.0)
    ui.set_label_screen_anchor("menu.main.title", 0.5, 0.5)
    ui.set_label_screen_pivot("menu.main.title", 0.5, 0.5)
    ui.set_label_position("menu.main.title", 0, 80)

    ui.create_button("menu.main.start_button", "menu.main")
    ui.set_button_text("menu.main.start_button", "Start Game")
    ui.set_button_screen_anchor("menu.main.start_button", 0.5, 0.5)
    ui.set_button_screen_pivot("menu.main.start_button", 0.5, 0.5)
    ui.set_button_position("menu.main.start_button", 0, 12)
    ui.set_button_size("menu.main.start_button", 180, 44)
end

function M.on_enter(self)
end

function M.on_exit(self)
end

function M.on_update(self, dt)
    if ui.was_button_clicked("menu.main.start_button") or engine.is_key_pressed(257) then -- Enter
        screenflow.show_base("gameplay")
    end
end

return M