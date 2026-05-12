return {
    texture = "player",
    animation = "player_idle",
    scale = {48, 48},
    bounds_offset = {20, 5},
    bounds_size = {14, 22},
    script = "scripts/behaviors/player.lua",
    physics_enabled = true,
    data = {
        health = 100,
        type = "player",
        damage = 3
    }
}
