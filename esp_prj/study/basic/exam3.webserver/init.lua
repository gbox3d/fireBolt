wifi.setmode(wifi.STATION)
cfg = {ip = "192.168.9.20",netmask = "255.255.255.0",gateway="192.168.9.1"}
wifi.sta.setip(cfg)
wifi.sta.config("test","12345678",1)

print("it is http exam 1")

gpio.mode(0,gpio.OUTPUT)
gpio.write(0,gpio.HIGH)

srv=net.createServer(net.TCP)

srv:listen(80,function(conn)
    conn:on("receive",function(conn,payload)
        _, _, method, path, vars = string.find(payload, "([A-Z]+) /([^?]*)%??(.*) HTTP");
        print(path)
        if path=="on" then gpio.write(0,gpio.HIGH) elseif path=="off" then gpio.write(0,gpio.LOW) end
        conn:send('HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin:*\r\n\r\n',
            function(sent)
                print(" : send ok!");
                conn:send('next ya?',function() print(" : send fin!"); conn:close() end)

            end
            )
        --tmr.alarm(0,10,0,function() conn:send("whats up?") conn:close() end)
    end)
end)

