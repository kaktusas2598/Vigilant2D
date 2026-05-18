local M = {}

function M.build(self)
    ui.create_label("menu.game_over.title", "menu.game_over")
    ui.set_label_text("menu.game_over.title", "Game Over")
    ui.set_label_scale("menu.game_over.title", 2.2)
    ui.set_label_screen_anchor("menu.game_over.title", 0.5, 0.5)
    ui.set_label_screen_pivot("menu.game_over.title", 0.5, 0.5)
    ui.set_label_position("menu.game_over.title", 0, -80)
    ui.set_label_text_color("menu.game_over.title", 1.0, 0.2, 0.2, 1.0)

    ui.create_button("menu.game_over.main_menu", "menu.game_over")
    ui.set_button_text("menu.game_over.main_menu", "Main Menu")
    ui.set_button_screen_anchor("menu.game_over.main_menu", 0.5, 0.5)
    ui.set_button_screen_pivot("menu.game_over.main_menu", 0.5, 0.5)
    ui.set_button_position("menu.game_over.main_menu", 0, 0)
    ui.set_button_size("menu.game_over.main_menu", 180, 44)

    ui.create_button("menu.game_over.quit", "menu.game_over")
    ui.set_button_text("menu.game_over.quit", "Quit")
    ui.set_button_screen_anchor("menu.game_over.quit", 0.5, 0.5)
    ui.set_button_screen_pivot("menu.game_over.quit", 0.5, 0.5)
    ui.set_button_position("menu.game_over.quit", 0, 58)
    ui.set_button_size("menu.game_over.quit", 180, 44)
end

function M.on_enter(self)
    engine.set_post_tint(1.0, 0.45, 0.45)
    engine.set_post_saturation(0.45)
    engine.set_post_vignette(0.4)
    engine.set_post_fade_amount(0.18)
end

function M.on_exit(self)
    engine.set_post_tint(1.0, 1.0, 1.0)
    engine.set_post_saturation(1.0)
    engine.set_post_vignette(0.18)
    engine.set_post_fade_amount(0.0)
end

function M.on_update(self, dt)
    if ui.was_button_clicked("menu.game_over.main_menu") then
        screenflow.show_base("main_menu")
    elseif ui.was_button_clicked("menu.game_over.quit") then
        engine.close_game()
    end
end

return M