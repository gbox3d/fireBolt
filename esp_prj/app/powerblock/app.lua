
function getHttpBody(packet)
    local _,ie = string.find(packet,"\r\n\r\n")
    local is2,_ = string.find(packet,"\r\n",ie);
    local body_length = string.sub(packet,ie+1,is2);
    is2 = is2+2
    local strTemp = string.sub(packet,is2,is2+tonumber("0x"..body_length));
    return strTemp
    --return cjson.decode(string.sub(packet,is2,is2+tonumber("0x"..body_length)))
end

local bLoop = true;

function requestLoop(port,ip)

    sk=net.createConnection(net.TCP, 0)
    sk:on("receive", function(sck, c)
        --[[
        --print(c)
        --test_str = c;
        local is,ie = string.find(c,"\r\n\r\n")
        local is2,_ = string.find(c,"\r\n",ie);
        body_length = string.sub(c,ie+1,is2);
        is2 = is2+2
        --print(string.sub(c,is2,is2+tonumber("0x"..body_length)));
        --local body_str = string.sub(c,is2,is2+tonumber("0x"..body_length));
        local json_obj = cjson.decode(string.sub(c,is2,is2+tonumber("0x"..body_length)))
        ]]--
        local json_obj = cjson.decode(getHttpBody(c))
        --print(json_obj.result);
        if(json_obj.result == 'ok') then
            print(json_obj.content.cmd)
            local ct = json_obj.content;
            if(ct.cmd == 'setdio') then
                gpio.mode(ct.pin,gpio.OUTPUT)
                gpio.write(ct.pin,ct.val)
            elseif(ct.cmd=='servo-swipe') then

            end

            end
        sk:close()
        if(bLoop == true) then
            tmr.alarm(0,2000,0,function() requestLoop(1883,"gunpowerpoint.iptime.org")  end)
        end

    end )
    sk:on("reconnection", function(sck,c) print('reconnect') end)
    sk:on("disconnection", function(sck,c) print('disconn') end)
    sk:on("sent", function(sck,c) print('sent') end)

    sk:on("connection", function(sck,c)
        print("connect ok")
        -- Wait for connection before sending.
        sk:send("GET /pop?id=test0001 HTTP/1.1\r\nHost: powerblock.com\r\nConnection: keep-alive\r\nAccept: */*\r\n\r\n",function() print('send ok') end)
    end)
    sk:connect(port,ip)

end

function startLoop()
    print('start request loop')
    requestLoop(1883,"gunpowerpoint.iptime.org")
end

function stopLoop()
    bLoop = false;
end
--requestLoop(1883,"192.168.9.4");