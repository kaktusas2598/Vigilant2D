local M = {}

function M.build(self)
end

function M.on_enter(self)
end

function M.on_exit(self)
end

function M.on_update(self, dt)
    if engine.is_key_pressed(256) then -- Escape
        screenflow.toggle_overlay("pause_menu")
    end

    if engine.is_key_pressed(73) then -- I
        screenflow.toggle_overlay("inventory")
    end
end

return M