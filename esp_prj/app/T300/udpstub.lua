function start_udpStub(port)
    print("udp  config server started :" .. port)
    s=net.createServer(net.UDP)
    cc_udp_socket = net.createConnection(net.UDP);
    s:on("receive",function(skt,c)
        print(c)local packet = cjson.decode(c)
        rt = {id=0,result='ok'}
        if packet.id then rt.id = packet.id end
        if packet.ip then cc_udp_socket:connect(port,packet.ip) end
        if packet.cmd == 'ping' then rt.result = 'ok' rt.version = firm_version
        elseif packet.cmd == 'save' then file.open(packet.name, "w") file.write(packet.content) file.close()
        elseif packet.cmd == 'reboot' then rt.result = 'ok' tmr.alarm(0,1000,1,function() node.restart() end)
        elseif packet.cmd == 'pout' then rt.result = 'ok';gpio.mode(packet.index,gpio.OUTPUT);gpio.write(packet.index,packet.val)
        else rt.result = 'nocmd'
        end
        cc_udp_socket:send(cjson.encode(rt))
    end)
    s:listen(port)

end