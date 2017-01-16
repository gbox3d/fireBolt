wifi.setmode(wifi.STATION)

cfg = {
    ip = "192.168.9.108",
    netmask = "255.255.255.0",
    gateway="192.168.9.177"
}
wifi.sta.setip(cfg)
wifi.sta.config("--","")
wifi.sta.connect()

function startServer(port)
    srv=net.createServer(net.TCP)
    print("http server started")
    srv:listen(port,function(conn)
        conn:on("receive", function(conn, payload)

            _, _, method, path, vars = string.find(payload, "([A-Z]+) /([^?]*)%??(.*) HTTP")

            print(method.."["..path.."]"..vars)

            conn:send('ok');
            conn:close()

        end)
    end)
end




function startUdpServer(port)
    print("udp  started")
    s=net.createServer(net.UDP)
    s:on("receive",function(s,c) print(c) end)
    s:listen(port)

end

startServer(80)
startUdpServer(33333)