wifi.setmode(wifi.STATIONAP)
--cfg = {ip = "192.168.219.20",netmask = "255.255.255.0",gateway="192.168.219.1"}
--wifi.sta.setip(cfg)
--wifi.sta.config("","")
--wifi.sta.connect()
srv=net.createServer(net.TCP)
srv:listen(80,function(conn)
    conn:on("receive",function(conn,payload)
        --_, _, method, path, vars = string.find(payload, "([A-Z]+) /([^?]*)%??(.*) HTTP");print(path)
        conn:send("<h1> Hello, NodeMCU.</h1>")
        --print(node.heap())
        --conn:close()
    end)
    conn:on("sent",function() conn:close() print(node.heap())  end)
end)


