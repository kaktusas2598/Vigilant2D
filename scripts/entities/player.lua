return {
    texture = "player",
    animation = "player_idle",
    scale = {48, 48},
    bounds_offset = {20, 5},
    bounds_size = {14, 22},
    script = "scripts/behaviors/player.lua",
    physics_enabled = true,

    controller = {
        move_speed = 120.0,
        idle_animation = "player_idle",
        walk_up_animation = "player_walk_up",
        walk_down_animation = "player_walk_down",
        walk_right_animation = "player_walk_right",
        allow_flip_x = true
    },

    data = {
        health = 100,
        type = "player",
        damage = 3
    }
}
