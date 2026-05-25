local M = {}

function M.on_create(self)
    print("[LUA] Item Drop created")
    local x, y = engine.get_entity_position(self.id)
    self.base_x = x or 0
    self.base_y = y or 0
    self.hover_time = 0.0
    self.pickup_delay = 0.25
end

function M.on_update(self, dt)
    self.hover_time = self.hover_time + dt

    if self.pickup_delay > 0.0 then
        self.pickup_delay = math.max(0.0, self.pickup_delay - dt)
    end
    
    local offsetY = math.sin(self.hover_time * 5.0) * 3.0
    engine.set_entity_position(self.id, self.base_x, self.base_y + offsetY)

    if self.pickup_delay > 0.0 then
        return
    end

    local dropX, dropY = engine.get_entity_centre(self.id)
    local playerX, playerY = engine.get_entity_centre("player")
    if dropX == nil or playerX == nil then
        return
    end

    local dx = playerX - dropX
    local dy = playerY - dropY
    local distance = math.sqrt(dx * dx + dy * dy)

    if distance <= 16.0 then
        local itemType = engine.get_entity_data(self.id, "item_type")
        local count = engine.get_entity_data(self.id, "count") or 1
        local inventory = require("scripts.lib.inventory")
        local added = inventory.add_item("inventory", itemType, count)
        
        print("Item picked up: " .. itemType .. " (x" .. count .. ")")
        engine.play_sound("pickup_item", 0.8)
        engine.emit_particles("coin_pickup_0", dropX, dropY, 18)
        engine.emit_particles("heal_sparkle_0", playerX, playerY, 10)

        -- TODO: insteawd of automatic effect apply - instead items will have to be 
        -- consumed to get effect - vegetables consumed for hp/energy, seeds planted etc.
        -- Temp for testing
        local playerHealth = engine.get_entity_data("player", "health") or 0
        engine.set_entity_data("player", "health", playerHealth + 10)

        if added > 0 then
            -- HACK: Allows player to know that hotbar UI needs to be refreshed
            engine.set_entity_data("player", "hotbar_dirty", true)

            engine.destroy_entity(self.id)
        end
    end
end

return M