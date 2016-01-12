dofile('tcp_safe_sender.lua')

function wifi2UartStart(port)
    uart.setup(0,9600,8,0,1,1)
    sv=net.createServer(net.TCP, 30)
    global_c = nil
    sv:listen(port, function(c)
        if global_c~=nil then global_c:close() end global_c=c;
        c:on('receive',function(sck,pl) uart.write(0,pl)end)
        c:on('disconnection',function(sck,pl) global_c = nil  end)
    end)
    --safe_sender = AsyncTCPSender_Safe({getsocket = function() return global_c end})
    safe_sender = AsyncTCPSender_Safe({getsocket = function() return global_c end,delay=25})
    uart.on('data',0, safe_sender, 0)
    --uart.on('data',0, function(data) if global_c~=nil then  global_c:send(data) end end, 0)
end

print('start tcp2uart port 2323')
tmr.alarm(0,500,0,function()wifi2UartStart(2323) end)
