wifi.setmode(wifi.STATION)
cfg = {
    ip = "192.168.0.20",
    netmask = "255.255.255.0",
    gateway="192.168.0.1"
}
wifi.sta.setip(cfg)
wifi.sta.config("RD2GUNPOWER","808671004")
wifi.sta.connect()

target_status = 0;

function start_udpStub(port)
    print("udp  config server started :" .. port)
    s=net.createServer(net.UDP)
    cc_udp_socket = net.createConnection(net.UDP);
    s:on("receive",function(skt,c)
        --print(c)
        local packet = cjson.decode(c)
        rt = {id=0,result='ok'}
        if packet.id then rt.id = packet.id end
        if packet.ip then cc_udp_socket:connect(port,packet.ip) end
        if packet.cmd == 'req' then rt.status = target_status
        elseif packet.cmd == 'up' then rt.result='ok' target_status=1
        elseif packet.cmd == 'down' then rt.result='ok' target_status=0
        else rt.result = 'nocmd'
        end
        --print(cjson.encode(rt))
        cc_udp_socket:send(cjson.encode(rt))
    end)
    s:listen(port)

end

start_udpStub(1471)

