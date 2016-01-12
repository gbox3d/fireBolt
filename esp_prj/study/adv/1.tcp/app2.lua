dofile('tcp_safe_sender.lua')

function appStart(port)
    print('start port ' .. port)
    -- 0.5초쉬었다가..
    tmr.alarm(0,500,0,
        function()
            uart.setup(0,9600,8,0,1,1)
            sv=net.createServer(net.TCP, 30)
            global_c = nil
            sv:listen(port, function(c) if global_c~=nil then global_c:close() end;global_c=c end)
            uart.on('data',0,AsyncTCPSender_Safe({getsocket = function() return global_c end}),0)
        end
        )
end

sk=net.createConnection(net.TCP, 0)
sk:dns("www.nodemcu.com",function(conn,ip) print(ip) end)
sk = nil