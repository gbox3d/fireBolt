wifi.setmode(wifi.STATIONAP)
-- 30s time out for a inactive client
sv = net.createServer(net.TCP, 30)
-- server listens on 80, if data received, print data to console and send "hello world" back to caller
sv:listen(1471, function(c)
    print("new connect")
    c:on("receive", function(c, pl)
        print(pl)
        c:send('S'..pl)
        print(node.heap())
    end)
    c:send("hello world")
end)

