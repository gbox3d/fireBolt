--dofile("tcp_safe_sender.lua")
--dofile("packet.lua")
app_version='1.0.0'
app_status={}

timerid_Udpcaster = 0;
timerid_UartChecker = 1;
udp_work_socket = net.createConnection(net.UDP)


function startup(port)

    print("start application..")
    
    function processRecv(s,c)
        --print(c)
        local packet = cjson.decode(c)
        local rt = {type="res",result = 'ok',id = 0,ip=app_config.ip} if packet.id then rt.id = packet.id end
        --print(packet.cmd)
        if packet.cmd == 'ping' then  rt.mac=wifi.sta.getmac()
        --elseif packet.cmd == 'save' then file.open(packet.name, 'w') file.write(packet.content) file.close()
        --elseif packet.cmd == 'load' then if file.exists(packet.name) then file.open(packet.name, 'r') rt.data = file.read() file.close() end
        elseif packet.cmd == 'eval' then loadstring(packet.code)()
        else rt.result='nocmd'
        end
        udp_work_socket:send(cjson.encode(rt))
    end 
    udp_work_socket:on("receive",processRecv)

    local ip = app_config.ip;
    local broad_ip = ip[1] .. ".".. ip[2].."." ..ip[3] .. ".255"
    print(broad_ip ..":" .. port)
    local broadcast_socket = net.createConnection(net.UDP)
    broadcast_socket:connect(port, broad_ip)
    
    --broadcast_socket:connect(31486, "192.168.9.3")
    --udp_safe_sender = AsyncTCPSender_Safe({getsocket = function() return broadcast_socket end,delay=25})

    startUdpCast = function()
        tmr.alarm(timerid_Udpcaster,2000,tmr.ALARM_AUTO,function()
            local data={sk=1,did=0,cid=chipid,rt,type="bc",aps=app_status,ip=app_config.ip}
            broadcast_socket:send(cjson.encode(data))
            --print('send ok..and ' .. node.heap() )
        end)
    end

    stopUdpCast = function()tmr.stop(timerid_Udpcaster) end

    --startUdpCast();

end


print("App version " .. app_version)




