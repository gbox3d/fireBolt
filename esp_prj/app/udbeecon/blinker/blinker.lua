function blink_led(option)
    local status,pin,delay,tmid=1,option.pin,option.delay,option.tmid
    local count = 0;
    local max_count = option.max_count;
    gpio.mode(pin,gpio.OUTPUT)
    gpio.write(pin,gpio.HIGH)
    tmr.stop(tmid)
    return function(option)
        --local function loop() gpio.write(0,status);if(status) then status=0 else status=1 end end
        if(option=='stop-off') then tmr.stop(tmid);gpio.write(pin,gpio.LOW)
        elseif option=='stop-on' then tmr.stop(tmid);gpio.write(pin,gpio.HIGH)
        else
            local function loop()
                gpio.write(pin,status);
                if(status==1) then status=0 else status=1;count = count+1
                end
                if( max_count > 0) then
                    if(max_count > count) then tmr.alarm(tmid,delay,0,loop) end
                else
                    tmr.alarm(tmid,delay,0,loop)
                end
            end
            loop()
        end
    end
end


