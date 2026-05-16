local M = {}

local function get_sword_hit_box(self)
    local playerX, playerY = engine.get_entity_position(self.id)
    if playerX == nil then
        return nil
    end

    -- Simple first version: centered around player a bit wider than the body
    return playerX - 8, playerY - 4, 32, 24
end

local FOLLOW_DISTANCE = 100.0
local STOP_DISTANCE = 5.0
local MOVE_SPEED = 20.0

function M.on_create(self)
    print("[LUA] Player created")
    self.selected_tool = "shovel"
    self.health = 72;
    self.max_health = 100;

    -- Create Quickbar UI
    ui.create_slot_strip("hud.hotbar", "hud")
    ui.set_slot_strip_slot_count("hud.hotbar", 8)
    ui.set_slot_strip_position("hud.hotbar", 20, 20)
    ui.set_slot_strip_slot_texture("hud.hotbar", 0, "shovel")
    ui.set_slot_strip_slot_tileset_tile("hud.hotbar", 1, "cozy_farm_free_version", 108)
    ui.set_slot_strip_slot_texture("hud.hotbar", 2, "sword")
    ui.set_slot_strip_slot_texture("hud.hotbar", 3, "bucket")
    ui.set_slot_strip_selected("hud.hotbar", 0)

    ui.create_label("hud.hotbar_label", "hud")
    ui.set_label_text("hud.hotbar_label", "Hotbar")
    ui.set_label_position("hud.hotbar_label", 20, 96)

    -- Create player HUD
    ui.create_progress_bar("player.health", "world")
    ui.set_progress_bar_render_space("player.health", "world")
    ui.set_progress_bar_size("player.health", 32, 5)
    ui.set_progress_bar_range("player.health", 0, self.max_health)
    ui.set_progress_bar_value("player.health", self.health)

    ui.create_label("player.name", "world")
    ui.set_label_render_space("player.name", "world")
    ui.set_label_text("player.name", "Player")
    ui.set_label_scale("player.name", 0.35)
end

function M.on_update(self, dt)
    -- Update player UI based on player's current position
    local playerX, playerY = engine.get_entity_position(self.id)
    if playerX ~= nil then
        local health = engine.get_entity_data(self.id, "health")
        ui.set_progress_bar_position("player.health", playerX + 6, playerY + 32)
        ui.set_progress_bar_value("player.health", health)
        ui.set_label_position("player.name", playerX, playerY + 42)
    end

    local mouseX, mouseY = engine.get_mouse_world_position()
    if mouseX == nil then
        return
    end

    local tileX, tileY = engine.get_mouse_tile()
    if tileX == nil then
        return
    end


    -- Particle emitter test
    if engine.is_key_pressed(80) then -- 'p'
        engine.emit_particles("crates_0", mouseX, mouseY, 128)
    elseif engine.is_key_pressed(66) then -- 'b'
        engine.emit_particles("blood_0", mouseX, mouseY, 256)
    end

    -- Hotbar/selected tool update based on input
    if engine.is_key_pressed(49) then -- '1'
        self.selected_tool = "shovel"
        ui.set_slot_strip_selected("hud.hotbar", 0)
        ui.set_label_text("hud.hotbar_label", "Shovel")
    elseif engine.is_key_pressed(50) then -- '2'
        self.selected_tool = "seeds"
        ui.set_slot_strip_selected("hud.hotbar", 1)
        ui.set_label_text("hud.hotbar_label", "Potato seeds")
    elseif engine.is_key_pressed(51) then -- '3'
        self.selected_tool = "sword"
        ui.set_slot_strip_selected("hud.hotbar", 2)
        ui.set_label_text("hud.hotbar_label", "Sword")
    elseif engine.is_key_pressed(52) then -- '4'
        self.selected_tool = "bucket"
        ui.set_label_text("hud.hotbar_label", "Bucket")
        ui.set_slot_strip_selected("hud.hotbar", 3)
    end


    if engine.is_mouse_button_pressed(0) then -- LMB
        if self.selected_tool == "shovel" then
            -- engine.set_tile_region_from_grid("Ground", tileX, tileY, "cozyFarm", 80, 144, 54, 54)
            -- Replace grass tile on grounds layer with ground tile
            engine.set_tile_tileset_override("Ground", tileX, tileY, "cozyFarm", 491)
                -- place tilled ground tile in farmland layer above
            engine.set_tile_tileset_override("Farmland", tileX, tileY, "cozyFarm", 494)

            farm.set_tilled(tileX, tileY, true) -- custom game bindings
        elseif self.selected_tool == "seeds" then
            -- add crop on top of ground an farmland layer
            if farm.is_tilled(tileX, tileY) then
                engine.set_tile_tileset_override("Crops", tileX, tileY, "cozy_farm_free_version", 110)
            end
        elseif self.selected_tool == "sword" then
            -- TODO: define animation based on players direction
            engine.play_entity_animation(self.id, "player_fight_right", false)
            engine.play_sound("sword_hit", 0.7)
            local hitX, hitY, hitW, hitH = get_sword_hit_box(self)
            if hitX ~= nil then
                local hits = engine.get_entities_in_box(hitX, hitY, hitW, hitH)
                for _, entityId in ipairs(hits) do
                    if entityId ~= self.id then
                        -- No friendly fire!
                        local type = engine.get_entity_data(entityId, "type")
                        if type == "enemy" then
                            local health = engine.get_entity_data(entityId, "health")
                            local damage = engine.get_entity_data(self.id, "damage")
                            engine.set_entity_data(entityId, "health", health - damage)
                            print("Player hit "..entityId.." HP: "..health - damage)
                            local ex, ey = engine.get_entity_position(entityId)
                            if ex~= nil then
                                -- engine.emit_particles("blood_0", ex + 8, ey + 8, 32)
                            end
                        end
                    end
                end
            end
        end

        local tilled = farm.is_tilled(tileX, tileY)

        -- print("Tile "..tileX..","..tileY.." tilled = "..tostring(tilled))
    end

    -- Clear tile overrides with RMB
    if engine.is_mouse_button_pressed(1) then -- RMB
        engine.clear_tile_override("Ground", tileX, tileY)
        engine.clear_tile_override("Farmland", tileX, tileY)
        engine.clear_tile_override("Crops", tileX, tileY)
        farm.set_tilled(tileX, tileY, false) -- custom game bindings
    end

    local playerHealth = engine.get_entity_data(self.id, "health")
    -- Display game over label
    if playerHealth ~= nil and playerHealth < 0 and not self.game_over then
        self.game_over = true
        ui.create_label("hud.game_over_label", "hud")
        ui.set_label_text("hud.game_over_label", "GameOver")
        local viewportWidth, viewportHeight = engine.get_viewport_size()
        ui.set_label_text_color("hud.game_over_label", 1.0, 0.2, 0.2, 1.0)
        ui.set_label_scale("hud.game_over_label", 2.0)
        ui.set_label_position("hud.game_over_label", viewportWidth / 2 - 100, viewportHeight / 2 - 25)
    end

end

return M
