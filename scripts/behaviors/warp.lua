local M = {}

function M.on_update(self, dt)
    local warpX, warpY = engine.get_entity_centre(self.id)
    local playerX, playerY = engine.get_entity_centre("player")
    if warpX == nil or playerX == nil then
        return
    end

    local dx = playerX - warpX
    local dy = playerY - warpY
    local distance = math.sqrt(dx * dx + dy * dy)

    local radius = engine.get_entity_data(self.id, "radius") or 14
    if distance > radius then
        return
    end

    local targetMap = engine.get_entity_data(self.id, "target_map")
    local targetSpawn = engine.get_entity_data(self.id, "target_spawn")

    if targetMap ~= nil and targetSpawn ~= nil then
        engine.request_map_warp(targetMap, targetSpawn)
    end
end

return M