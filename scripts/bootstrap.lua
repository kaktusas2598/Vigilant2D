local M = {}

function M.start(self)
    engine.load_map("assets/farmMap.tmx")
    engine.set_camera_zoom(4.0)

    engine.create_emitter_from_preset("blood_0", "blood")
    engine.create_emitter_from_preset("crates_0", "crate_burst")
end

function M.post_start(self)
    engine.run_script("scripts/automations/intro.lua", "start")
end

return M