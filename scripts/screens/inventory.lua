local M = {}

function M.build(self)
    local viewportWidth, viewportHeight = engine.get_viewport_size()

    ui.create_label("menu.inventory.title", "menu.inventory")
    ui.set_label_text("menu.inventory.title", "Inventory")
    ui.set_label_scale("menu.inventory.title", 1.6)
    ui.set_label_screen_anchor("menu.inventory.title", 0.5, 0.5)
    ui.set_label_screen_pivot("menu.inventory.title", 0.5, 0.5)
    ui.set_label_position("menu.inventory.title", 0, -60)

    ui.create_label("menu.inventory.prompt", "menu.inventory")
    ui.set_label_text("menu.inventory.prompt", "Press I to Close")
    ui.set_label_screen_anchor("menu.inventory.prompt", 0.5, 0.5)
    ui.set_label_screen_pivot("menu.inventory.prompt", 0.5, 0.5)
    ui.set_label_position("menu.inventory.prompt", 0, 0)
end

function M.on_enter(self)
end

function M.on_exit(self)
end

function M.on_update(self, dt)
    if engine.is_key_pressed(73) then -- I
        screenflow.clear_overlay()
    end
end

return M