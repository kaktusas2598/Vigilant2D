return {
    texture = "cozy_farm_seeds",
    texture_grid = {
        column = 5,
        row = 0,
        columns = 8,
        rows = 6 
    },
    scale = {12, 12},
    script = "scripts/behaviors/item_drop.lua",
    physics_enabled = false,
    data = {
        type = "pickup",
        count = 1,
        item_type = "potato_seeds"
    }
}
