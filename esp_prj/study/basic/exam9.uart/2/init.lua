theApp = {verion = '0.1.0' }

pin_num = 5; -- gpio14
gpio.mode(pin_num,gpio.OUTPUT)

function startApp()
    print("uart mode start!")
    --tmr.delay(1000000) -- delay 1 sec
    uart.setup(0,115200,8,0,1,0)
    -- 아스키코드값을 직접넣어주려면 string.char를 사용한다.
    --  0x0d, 즉 \r 를 만나면 데이터를 끈어 넘겨준다.(마샬링) 넘겨주는 데이터는 마샬링 바이트까지 포함된다.
    uart.on("data", 0, --parse by 0x0d
        function(data)
            --루아는 1부터시작한다.
            --[[local code = string.byte(data,1)
            if code == 81 then
                uart.setup(0,115200,8,0,1,1)
                uart.on("data") --uart 종료
                tmr.delay(1000000)
                print("\r\nuart mode quit\r\n----------------\r\n")
            elseif code == 69 then
                --echo
                uart.write(0,data)uart.write(0,"\r\n")
            end
            ]]--

            --한바이트씩 단문자 처리 하기.
            data:gsub(".", function(code)
                if code == "Q" then
                    uart.setup(0,115200,8,0,1,1)
                    uart.on("data") --uart 종료
                    --tmr.delay(1000000)
                    print("\r\n uart mode quit\r\n----------------\r\n")
                elseif code == "A" then -- A
                    gpio.write(pin_num,gpio.HIGH)
                elseif code == "a" then -- a
                    gpio.write(pin_num,gpio.LOW)
                else
                    uart.write(0, " code : " .. code .. "\n");
                end

            end)


        end,
        0 -- not run_lua
    )
end
