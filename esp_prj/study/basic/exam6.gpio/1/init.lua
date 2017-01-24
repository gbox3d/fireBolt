wifi.sta.autoconnect(0) --이전 세팅이 가끔끼어드는거 시러효!!
wifi.sta.disconnect() -- 완전히 끝내고 다시하고시퍼 ~~

function system_info()
    local remaining, used, total=file.fsinfo()
    print("\nFile system info:\nTotal : "..total.." Bytes\nUsed : "..used.." Bytes\nRemain: "..remaining.." Bytes\n")
    print("free head size :" .. node.heap())
end

gpiolookup = {[0]=3,[1]=10,[2]=4,[3]=9,[4]=2,[5]=1,[10]=12,[12]=6,[13]=7,[14]=5,[15]=8,[16]=0}


system_info()

gpio.mode(gpiolookup[16],gpio.OUTPUT)
gpio.mode(gpiolookup[5],gpio.OUTPUT)
gpio.mode(gpiolookup[4],gpio.OUTPUT)

gpio.mode(gpiolookup[14],gpio.INPUT)

gpio.write(gpiolookup[16],gpio.HIGH)
gpio.write(gpiolookup[4],gpio.HIGH)
gpio.write(gpiolookup[5],gpio.HIGH)

tmr.alarm(1,1000,0,function()
    gpio.write(gpiolookup[16],gpio.LOW)
    gpio.write(gpiolookup[4],gpio.LOW)
    gpio.write(gpiolookup[5],gpio.LOW)

end)

print("hello it is gpio sample")
print("gpio 14 :" .. gpio.read(gpiolookup[14]))




