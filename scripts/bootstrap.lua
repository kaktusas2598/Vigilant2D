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

    -- Initial inventory items
    list.set_data("inventory", 0, "item_id", "shovel")
    list.set_data("inventory", 0, "count", 1)
    list.set_data("inventory", 1, "item_id", "potato_seeds")
    list.set_data("inventory", 1, "count", 10)
    list.set_data("inventory", 2, "item_id", "sword")
    list.set_data("inventory", 2, "count", 1)
    list.set_data("inventory", 3, "item_id", "bucket")
    list.set_data("inventory", 3, "count", 1)
end

return M