--GPIO Define
function initGPIO()
    --1,2EN   D1 GPIO5
    --3,4EN   D2 GPIO4
    --1A  ~2A   D3 GPIO0
    --3A  ~4A   D4 GPIO2

    gpio.mode(0,gpio.OUTPUT);--LED Light on
    gpio.write(0,gpio.LOW);

    gpio.mode(1,gpio.OUTPUT);gpio.write(1,gpio.LOW);
    gpio.mode(2,gpio.OUTPUT);gpio.write(2,gpio.LOW);

end

function setupAPMode()
    print("Ready to start soft ap")

    cfg={}
    cfg.ssid="spectrum"..node.chipid();
    cfg.pwd="12345678"
    wifi.ap.config(cfg)

    cfg={}
    cfg.ip="192.168.1.1";
    cfg.netmask="255.255.255.0";
    cfg.gateway="192.168.1.1";
    wifi.ap.setip(cfg);
    wifi.setmode(wifi.SOFTAP)

    str=nil;
    ssidTemp=nil;
    --collectgarbage();

    print("Soft AP started")
end

-- Build and return a table of the http request data
function parseHTTPReq (req)
    return string.match(req, "([A-Z]*) \/([^? ]*)")
end

function _(x) for v in pairs(x) do print(v) end end

--function startServer()
--    srv=net.createServer(net.TCP)
--    print("http server started")
--    srv:listen(80,function(conn)
--        conn:on("receive",function(conn,request)
--            local buf = ""
--            method, path = parseHTTPReq(request)
--            if (path == nil or path=="/" or path=="") then
--                path = "index.html"
--            end
--            print("] "..path)
--            if (string.len(path)==2) then
--                local l=string.sub(path,1,1)+0
--                local r=string.sub(path,2,2)+0
--                --print(l ..','.. r)
--                gpio.write(l,r);
--                conn:send('{"r":"ok"}')
--                --print(path.." direction")
--            else
--                print("[", method, ",", path, "]")
--                if (file.open(path, "r")) then
--                    local q=file.read()
--                    local x=""
--                    while q do
--                        x=x..(q and q or "")
--                        q=file.read()
--                    end
--                    --print(x)
--                    conn:send(x)
--                else
--                    conn:send("404")
--                end
--                file.close()
--            end
--            conn:close()
--            collectgarbage()
--        end)
--    end)
--end

function AsyncTCPSender_Safe(option)
    local test_set = false;
    local tempBuf = "";
    local over_size = 0;
    return function(data)
        local socket = option.getsocket()
        function _send(data)
            if test_set == true then tempBuf = tempBuf..data --print(tempBuf)
            else
                tempBuf = tempBuf..data
                if tempBuf:len()>0 then

                    if tempBuf:len() <= 1024 then
                        local _data = tempBuf;tempBuf=""
                        test_set=true
                        socket:send(_data,
                            function()
                                test_set=false;
                                if tempBuf:len()>0 then
                                    --print("remain :  " ..tempBuf)
                                    _send("") end
                            end
                        )
                    else -- over 1024
                        over_size = tempBuf:len() - 1024

                    end
                end
            end
        end
        if socket~=nil then _send(data) end
    end --function
end

function startServer()
    srv=net.createServer(net.TCP)
    print("http server started")
    srv:listen(80,function(conn)
        conn:on("receive", function(conn, payload)

            _, _, method, path, vars = string.find(payload, "([A-Z]+) /([^?]*)%??(.*) HTTP");print(path);

            if path == "cmd" then
                --gpio.write(newGPIO[14],gpio.HIGH)
                --크로스도메인 허용하기위한 헤더
                conn:send('HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin:*\r\n\r\n' .. '{"result":"ok"}')
                conn:close()
            else


            end



        end)
    end)
end

initGPIO()
setupAPMode()
startServer()