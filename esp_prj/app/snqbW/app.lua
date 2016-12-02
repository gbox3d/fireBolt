--dofile("tcp_safe_sender.lua")
--dofile("packet.lua")
app_version='1.0.1'
app_status={}

timerid_Udpcaster = 0;
--timerid_UartChecker = 1;

--udp_work_socket = net.createConnection(net.UDP)
udp_server =net.createServer(net.UDP)


packet_dic = {
    default = function() local rt = {result = 'nocmd'} udp_server:send(cjson.encode(rt))  end
}

packet_dic["eval"] = function(packet) loadstring(packet.code)() end
packet_dic["stat"] = function(packet) local rt = {result = "ok",sta=app_status,ip=app_config.ip} udp_server:send(cjson.encode(rt)) end

function udp_safe_sender(data)
    udp_server:send(data)
end

function startup()

    print("start application..")
    
    function processRecv(s,c)
        --print(c)
        local packet = cjson.decode(c)
        --local rt = {result = 'ok',id = 0,ip=app_config.ip} if packet.id then rt.id = packet.id end

        if packet_dic[packet.cmd] ~= nil then
            packet_dic[packet.cmd](packet)
        else
            print("unknown packet")
            print(c)
            local rt = {result="nocmd"} udp_server:send(cjson.encode(rt))
            --rt.result='nocmd'
        end

    --[[
        --print(packet.cmd)
        if packet.cmd == 'ping' then  rt.mac=wifi.sta.getmac()
        --elseif packet.cmd == 'save' then file.open(packet.name, 'w') file.write(packet.content) file.close()
        --elseif packet.cmd == 'load' then if file.exists(packet.name) then file.open(packet.name, 'r') rt.data = file.read() file.close() end
        elseif packet.cmd == 'eval' then loadstring(packet.code)()
        else rt.result='nocmd'
        end
    ]]
        --udp_work_socket:send(cjson.encode(rt))
    end 
    udp_server:on("receive",processRecv)
    udp_server:listen(app_config.bc_port)
    print("udp listen at " .. app_config.bc_port)

    local ip = app_config.ip;
    local broad_ip = ip[1] .. ".".. ip[2].."." ..ip[3] .. ".255"
    print("udp broadcast at " .. broad_ip ..":" .. app_config.bc_port)
    local broadcast_socket = net.createConnection(net.UDP)
    broadcast_socket:connect(app_config.bc_port, broad_ip)
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




