print('u2t0,1.1a')dofile('gpio.lua')dofile('wifi_setup.lua')dofile('u2u.lua')
function startup()
    gpio.write(pin_ap_led,gpio.HIGH)gpio.write(pin_connection_led,gpio.LOW)
    bconnect = false;
    step = 0;
    local function listap(t)
        if t~=nil then
            for ssid,v in pairs(t) do
                if(ssid == my_ssid) then print('find!');bconnect=true;break  end;
            end
        end
        step=0
    end
    wait_count=0 lighton=0 step=0
    tmr.alarm(0,500,1,
        function()
            wait_count=wait_count+1;
            if step == 0 then
                if bconnect == false then
                    if lighton==0 then gpio.write(pin_ap_led,gpio.LOW)lighton=1 else gpio.write(pin_ap_led,gpio.HIGH)lighton=0 end
                    wifi.sta.getap(listap);step=2
                else step=1;print('connect ap:'..my_ssid);dofile('app.lua')
                end
            elseif step==1 then
                tmr.stop(0)
                gpio.write(pin_ap_led,gpio.LOW)start_configServer(1471)wifi2UartStart(1470) step=100
            end
        end)
end
startup()



