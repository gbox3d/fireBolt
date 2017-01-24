wifi.setmode(wifi.STATION)
cfg = {ip = "192.168.42.20",netmask = "255.255.255.0",gateway="192.168.42.1"}
wifi.sta.setip(cfg)
wifi.sta.config("GUNPIE","")
wifi.sta.connect()
newGPIO = {[0]=3,[1]=10,[2]=4,[3]=9,[4]=2,[5]=1,[10]=12,[12]=6,[13]=7,[14]=5,[15]=8,[16]=0 }
gpio.mode(newGPIO[14],gpio.OUTPUT)
function startServer()
    srv=net.createServer(net.TCP)
    print("http server started")
    srv:listen(80,function(conn)
        conn:on("receive", function(conn, payload)

            _, _, method, path, vars = string.find(payload, "([A-Z]+) /([^?]*)%??(.*) HTTP");print(path);

            if path == "on" then
                gpio.write(newGPIO[14],gpio.HIGH)

            elseif path== "off" then
                gpio.write(newGPIO[14],gpio.LOW)
            else

            end

            --크로스도메인 허용하기위한 헤더
            conn:send('HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin:*\r\n\r\n' .. '{"result":"ok"}',function() conn:close() end)
            --conn:send('HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin:*\r\n\r\n' .. '{"result":"ok"}')
            --결과 메씨지 보내기
            --conn:send('{"test":123}')
            --conn:send('{"test":456}')
            --conn:send("<h1> Hello, NodeMCU.</h1>")
            --conn:close()

        end)
    end)
end

startServer();

