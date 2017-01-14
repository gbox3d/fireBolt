--dofile("tcp_safe_sender.lua")

app_version='1.4.1'
app_status = {fsm=0};

timerid_Udpcaster = tmr.create();
--timerid_UartChecker = tmr.create();
network_latency = 0;
last_nt_tick = tmr.now();
--udp_work_socket = net.createServer(net.UDP)
--master_socket = net.createConnection(net.UDP)

master_socket = net.createUDPSocket()

local ip = app_config.ip;
local broad_ip = ip[1] .. ".".. ip[2].."." ..ip[3] .. ".255" 

--master_socket:connect(app_config.bc_port, broad_ip)

udp_safe_sender = AsyncSender_Safe({getsocket = function() return master_socket end})

packet_dic = {
    default = function() local rt = {result = 'nocmd'} udp_server:send(cjson.encode(rt))  end
}

packet_dic["eval"] = function(packet) loadstring(packet.code)() end
packet_dic["eval2"] = function(packet) 
    __rt={type="res",id=packet.id} 
    loadstring(packet.code)() 
    udp_safe_sender(cjson.encode(__rt));
end
--packet_dic["stat"] = function(packet) local rt = {result = "ok",sta=app_status,ip=app_config.ip} udp_server:send(cjson.encode(rt)) end

function startup()

    print("App version " .. app_version .. " start..")

    function processRecv(s,c)
        last_nt_tick = tmr.now()        
        if c:byte(1,1) == 123 then -- check '{}'
            local packet = cjson.decode(c)
            if packet.cmd ~= nil then
                if packet_dic[packet.cmd] ~= nil then
                    packet_dic[packet.cmd](packet)
                else
                    print("unknown packet")
                    print(c)
                    local rt = {result="nocmd"} udp_server:send(cjson.encode(rt))
                end
            end

        end 

    end
    master_socket:on("receive",processRecv)
    master_socket:listen(app_config.data_port)
    

    --[[
    local ip = app_config.ip;
    local broad_ip = ip[1] .. ".".. ip[2].."." ..ip[3] .. ".255"
    print(broad_ip ..":" .. app_config.bc_port)

    local broadcast_socket = net.createConnection(net.UDP)
    --broadcast_socket:on("receive",processRecv)
    broadcast_socket:connect(app_config.bc_port, broad_ip)
    --broadcast_socket:connect(31486, "192.168.9.3")
    --udp_safe_sender = AsyncTCPSender_Safe({getsocket = function() return broadcast_socket end,delay=25})
    ]]

    startUdpCast = function()    
        --tmr.alarm(timerid_Udpcaster,app_config.cast_delay,tmr.ALARM_AUTO,
        print("cast delay :" .. app_config.cast_delay)
        timerid_Udpcaster:alarm(app_config.cast_delay,tmr.ALARM_AUTO,
            function()
                local delta = tmr.now() - last_nt_tick
                --if delta < 0 then delta = delta + 2147483647 end -- proposed because of delta rolling over, https://github.com/hackhitchin/esp8266-co-uk/issues/2
                if delta < 0 then delta = 0 last_nt_tick = tmr.now() end
                network_latency = delta;
                if(network_latency > 5000000) then -- delay over 5 sec then wakeup broadcasting
                    master_socket:send(app_config.bc_port,broad_ip,cjson.encode({sk=1,did=0,cid=chipid,type="bc",aps=app_status}))
                    --master_socket:send(cjson.encode({sk=1,did=0,cid=chipid,type="bc",aps=app_status}))
                end
            end)
    end

    stopUdpCast = function() timerid_Udpcaster:stop() end
    startUdpCast();    
end



