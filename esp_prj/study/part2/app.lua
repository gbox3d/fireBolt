
gpio.mode(pin_ap_led,gpio.OUTPUT)
gpio.mode(pin_connection_led,gpio.OUTPUT)
gpio.mode(softuart_tx_pin,gpio.OUTPUT)
gpio.mode(pin_switch_sel_mode,gpio.INPUT)

gpio.write(pin_ap_led,gpio.LOW)
gpio.write(pin_connection_led,gpio.LOW)

print("- gunpower wifi2serial v0.1.0")

tmr.alarm(0,1000,1,function()
    local ipaddr = wifi.sta.getip();
    if ipaddr == nil then
        print('-wait connecting..')
    else
        tmr.stop(0)
        print("-connect ok " .. ipaddr );

        gpio.write(pin_ap_led,gpio.HIGH)

        if gpio.read(pin_switch_sel_mode) == 0 then

            print("- server start...")
            --tmr.delay(500000) --100ms
            tmr.alarm (0,300,0,
                function()
                    uart.setup(0,115200,8,0,1,0)
                    setup_server()
                end)

        else
            print("-start dev mode\n")
        end
    end
end)


function setup_server()

    sv=net.createServer(net.TCP, 30)
    global_c = nil

    sv:listen(1470, function(c)
        if global_c~=nil then
            global_c:close()

        end

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

