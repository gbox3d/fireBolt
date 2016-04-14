local safe_sender;
local udp_socket;
local udp_server;
local blinker;
function start_UdpController(device_id,ip,ip_class,port,safe_delay)
    --uart.alt(uart_alt)
    --uart.setup(0,app_config.baud,8,0,1,0)
    print(port)
    if udp_server == nil then
        udp_server = net.createServer(net.UDP)
        udp_server:on("receive",
            function(socket,c)
                --uart.write(0,c)
                print(c)
                local jsn_obj = cjson.decode(c)
                if jsn_obj.fn == 'test' then
                    --gpio.write(0,gpio.HIGH)
                    --tmr.alarm(3,5000,0,function() gpio.write(0,gpio.LOW) print('ok')end)
                    --gpio.serout(control_led,1,{500,500},3)
                    blinker_control_led=blink_led({pin=control_led,delay=500,tmid=1,max_count=3})
                    blinker_control_led("start")
                elseif jsn_obj.fn == 'blink' then
                    blinker=blink_led(
                        {
                            pin=jsn_obj.pin,
                            delay=jsn_obj.delay,
                            tmid=1,
                            max_count=jsn_obj.max_count
                        })
                    blinker("start")
                elseif jsn_obj.fn == 'blink-stop' then
                    blinker("stop-off")
                elseif jsn_obj.fn=='gpio-set' then
                    --blinker_control_led("start")
                    gpio.mode(jsn_obj.pin,gpio.OUTPUT)
                    gpio.write(jsn_obj.pin,jsn_obj.val)

                end


            end)
        udp_server:listen(port)
    end

    udp_socket=net.createConnection(net.UDP)
    udp_socket:connect(port,ip_class .. "255")
    safe_sender = AsyncTCPSender_Safe({getsocket = function() return udp_socket end,delay=safe_delay})

    local info = {
        ip = ip,
        device_id = device_id
    }
    local str_info = cjson.encode(info)
    tmr.alarm(0,3000,tmr.ALARM_AUTO,function () safe_sender(str_info) end)

    --uart.on("data", parser , safe_sender, 0)
    
end
