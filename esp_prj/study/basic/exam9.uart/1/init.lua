theApp={version='0.1.0'}
function startApp()
    print("uart mode start!")
    tmr.delay(1000000);
    uart.setup(0,9600,8,0,1,0)
    --uart.setup(0,115200,8,0,1,0)
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
end
