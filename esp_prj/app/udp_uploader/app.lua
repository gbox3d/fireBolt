--
-- Created by IntelliJ IDEA.
-- User: gunpower
-- Date: 2016. 10. 25.
-- Time: PM 2:59
-- To change this template use File | Settings | File Templates.
--

function startApp()

    udp_server = net.createServer(net.UDP)
    udp_server:on("receive",function(socket,play_load)
        print(play_load)

        local jsn_obj = cjson.decode(play_load)
        print(jsn_obj.seq)

        local temp_result = {result='ok',seq= jsn_obj.seq }
        socket:send( cjson.encode(temp_result) )

    end)
    udp_server:listen(1471)
end

