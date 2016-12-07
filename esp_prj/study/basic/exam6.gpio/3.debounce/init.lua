function debounce (func)
    local last = 0
    local delay = 300 * 1000

    return function (...)
        local now = tmr.now()
        if now - last < delay then return end
        last = now
        return func(...)
    end
end

count = 0
gpio.mode(1,gpio.INT)
gpio.trig(1,"up",debounce(function ( )
count = count+1
    print("hit..." .. count)
end))