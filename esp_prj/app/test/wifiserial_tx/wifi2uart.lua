function wifi2UartStart()

    uart.setup(0,115200,8,0,1,0)
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
            global_c:send(data)
        end
    end, 0)

end

wifi2UartStart()



