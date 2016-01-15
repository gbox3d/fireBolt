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
    --cfg.pwd="12345678"
    cfg.pwd=nil -- 암호는 없게, 요로케하면
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


function setupSTMode()
    wifi.setmode(wifi.STATION)
    wifi.sta.autoconnect(0) --이전 세팅이 가끔끼어드는거 시러효!!
    wifi.sta.disconnect() -- 완전히 끝내고 다시하고시퍼 ~~

    cfg = {ip = "192.168.9.20",netmask = "255.255.255.0",gateway="192.168.9.1"}
    wifi.sta.setip(cfg)
    --[[
    wifi.sta.config("esptest","",
        1 --오토모드의 부활
    )
    --]]
    wifi.sta.config("RD2GUNPOWER","808671004",1)

    wifi.sta.connect()
    --wifi.setphymode(wifi.PHYMODE_B) -- 모드 N에 비해 한바이트씩 전송하는것은 오히려 속도가 빠름
    --wifi.sleeptype(wifi.NONE_SLEEP) -- 슬립모드방지,이 옵션을 설정하지않으면 주기적으로 느려짐
end

-- Build and return a table of the http request data
function parseHTTPReq (req)
    return string.match(req, "([A-Z]*) \/([^? ]*)")
end

--uri 디코딩
--local function unescape(s)
--    local rt, i, len = "", 1, #s
--    s = s:gsub('+', ' ')
--    local j, xx = s:match('()%%(%x%x)', i)
--    while j do
--        rt = rt .. s:sub(i, j-1) .. string.char(tonumber(xx,16))
--        i = j+3
--        j, xx = s:match('()%%(%x%x)', i)
--    end
--    return rt .. s:sub(i)
--end

function unescape (s)
    s = string.gsub(s, "+", " ")
    s = string.gsub(s, "%%(%x%x)", function (h)
        return string.char(tonumber(h, 16))
    end)
    return s
end

function parseurl (s,param)
    for k, v in string.gmatch( s, "([^&=?]+)=([^&=?]+)" ) do
        --t[k] = v
        if k == param then
            --print (k.." "..v)
            return unescape(v)
        end
    end
end


function sendLongFile(option)

    local path = option.path;
    local conn = option.socket;

    if (file.open(path, "r")) then

        function _loop()
            local q=file.read(1024)
            --print(q)
            if q ~= nil then
                conn:send(q,_loop)
            else
                file.close()
                option.callback()
                --conn:close()
            end
        end

        _loop()

    else
        conn:send("404",function() conn:close() end)
        file.close()
    end

end


function startServer()
    srv=net.createServer(net.TCP)
    print("http server started")

    srv:listen(80,function(conn)
        conn:on("receive", function(conn, payload)
            _, _, method, path, vars = string.find(payload, "([A-Z]+) /([^?]*)%??(.*) HTTP");
            if path == "json" then
                --http://192.168.9.20/cmd?json={"fn":"gpio","val":[1,0,1]}
                local json = parseurl(vars,"data")
                print(json)
                local jsn_obj = cjson.decode(json)
                print(jsn_obj.fn);
                if jsn_obj.fn == 'gpio' then

                    gpio.write(0,jsn_obj.val[1])
                    gpio.write(1,jsn_obj.val[2])
                    gpio.write(2,jsn_obj.val[3])

                    end

                --크로스도메인 허용하기위한 헤더
                conn:send('HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin:*\r\n\r\n' .. '{"result":"ok"}',function() conn:close() end)

            else
                sendLongFile({
                    path = path,
                    socket = conn,
                    callback = function() conn:close() end
                })

            end



        end)
    end)
end

initGPIO()
--setupAPMode()
setupSTMode()
startServer()

remaining, used, total=file.fsinfo()
print("\nFile system info:\nTotal : "..total.." Bytes\nUsed : "..used.." Bytes\nRemain: "..remaining.." Bytes\n")