return {
    texture = "slime",
    animation = "slime_jump",
    scale = {48, 48},
    bounds_offset = {20, 5},
    bounds_size = {14, 22},
    script = "scripts/behaviors/slime.lua",
    physics_enabled = false,
    data = {
        health = 20,
        type = "enemy",
        damage = 5,
        aggro_range = 100.0
    }
}
