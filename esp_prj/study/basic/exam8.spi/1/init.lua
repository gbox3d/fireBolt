

--[[ ESP8266 connected to a 74hc595 port expander
By using SPI.

This DEMO sends serial data to one 74hc595, can easily be modified to use several.
It counts up from 0 to 255 and send it through SPI to the hc595.

SPI Connection:
GPIO12 (6) to MISO (NOT USED)
GPIO13 (7) to MOSI (hc595 pin 14 (DS))
GPIO14 (5) to CLK (hc595 pin 11 (SH))
GPIO15 (8) to CS (hc595 pin 12 (SS))

Master reset (/MR) i connect directly to VCC no need for resetting here unless you need it.
Output Enable (/OE) i connect directly to GND as i dont need high impedance mode on outputs.
]]
pin=8
spi.setup(1, spi.MASTER, spi.CPOL_HIGH, spi.CPHA_LOW, spi.DATABITS_8, 800);

--pin=12
--spi.setup(0, spi.MASTER, spi.CPOL_HIGH, spi.CPHA_LOW, spi.DATABITS_8, 800);
gpio.mode(pin, gpio.OUTPUT)

for j=1,255,1 do
    gpio.write(pin, gpio.LOW) --chip select avtive low
    tmr.delay(20)
    spi.send(1,j)    --send the byte to hc595
    gpio.write(pin, gpio.HIGH)
    tmr.delay(100000)   --delay so we can see whats going on

    print(j)
    tmr.wdclr() --Since we are delaying wery much we need to tell the system it has not crashed.

end
-- See more at: http://www.esp8266.com/viewtopic.php?p=12734#sthash.MnarzRgq.dpuf