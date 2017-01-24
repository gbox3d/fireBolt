function start_Udp2Uart(remote_port,remote_ip,parser,safe_delay,uart_alt)
    uart.alt(uart_alt)
    uart.setup(0,app_config.baud,8,0,1,0)
    if udp_server == nil then
        udp_server = net.createServer(net.UDP)
        udp_server:on("receive",function(socket,c) uart.write(0,c) end)
        udp_server:listen(app_config.local_info.port)
    end
    cu=net.createConnection(net.UDP)
    cu:connect(app_config.remote.port,app_config.remote.ip)
    --cu:on("receive",function(cu,c) uart.write(0,c) end)
    --string.char(0x0d)
    safe_sender = AsyncTCPSender_Safe({getsocket = function() return cu end,delay=safe_delay})
    uart.on("data", parser , safe_sender, 0) 
    
end
