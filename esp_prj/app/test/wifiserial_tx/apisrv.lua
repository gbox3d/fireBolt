
local decodeURI
do
    local char, gsub, tonumber = string.char, string.gsub, tonumber
    local function _(hex) return char(tonumber(hex, 16)) end

    function decodeURI(s)
        s = gsub(s, '%%(%x%x)', _)
        return s
    end
end

function ApiServer_start()
    srv=net.createServer(net.TCP)
    print("http server started")
    srv:listen(80,function(conn)
        conn:on("receive", function(conn, payload)

            local _, _, method, path, vars = string.find(payload, "([A-Z]+) /([^?]*)%??(.*) HTTP")

            vars = string.gsub(vars, "+", " ")

            vars = decodeURI(vars)

            --print(method.."["..path.."]"..vars)

            conn:send("HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin:*\r\n\r\n")

            --local _, _, _, lv = string.find(vars, "(.*)=(.*)")

            local _,lv = string.match(vars, "([^=]*)=(.*)")

            local json_obj = cjson.decode(lv)

            local rts = {}
            rts.result = 'ok'

            --print(lv);

            if path=='file-save' then

                file.open(json_obj.name, "w")

                for i,value in ipairs(json_obj.content) do file.writeline(value) end

                file.close()
                rts.msg = 'saved'

            elseif path=='compile' then
                --print(json_obj.name)
                node.compile(json_obj.name)
                rts.msg = 'compile'
                --file.remove(json_obj.name)
            elseif path=='restart' then
                tmr.alarm(0,100,0, function() node.restart()  end)
                rts.msg = 'reboot'
            else
            end

            conn:send(cjson.encode(rts))
            conn:close()

        end)
    end)
end

ApiServer_start()
