
gpio.mode(new_gpio[4],gpio.OUTPUT)
gpio.mode(new_gpio[5],gpio.OUTPUT)
gpio.mode(new_gpio[14],gpio.INPUT)

gpio.write(new_gpio[4],gpio.LOW)
gpio.write(new_gpio[5],gpio.LOW)

tmr.alarm(0,1000,1,function()
    local ipaddr = wifi.sta.getip();
    --print(ipaddr);
    if ipaddr == nil then
        print('-wait connecting..')
    else
        print("-connect ok " .. ipaddr );
        --print(ipaddr);
        --print("\n")
        tmr.stop(0)

        gpio.write(new_gpio[4],gpio.HIGH)

        if gpio.read(new_gpio[14]) == 0 then

            print("-server start...\n")
            tmr.delay(500000) --100ms
            setup_server()
        else
            print("-start dev mode\n")
        end
    end
end)


function setup_server()
    uart.setup(0,115200,8,0,1,0)
    sv=net.createServer(net.TCP, 30)
    global_c = nil

    sv:listen(1470, function(c)
        if global_c~=nil then
            global_c:close()

        end
        global_c=c
        gpio.write(new_gpio[5],gpio.HIGH)
        c:on("receive",function(sck,pl)	uart.write(0,'%'..pl..'%') end)
        c:on('disconnection',function(sck,pl) gpio.write(new_gpio[5],gpio.LOW) global_c = nil  end)
    end)

    uart.on("data",0, function(data)
        if global_c~=nil then
            global_c:send(data)
        end


    end, 0)

end
