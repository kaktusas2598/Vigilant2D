local M = {}

function M.on_create(self)
    print("slime created")
end

function M.on_update(self, dt)
    -- print("Delta time: "..dt)
end

return M
