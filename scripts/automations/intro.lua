local M = {}

function M.start()
    engine.start_global_coroutine(function()
        engine.clear_camera_target()

        local startX, startY = engine.get_camera_position()
        -- local targetX, targetY = startX + 20, startY + 12
        local targetX, targetY = engine.get_entity_centre("player")

        engine.set_post_fade_amount(1.0)

        local fadeDuration = 3.6
        local fadeElapsed = 0.0

        while fadeElapsed < fadeDuration do
            local dt = engine.wait_frame()
            fadeElapsed = fadeElapsed + dt

            local t = math.min(fadeElapsed / fadeDuration, 1.0)

            local camX = startX + (targetX - startX) * t
            local camY = startY + (targetY - startY) * t

            engine.set_camera_position(camX, camY)
            engine.set_post_fade_amount(1.0 - t)
        end

        engine.set_camera_position(targetX, targetY)
        engine.set_post_fade_amount(0.0)

        engine.set_camera_target_entity("player")
    end)
end

return M
