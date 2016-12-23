
function sensorTrigger()
    gpio.mode(4, gpio.INT, gpio.PULLUP)
    gpio.trig(4, "both", function(l)
        gpio.write(3,bit.bnot(l)+2)
    end)
    print("sensor initiated")
end

function startServer()
    srv=net.createServer(net.TCP)
    print("AP started")
    srv:listen(80,function(conn)
        conn:on("receive", function(conn, payload)
            
            --print(payload)
            
            _, _, method, url, vars = string.find(payload, "([A-Z]+) /([^?]*)%??(.*) HTTP")
            print(method.."["..url.."]"..vars)
            
            --if method == ""
            _,_,rv,lv = string.find(vars,"(.*)=(.*)")
            print(rv)
            print(lv)
            
            conn:send("<html>")
            conn:send("<head><meta name=\"viewport\" content=\"width=device-width, user-scalable=no, initial-scale=1.0, maximum-scale=1.0, minimum-scale=1.0\"></head>")
            conn:send("<body><h1>RayTracer</h1>")

            conn:send("</body></html>")
            conn:close()
        end)
    end)
end

aplist={}
wifi.sta.getap(function(t)
    aplist = t
    startServer()
end)

print("device on")