
gpio.mode(pin_ap_led,gpio.OUTPUT)
gpio.mode(pin_connection_led,gpio.OUTPUT)
gpio.mode(softuart_tx_pin,gpio.OUTPUT)
gpio.mode(pin_switch_sel_mode,gpio.INPUT)

gpio.write(pin_ap_led,gpio.LOW)
gpio.write(pin_connection_led,gpio.LOW)

tmr.alarm(0,1000,1,function()
    local ipaddr = wifi.sta.getip();
    if ipaddr == nil then
        print('-wait connecting..')
    else
        print("-connect ok " .. ipaddr );
        tmr.stop(0)
        gpio.write(pin_ap_led,gpio.HIGH)

        if gpio.read(pin_switch_sel_mode) == 0 then

            --print("-server start...\n")
            --tmr.delay(500000) --100ms
            setup_server()
        else
            print("-start dev mode\n")
        end
    end
end)


function setup_server()

    --uart.setup(0,115200,8,0,1,0)
    sv=net.createServer(net.TCP, 30)
    global_c = nil

    sv:listen(1470, function(c)
        if global_c~=nil then
            global_c:close()

        end

        --c:send('welcome~')

        global_c=c
        gpio.write(pin_connection_led,gpio.HIGH)
        c:on("receive",function(sck,pl) softuart.write(softuart_tx_pin,115200,pl)	end)
        c:on('disconnection',function(sck,pl) gpio.write(pin_connection_led,gpio.LOW) global_c = nil  end)
    end)

    uart.on("data",0, function(data)
        if global_c~=nil then
            --print("send data:", data)
            --global_c:send('a')
            global_c:send(data)
        end
    end, 0)

end


