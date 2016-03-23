

tmr.alarm(0,1000,1,function()
    print('send data....')
    softuart.write(6,115200,'abc\n')
end)

