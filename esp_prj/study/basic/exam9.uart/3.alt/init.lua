--
-- Created by IntelliJ IDEA.
-- User: gunpower
-- Date: 2016. 3. 3.
-- Time: 오전 11:16
-- To change this template use File | Settings | File Templates.
--

theApp={version='0.1.0'}
function startApp()
    print("uart mode start!")
    tmr.delay(1000000);
    --uart.setup(0,9600,8,0,1,0)
    uart.alt(1);
    uart.setup(0,115200,8,0,1,0)
    --uart.setup( id, baud, databits, parity, stopbits, echo )
    -- when '\r' is received.
    uart.on("data", string.char(0x0d),
        function(data)
            if data=="+++quit\r" then
                print("uart mode quit ")
                uart.setup(0,9600,8,0,1,1)
                uart.on("data")
            else
                print("receive from uart:", data)
            end
        end,
        0 -- not run_lua
    )

    tmr.alarm(0,1000,tmr.ALARM_AUTO,function() uart.write(0,'hello')uart.write(0,"\r\n") end)


end

