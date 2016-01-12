
function startServer()
    srv=net.createServer(net.TCP)
    print("http server started")
    srv:listen(80,function(conn)
        conn:on("receive", function(conn, payload)

            _, _, method, path, vars = string.find(payload, "([A-Z]+) /([^?]*)%??(.*) HTTP")

            print(method.."["..path.."]"..vars)

            if path == "echo" then

                local _, _, rv, lv = string.find(vars, "(.*)=(.*)")

                local param = {
                    [rv] = lv
                }

                print(param['msg'])

                if param["msg"] ~= nil then
                    conn:send('{"result":"ok","msg" :"' ..param['msg'] .. '"}' )
                else
                    conn:send('{"result":"ok"}')
                end
            elseif path== "file-list" then
                local l = file.list()
                file_list = ''
                for k,v in pairs(l) do
                    --print("name :"..k..", size size:"..v)
                    file_list = file_list .. '{"name" :"'..k..'", "size":'..v..'},'
                end
                file_list = file_list..'{}'
                conn:send('{"result":"ok","list" : ['..file_list..']}')
            else
                conn:send('{"result":"ok","info":"it is http sample"}')

            end

            conn:close()

        end)
    end)
end

startServer();
