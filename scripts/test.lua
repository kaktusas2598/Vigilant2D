print("[Lua] Engine init")

function test()
    print("test() called from C++")
end

local M = {}

function M.on_create()
    print("Luad behavior created")
end


function M.on_update(dt)
    -- print("dt:", dt)
end

return M