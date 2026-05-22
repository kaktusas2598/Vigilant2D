local M = {}

function M.grid_local_position(index, columns, startX, startY, spacingX, spacingY)
    local col = index % columns
    local row = math.floor(index / columns)
    return startX + col * spacingX, startY - row * spacingY
end

return M