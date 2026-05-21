local M = {}

function M.start(self)
    engine.load_map("assets/farmMap.tmx")
    engine.set_camera_zoom(4.0)

    engine.create_emitter_from_preset("blood_0", "blood")
    engine.create_emitter_from_preset("crates_0", "crate_burst")
    engine.create_emitter_from_preset("coin_pickup_0", "coin_pickup")
    engine.create_emitter_from_preset("dust_puff_0", "dust_puff")
    engine.create_emitter_from_preset("heal_sparkle_0", "heal_sparkle")
    engine.create_emitter_from_preset("slime_pop_0", "slime_pop")
end

function M.post_start(self)
    engine.run_script("scripts/automations/intro.lua", "start")

    grid.create("farm", 120, 120)
    list.create("inventory", 16)
    list.create("hotbar", 8)
end

return M