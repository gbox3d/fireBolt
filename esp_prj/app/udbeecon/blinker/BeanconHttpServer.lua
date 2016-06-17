function BeaconHttpServer(option)

    local This = {version='1.0.0'}

    local function sendLongFile(option)

        local path = option.path;
        local conn = option.socket;
        --print(path)
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

    -- Build and return a table of the http request data
    local function parseHTTPReq (req)
        return string.match(req, "([A-Z]*) \/([^? ]*)")
    end

    function unescape (s)
        s = string.gsub(s, "+", " ")
        s = string.gsub(s, "%%(%x%x)", function (h)
            return string.char(tonumber(h, 16))
        end)
        return s
    end

    local function parseurl (s,param)
        for k, v in string.gmatch( s, "([^&=?]+)=([^&=?]+)" ) do
            --t[k] = v
            if k == param then
                --print (k.." "..v)
                return unescape(v)
            end
        end
    end

    local srv=net.createServer(net.TCP)

    This.startup = function()
        srv:listen(80,function(conn)
            conn:on("receive", function(conn, payload)
                _, _, method, path, vars = string.find(payload, "([A-Z]+) /([^?]*)%??(.*) HTTP");

                if path == "json" then
                    local json = parseurl(vars,"data")
                    local str_res = '{"result":"ok"}'
                    local jsn_obj = cjson.decode(json)
                    if jsn_obj.fn == 'rf' then
                        if (file.open(jsn_obj.name, "r")) then
                            if(jsn_obj.offset) then
                                file.seek("set",jsn_obj.offset)
                            end
                            local tmp
                            if jsn_obj.length then
                                tmp = file.read(jsn_obj.length)
                            else
                                tmp = file.read()
                            end
                            local temp = {result='ok',data=tmp }
                            str_res = cjson.encode(temp)
                        end
                        file.close()
                    elseif jsn_obj.fn == 'wf' then
                        print('write file : ' .. jsn_obj.name)
                        file.open(jsn_obj.name, jsn_obj.fg)
                        for i,v in ipairs(jsn_obj.content) do file.write(string.char(v)) end
                        file.close()
                    elseif jsn_obj.fn == 'rs' then
                        print('restart ')
                        tmr.alarm(0,1500,tmr.ALARM_SINGLE,function() node.restart() end)
                    end

                    if(option.callback) then
                        local temp_res = option.callback(jsn_obj)
                        if(temp_res) then str_res = temp_res end
                    end

                        --크로스도메인 허용하기위한 헤더
                    conn:send('HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin:*\r\n\r\n' .. str_res,function() conn:close() end)
                elseif path == "" then
                    print('index page')
                    sendLongFile({
                        path = "index.html",
                        socket = conn,
                        callback = function() conn:close() print('comple index.html') end
                    })
                else
                    print('requst ' .. path)
                    sendLongFile({
                        path = path,
                        socket = conn,
                        callback = function() conn:close() print('comple ' .. path) end
                    })

                end



            end)


        end)
    end

    return This;


end
