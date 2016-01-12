function appStart(port)
    print('start port ' .. port)
    tmr.alarm(0,500,0,function()
        uart.setup(0,9600,8,0,1,1)
        sv=net.createServer(net.TCP, 30)
        global_c = nil
        sv:listen(port, function(c)
            if global_c~=nil then global_c:close() end
            global_c=c;
            print('client connect');
            --c:on('receive',function(sck,pl) softuart.write(softuart_tx_pin,115200,pl)	end)
            --c:on('disconnection',function(sck,pl) gpio.write(pin_connection_led,gpio.LOW) global_c = nil  end)
        end)
        --uart.on('data',0, function(data) if global_c~=nil then  global_c:send(data)end end, 0)
    end)

end

--연속해서 보내는 예제
function sendTest()
    index = 1;
    data_table = {"a","b","c","d","e" }
    function _test()
        if index <=5 then
            if global_c~=nil then  global_c:send(data_table[index],function () tmr.alarm(1,10,0,_test) end) index = index+1 end
        end
    end
    _test();
end