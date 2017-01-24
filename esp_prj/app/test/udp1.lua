
wifi.setmode(wifi.STATION)

cfg = {
    ip = "192.168.9.108",
    netmask = "255.255.255.0",
    gateway="192.168.9.177"
}
wifi.sta.setip(cfg)
wifi.sta.connect()
wifi.sta.config("GUNPOWER_PI_1","")



function startUdpServer(port)
    print("udp  started")
    s=net.createServer(net.UDP)

    s:on("receive",function(s,c)

        local packet = cjson.decode(c)

        cc_socket = s;

        print(packet.cmd)

        if packet.cmd == 'echo' then
            s:send('echo :' .. packet.msg)
        elseif packet.cmd == 'save' then
            --print(packet.name)
            --print(json_obj.content)

            file.open(packet.name, "w")
            file.write(packet.content)
            file.close()
        end


    end)
    s:listen(port)

end

startUdpServer(1471)