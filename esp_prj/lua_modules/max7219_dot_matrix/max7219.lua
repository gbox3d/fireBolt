-- vcc 3.3 , din ->gpio13 ,cs->gpio15, clk->gpio14
--http://frightanic.com/iot/nodemcu-max7219-8x8-led-matrix-display/
--[[happy = {0x3C, 0x42, 0xA5, 0x81, 0xA5, 0x99, 0x42, 0x3C}
frown = {0x3C, 0x42, 0xA5, 0x81, 0xBD, 0x81, 0x42, 0x3C}
sad = {0x3C, 0x42, 0xA5, 0x81, 0x99, 0xA5, 0x42, 0x3C}
faces = {happy, frown, sad}
]]--
local max7219

do
local MAX7219_REG_NOOP        = 0x00
local MAX7219_REG_DECODEMODE  = 0x09
local MAX7219_REG_INTENSITY   = 0x0A
local MAX7219_REG_SCANLIMIT   = 0x0B
local MAX7219_REG_SHUTDOWN    = 0x0C
local MAX7219_REG_DISPLAYTEST = 0x0F

local sendByte = function (reg, data)
  spi.send(1,reg * 256 + data)
  tmr.delay(50)
end

--[[
    function displayFace(faceIndex)
  local face = faces[faceIndex]
  -- iterates over all 8 columns and sets their values
  for i=1,8 do
    sendByte(i,face[i])
  end
end
]]--

local setup = function ()
    spi.setup(1, spi.MASTER, spi.CPOL_LOW, spi.CPHA_LOW, 16, 8)

    sendByte (MAX7219_REG_SHUTDOWN, 1)
    sendByte (MAX7219_REG_SCANLIMIT, 7)
    sendByte (MAX7219_REG_DECODEMODE, 0x00)
    sendByte (MAX7219_REG_DISPLAYTEST, 0)
    sendByte (MAX7219_REG_INTENSITY, 9)
    sendByte (MAX7219_REG_SHUTDOWN, 1)
    --tmr.stop(0)
end

-- changes the face every two seconds cycling through the array of faces
--[[
function moody(i)
  faceIndex = (i % 3) + 1
  displayFace(faceIndex)
  tmr.alarm(0, 2000, 0, function()
    moody(faceIndex)
  end);
end

setup()
moody(0)
]]--
max7219 = {
  setup = setup,
  sendByte = sendByte
}

end

return max7219