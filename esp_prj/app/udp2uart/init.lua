print("u2u 0,0.4")
new_gpio = {[0]=3,[1]=10,[2]=4,[3]=9,[4]=2,[5]=1,[10]=12,[12]=6,[13]=7,[14]=5,[15]=8,[16]=0}

pin_ap_led = new_gpio[4] pin_connection_led = new_gpio[5] softuart_tx_pin = new_gpio[12] pin_switch_sel_mode = new_gpio[14]
gpio.mode(pin_ap_led,gpio.OUTPUT) gpio.mode(pin_connection_led,gpio.OUTPUT) gpio.mode(softuart_tx_pin,gpio.OUTPUT) gpio.mode(pin_switch_sel_mode,gpio.INPUT)
gpio.write(pin_ap_led,gpio.LOW) gpio.write(pin_connection_led,gpio.LOW)

dofile('wifi_setup.lua')

function start_configServer(port)

    print("udp  config server started :" .. port)
    s=net.createServer(net.UDP)
    cc_udp_socket = nil;

    s:on("receive",function(s,c)
        local packet = cjson.decode(c)
        cc_udp_socket = s;
        rt = {}
        rt.id = 0
        if packet.id then rt.id = packet.id end
        if packet.cmd == 'ping' then rt.result = 'ok'
        elseif packet.cmd == 'save' then file.open(packet.name, "w") file.write(packet.content) file.close() rt.result = 'ok'
        elseif packet.cmd == 'reboot' then rt.result = 'ok' tmr.alarm(0,1000,1,function() node.restart() end)
        elseif packet.cmd == 'to-ip' then start_Udp2Uart(5638,packet.ip)
        elseif packet.cmd == 'uart' then softuart.write(softuart_tx_pin,115200,packet.data)
        elseif packet.cmd == 'uart_bin' then
            for i,v in pairs(packet.data) do
                softuart.write(softuart_tx_pin,115200,string.char(v))
            end
        end
        s:send(cjson.encode(rt))
    end)
    s:listen(port)
end

function start_Udp2Uart(remote_port,remote_ip) cu=net.createConnection(net.UDP) cu:connect(remote_port,remote_ip) uart.on("data", 0 , function(data)if cu ~= nil then cu:send(data) end  end, 0) end


if gpio.read(pin_switch_sel_mode) == 0 then
    wait_count = 0
    tmr.alarm(0,1000,1,function()
        local ipaddr = wifi.sta.getip()
        if ipaddr == nil then
            print('-wait connecting..')
            wait_count = wait_count + 1
            if wait_count > 10 then
                tmr.stop(0)

                wifi.setmode(wifi.SOFTAP)
                cfg={}
                cfg.ssid="espudp"
                cfg.pwd="12345678"
                wifi.ap.config(cfg)
                gpio.write(pin_connection_led,gpio.HIGH)
                start_configServer(1471)
            end
        else
            print("- connect ok find ip " .. ipaddr )
            tmr.stop(0)
            wifi.sta.setip(cfg)
            start_configServer(1471)
            gpio.write(pin_ap_led,gpio.HIGH)
            start_Udp2Uart(5638,central_server_ip)
            print("- connect ok " ..  wifi.sta.getip() )

        end
    end)

else
    gpio.write(pin_connection_led,gpio.HIGH)
    print("-start dev mode\n")
end





