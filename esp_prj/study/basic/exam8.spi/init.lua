--[[
    GPIO12(6) - MISO
GPIO13 (7)- MOSI
GPIO14 (5)- CLK
GPIO15 (8) - CS (with an 10k pulldown to ground so the ESP8266 will boot up) 
--]]
pin=8 
--spi.setup(0, spi.MASTER, spi.CPOL_LOW, spi.CPHA_LOW, spi.DATABITS_8, 800);
spi.setup(1,spi.MASTER, spi.CPOL_HIGH, spi.CPHA_LOW,spi.DATABITS_8, 800);

gpio.mode(pin, gpio.OUTPUT)
gpio.write(pin, gpio.HIGH)
tmr.delay(30)
gpio.write(pin, gpio.LOW)
tmr.delay(30)
gpio.write(pin, gpio.HIGH)
tmr.delay(45)

function test()
    gpio.write(pin, gpio.HIGH)
    tmr.delay(30)
    gpio.write(pin, gpio.LOW)
    tmr.delay(30)
    gpio.write(pin, gpio.HIGH)
    tmr.delay(45)

    gpio.write(pin, gpio.LOW)
    spi.send(1,0x30)
    gpio.write(pin, gpio.HIGH)
    tmr.delay(100)

    gpio.write(pin, gpio.LOW)
    spi.send(1,0x41)
    spi.send(1,0x42)
    gpio.write(pin, gpio.HIGH)
    tmr.delay(100)

    gpio.write(pin, gpio.LOW)
    spi.send(1,0x80)
    read = spi.recv(1, 1)
    print(string.byte(read))
    gpio.write(pin, gpio.HIGH)
    tmr.delay(100) 

    print("complete")
end

--count=0
function test2()



--    gpio.write(pin, gpio.HIGH)
--    tmr.delay(30)
--    gpio.write(pin, gpio.LOW)
--    tmr.delay(30)
--    gpio.write(pin, gpio.HIGH)
--    tmr.delay(45)
--
    gpio.write(pin, gpio.LOW)
    spi.send(0,0x30)
    gpio.write(pin, gpio.HIGH)
    tmr.delay(100)
--
--    gpio.write(pin, gpio.LOW)
--    spi.send(0,0x30)
--    gpio.write(pin, gpio.HIGH)
--    tmr.delay(100)



--    gpio.write(pin, gpio.LOW)
--    spi.send(1,0x41)
--    spi.send(1,0x42)
--    gpio.write(pin, gpio.HIGH)
--    tmr.delay(100)

--    print("complete " .. count)
--    count = count+1
--    if(count > 5) then tmr.stop(0) end
end


--tmr.alarm(0,1000,tmr.ALARM_AUTO,test2)