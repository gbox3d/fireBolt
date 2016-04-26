
function startApp()
    uart.setup(0,9600,8,0,1,0)
    uart.on("data",'}',
        function(data)
            if data ~= "{\"I\":1,\"S\":'D',\"T\":0}" then
                print("wrong data")

            end

        end,
        0 -- not run_lua
    )
end