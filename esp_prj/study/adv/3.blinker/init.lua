
--blinker=blink_led({pin=0,delay=500,tmid=1})
--blinker("stop"),blinker("start")
function blink_led(option)
    local status,pin,delay,tmid=1,option.pin,option.delay,option.tmid
    gpio.mode(pin,gpio.OUTPUT)
    gpio.write(pin,gpio.HIGH)
    return function(option)
        --local function loop() gpio.write(0,status);if(status) then status=0 else status=1 end end
        if(option=='stop-off') then tmr.stop(1);gpio.write(pin,gpio.LOW)
        elseif option=='stop-on' then tmr.stop(1);gpio.write(pin,gpio.HIGH)
        else
            local function loop() gpio.write(pin,status);if(status==1) then status=0 else status=1 end tmr.alarm(tmid,delay,0,loop) end
            loop()
        end
    end
end

blinker_0=blink_led({pin=0,delay=500,tmid=1})
blinker_0("start")