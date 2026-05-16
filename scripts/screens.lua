return {
    main_menu = {
        groups = { "menu.main" },
        overlay = false,
        pauses_gameplay = true,
        initial = true,
        script = "scripts/screens/main_menu.lua"
    },

    gameplay = {
        groups = { "hud" },
        overlay = false,
        pauses_gameplay = false,
        script = "scripts/screens/gameplay.lua"
    },

    pause_menu = {
        groups = { "menu.pause" },
        overlay = true,
        pauses_gameplay = true,
        script = "scripts/screens/pause_menu.lua"
    },

    inventory = {
        groups = { "menu.inventory" },
        overlay = true,
        pauses_gameplay = false,
        script = "scripts/screens/inventory.lua"
    }
}