wifi.setmode(wifi.STATION)
cfg = {ip = "192.168.9.20",netmask = "255.255.255.0",gateway="192.168.9.1"}
wifi.sta.setip(cfg)
wifi.sta.config("RD2GUNPOWER","808671004")
wifi.sta.connect()
srv=net.createServer(net.TCP)
srv:listen(80,function(conn)
    conn:on("receive",function(conn,payload)
        _, _, method, path, vars = string.find(payload, "([A-Z]+) /([^?]*)%??(.*) HTTP");print(path)
        conn:send("<h1> Hello, NodeMCU.</h1>")
        conn:close()
        print(node.heap());
    end)
end)