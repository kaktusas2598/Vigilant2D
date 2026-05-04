print("hello from lua")

function test()
    print("test() called from C++")
end

function on_update(dt)
    print("dt:", dt)
end