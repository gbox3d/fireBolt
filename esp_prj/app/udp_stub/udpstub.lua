function start_udpStub(port)

    print("udp  config server started :" .. port)
    s=net.createServer(net.UDP)
    cc_udp_socket = nil;

    s:on("receive",function(s,c)
        local packet = cjson.decode(c)
        cc_udp_socket = s
        rt = {id=0,result='ok'}
        if packet.id then rt.id = packet.id end
        if packet.cmd == 'ping' then rt.result = 'ok' rt.version = firm_version
        elseif packet.cmd == 'save' then file.open(packet.name, "w") file.write(packet.content) file.close()
        elseif packet.cmd == 'reboot' then rt.result = 'ok' tmr.alarm(0,1000,1,function() node.restart() end)
        --elseif packet.cmd == 'to-ip' then start_Udp2Uart(5638,packet.ip)
        elseif packet.cmd == 'uart' then uart.write(0,packet.data)
        --softuart.write(softuart_tx_pin,115200,packet.data)
        elseif packet.cmd == 'uart_bin' then for i,v in pairs(packet.data) do uart.write(0,string.char(v)) end
        elseif packet.cmd == 'uart_reset' then uart.setup(0,packet.baudrate,8,0,1,1) uart.on("data")
        elseif packet.cmd == 'uart_start' then uart.setup(0,packet.baudrate,8,0,1,1) uart.on("data",string.char(packet.parser),function(data) cc_udp_socket.write(data) end)
        else rt.result = 'nocmd'
        end
        s:send(cjson.encode(rt))
    end)
    s:listen(port)
end