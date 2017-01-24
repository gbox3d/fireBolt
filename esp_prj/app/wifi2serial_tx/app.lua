app_version='u2t0,1.2'
function wifi2UartStart(port)
    uart.setup(0,115200,8,0,1,1)
    sv=net.createServer(net.TCP, 30)
    global_c = nil
    sv:listen(port, function(c)
        if global_c~=nil then global_c:close() end
        global_c=c;gpio.write(pin_connection_led,gpio.HIGH)
        c:on('receive',function(sck,pl) softuart.write(softuart_tx_pin,115200,pl)	end)
        --c:on('receive',function(sck,pl) uart.write(0,pl)end)
        c:on('disconnection',function(sck,pl) gpio.write(pin_connection_led,gpio.LOW) global_c = nil  end)
    end)
    uart.on('data',0, function(data) if global_c~=nil then  global_c:send(data) end end, 0)

end

