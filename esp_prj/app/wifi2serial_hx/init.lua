

print("es8266 for gunpower http2uart 0,0.2")

new_gpio = {[0]=3,[1]=10,[2]=4,[3]=9,[4]=2,[5]=1,[10]=12,[12]=6,[13]=7,[14]=5,[15]=8,[16]=0}
pin_ap_led = new_gpio[4];
pin_connection_led = new_gpio[5];
softuart_tx_pin = new_gpio[12];
pin_switch_sel_mode = new_gpio[14];

gpio.mode(pin_ap_led,gpio.OUTPUT)
gpio.mode(pin_connection_led,gpio.OUTPUT)
gpio.mode(softuart_tx_pin,gpio.OUTPUT)
gpio.mode(pin_switch_sel_mode,gpio.INPUT)

gpio.write(pin_ap_led,gpio.LOW)
gpio.write(pin_connection_led,gpio.LOW)

dofile('wifi_setup.lua')

function start_configServer(port)
    print("config server started at :" .. port)
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
        end

        s:send(cjson.encode(rt))


    end)
    s:listen(port)

end

function wifi2UartStart(port)

    --sensor_data = ''

    srv=net.createServer(net.TCP)

    print("http server started " .. port)

    srv:listen(port,function(conn)
        conn:on("receive", function(conn, payload)

            _, _, method, path, vars = string.find(payload, "([A-Z]+) /([^?]*)%??(.*) HTTP")

            --local lua_table = {}
            --lua_table.result = 'ok'
            --lua_table.data = sensor_data

            --크로스도메인 허용하기위한 헤더
            conn:send("HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin:*\r\n\r\n")
            --결과 메씨지 보내기
            conn:send('{"data":"' .. sensor_data ..'"}')
            conn:close()

        end)
    end)

    uart.when("data",0x81, function(data)

        sensor_data = data;
        --print(data)

    end, 0)

end

if gpio.read(pin_switch_sel_mode) == 0 then

    wait_count = 0


    tmr.alarm(0,1000,1,function()
        local ipaddr = wifi.sta.getip()
        if ipaddr == nil then
            print('-wait connecting..')
            wait_count = wait_count + 1
            if wait_count > 10 then
                tmr.stop(0)

                --[[
                wifi.setmode(wifi.SOFTAP)
                cfg={}
                cfg.ssid="espower"
                cfg.pwd="12345678"
                wifi.ap.config(cfg)
                --]]
                gpio.write(pin_connection_led,gpio.HIGH)
                gpio.write(pin_ap_led,gpio.HIGH)

                --print("-start ap setup mode[ espower/12345678 ]\n")
                start_configServer(1471)

            end
        else
            --print("-connect ok " .. ipaddr );
            tmr.stop(0)
            wifi.sta.setip(cfg)

            gpio.write(pin_ap_led,gpio.HIGH)

            start_configServer(1471)
            wifi2UartStart(80)

            print("- connect ok " ..  wifi.sta.getip() )

        end
    end)

else
    gpio.write(pin_connection_led,gpio.HIGH)
    print("-start dev mode\n")
    start_configServer(1471)
end



