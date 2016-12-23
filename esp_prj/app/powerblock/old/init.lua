

print("es8266 for codeblock 0,0.1")

new_gpio = {[0]=3,[1]=10,[2]=4,[3]=9,[4]=2,[5]=1,[10]=12,[12]=6,[13]=7,[14]=5,[15]=8,[16]=0 }

gpio.mode(new_gpio[12],gpio.OUTPUT)
gpio.mode(new_gpio[13],gpio.OUTPUT)
gpio.write(new_gpio[12],gpio.LOW)
gpio.write(new_gpio[13],gpio.LOW)


pin_ap_led = new_gpio[4];
pin_connection_led = new_gpio[5];
--softuart_tx_pin = new_gpio[12];
pin_switch_sel_mode = new_gpio[14];

gpio.mode(pin_switch_sel_mode,gpio.INPUT)

gpio.mode(pin_ap_led,gpio.OUTPUT)
gpio.mode(pin_connection_led,gpio.OUTPUT)

gpio.write(pin_ap_led,gpio.HIGH)
gpio.write(pin_connection_led,gpio.HIGH)

dofile('wifi_setup.lua')

function start_UPDServer(port)
    print("udp  config server started :" .. port)

    s=net.createServer(net.UDP)

    cc_udp_socket = nil;

    s:on("receive",function(s,c)

        local packet = cjson.decode(c)

        cc_udp_socket = s;

        print(packet.cmd)

        rt = {}
        rt.id = 0
        if packet.id then
            rt.id = packet.id
        end

        if packet.cmd == 'ping' then
            rt.result = 'ok';
        elseif packet.cmd == 'save' then
            --print(packet.name)
            --print(json_obj.content)

            file.open(packet.name, "w")
            file.write(packet.content)
            file.close()
            rt.result = 'ok';

        elseif packet.cmd == 'reboot' then
            rt.result = 'ok';
            tmr.alarm(0,1000,1,function() node.restart() end)

        elseif packet.cmd == 'gpio' then
            rt.result = 'ok';
            --print(packet.port ..':'..packet.value)
            gpio.write(new_gpio[packet.port],packet.value)

        end


        s:send(cjson.encode(rt))


    end)
    s:listen(port)

end

function start_Udp2Uart(remote_port,remote_ip)

    print('central server ip '.. remote_ip .. ', port :' .. remote_port)
    --uart.setup(0,115200,8,0,1,0)

    cu=net.createConnection(net.UDP)
    --cu:on("receive",function(cu,c) print('bind :' .. c) end)
    -- 첫번째 인자는 원격지의 포트번호이다.
    cu:connect(remote_port,remote_ip)
    --buffer = ''
    --uart.when("data", 0x81 , function(data) if cu ~= nil then cu:send(data) end  end, 0)
    --uart.on("data", string.char(0x81) , function(data) if string.len( data ) == 34 then if cu ~= nil then cu:send(data) end  end end, 0)
    uart.on("data", string.char(0x81) , function(data)if cu ~= nil then cu:send(data) end  end, 0)
    --tmr.alarm(0,1000,1,function() cu:send('hello') end);

end

if gpio.read(pin_switch_sel_mode) == 1 then

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
                print("-start ap setup mode[ espudp/12345678 ]\n")
                start_UPDServer(1471)

            end
        else
            print("- connect ok find ip " .. ipaddr );
            tmr.stop(0)
            wifi.sta.setip(cfg)
            start_UPDServer(1471)
            start_Udp2Uart(5638,central_server_ip)

            gpio.write(pin_ap_led,gpio.LOW)
            print("- connect ok " ..  wifi.sta.getip() )

        end
    end)
else


    print("-start dev mode\n")
end





