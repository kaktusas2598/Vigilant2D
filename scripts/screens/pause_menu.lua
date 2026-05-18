local M = {}

function M.build(self)
    ui.create_label("menu.pause.title", "menu.pause")
    ui.set_label_text("menu.pause.title", "Paused")
    ui.set_label_scale("menu.pause.title", 1.9)
    ui.set_label_screen_anchor("menu.pause.title", 0.5, 0.5)
    ui.set_label_screen_pivot("menu.pause.title", 0.5, 0.5)
    ui.set_label_position("menu.pause.title", 0, -90)

    ui.create_button("menu.pause.resume_button", "menu.pause")
    ui.set_button_text("menu.pause.resume_button", "Resume")
    ui.set_button_screen_anchor("menu.pause.resume_button", 0.5, 0.5)
    ui.set_button_screen_pivot("menu.pause.resume_button", 0.5, 0.5)
    ui.set_button_position("menu.pause.resume_button", 0, 4)
    ui.set_button_size("menu.pause.resume_button", 180, 44)

    ui.create_button("menu.pause.main_menu_button", "menu.pause")
    ui.set_button_text("menu.pause.main_menu_button", "Main Menu")
    ui.set_button_screen_anchor("menu.pause.main_menu_button", 0.5, 0.5)
    ui.set_button_screen_pivot("menu.pause.main_menu_button", 0.5, 0.5)
    ui.set_button_position("menu.pause.main_menu_button", 0, 58)
    ui.set_button_size("menu.pause.main_menu_button", 180, 44)

    ui.create_button("menu.pause.quit_button", "menu.pause")
    ui.set_button_text("menu.pause.quit_button", "Quit Game")
    ui.set_button_screen_anchor("menu.pause.quit_button", 0.5, 0.5)
    ui.set_button_screen_pivot("menu.pause.quit_button", 0.5, 0.5)
    ui.set_button_position("menu.pause.quit_button", 0, 112)
    ui.set_button_size("menu.pause.quit_button", 180, 44)
end


function M.on_enter(self)
    engine.set_post_tint(0.78, 0.86, 1.0)
    engine.set_post_saturation(0.55)
    engine.set_post_vignette(0.32)
    engine.set_post_fade_amount(0.28)
end

function M.on_exit(self)
    engine.set_post_tint(1.0, 1.0, 1.0)
    engine.set_post_saturation(1.0)
    engine.set_post_vignette(0.18)
    engine.set_post_fade_amount(0.0)
end

function M.on_update(self, dt)
    if engine.is_key_pressed(256) or ui.was_button_clicked("menu.pause.resume_button") then
        screenflow.clear_overlay()
    end

    if ui.was_button_clicked("menu.pause.main_menu_button") then
        screenflow.show_base("main_menu")
    end

    if ui.was_button_clicked("menu.pause.quit_button") then
        engine.close_game()
    end
end

return M