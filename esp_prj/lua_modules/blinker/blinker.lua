
function blink_led(option)
    local status,pin,delay=1,option.pin,option.delay
    gpio.mode(pin,gpio.OUTPUT)
    gpio.write(pin,gpio.HIGH)
    local tmr_obj = tmr.create();
    return function(option)
        --local function loop() gpio.write(0,status);if(status) then status=0 else status=1 end end
        if(option=='stop-off') then tmr_obj:stop();gpio.write(pin,gpio.LOW)
        elseif option=='stop-on' then tmr_obj:stop();gpio.write(pin,gpio.HIGH)
        else
            local function loop() 
                gpio.write(pin,status);
                if(status==1) then status=0 
                else status=1 
                end 
                
                tmr_obj:alarm(delay,tmr.ALARM_AUTO,loop)
            end
            loop()
        end
    end
end

--blinker_0=blink_led({pin=0,delay=500,tmid=1})
--blinker_0("start")