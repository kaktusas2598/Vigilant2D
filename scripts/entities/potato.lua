return {
    texture = "cozy_ui_items",
    texture_grid = {
        column = 5,
        row = 0,
        columns = 10,
        rows = 12 
    },
    scale = {12, 12},
    script = "scripts/behaviors/item_drop.lua",
    physics_enabled = false,
    data = {
        type = "pickup",
        count = 1,
        item_type = "vegetable"
    }
}
