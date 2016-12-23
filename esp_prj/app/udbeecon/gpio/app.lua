
function start_UdpController(device_id,ip,port)

    --start_stepper();
    print(port)
    local _,_, ip_class = string.find(ip,"(%d+%.%d+%.%d+.)");
    print(ip_class)
    if udp_server == nil then
        udp_server = net.createServer(net.UDP)
        udp_server:on("receive",
            function(socket,c)
                print(c)
                local jsn_obj = cjson.decode(c)
                if jsn_obj.fn == 'test' then
                    --stepper_move(1024,(1),1,function() print('end of step moter move') end)
                    print('test')
                elseif jsn_obj.fn=='gpio-set' then
                    --gpio.mode(jsn_obj.pin,gpio.OUTPUT)
                    gpio.write(jsn_obj.pin,jsn_obj.val)
                end


            end)
        udp_server:listen(port)
        print('start udp server'.. port);
    end


    
end
