function start_configServer(port)
    s=net.createServer(net.UDP)
    cc_udp_socket = nil;
    s:on('receive',function(s,c)
        local packet = cjson.decode(c)
        cc_udp_socket = s;
        rt = {} rt.result = 'ok' rt.id = 0 if packet.id then rt.id = packet.id end
        if packet.cmd == 'ping' then  rt.mac=wifi.sta.getmac()
        elseif packet.cmd == 'save' then file.open(packet.name, 'w') file.write(packet.content) file.close()
        elseif packet.cmd == 'reboot' then tmr.alarm(0,1000,1,function() node.restart() end)
        else rt.result='nocmd'
        end
        s:send(cjson.encode(rt))
    end)
    s:listen(port)
end