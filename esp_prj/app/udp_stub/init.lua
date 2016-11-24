firm_version = '0.0.2'
dofile('wifi_setup.lua')
tmr.delay(1000000)
print('udp stub load')
dofile('udpstub.lua')
tmr.delay(1000000)
--print('start udp stub 1471')
start_udpStub(1471)
