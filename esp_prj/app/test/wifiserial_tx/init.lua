print("es8266 for gunpower wifi2uart 0,0.1")

new_gpio = {[0]=3,[1]=10,[2]=4,[3]=9,[4]=2,[5]=1,[10]=12,[12]=6,[13]=7,[14]=5,[15]=8,[16]=0}
pin_ap_led = new_gpio[4];
pin_connection_led = new_gpio[5];
softuart_tx_pin = new_gpio[12];
pin_switch_sel_mode = new_gpio[14];

gpio.mode(pin_ap_led,gpio.OUTPUT)
gpio.mode(pin_connection_led,gpio.OUTPUT)
gpio.mode(softuart_tx_pin,gpio.OUTPUT)
gpio.mode(pin_switch_sel_mode,gpio.INPUT)

gpio.write(pin_ap_led,gpio.LOW)
gpio.write(pin_connection_led,gpio.LOW)

dofile('wifi_config.lua')

tmr.alarm(0,1000,1,function()
    local ipaddr = wifi.sta.getip();
    if ipaddr == nil then
        print('-wait connecting..')
    else
        print("-connect ok " .. ipaddr );
        tmr.stop(0)

        if gpio.read(pin_switch_sel_mode) == 0 then

            gpio.write(pin_ap_led,gpio.HIGH)

            print("-start app...\n")
            tmr.delay(500000) --100ms
            dofile('wifi2uart.lua')
            --wifi2SerialStart()
        else
            gpio.write(pin_connection_led,gpio.HIGH)
            print("-start setup mode\n")
            dofile('apisrv.lua')
            --ApiServer_start()
        end


    end
end)

--dofile('a.lua')
--dofile('esutil.lc')
--dofile('apisrv.lc')
--dofile('app.lc')

--file.open('init.lua')
--file.writeline([[print("chun setup 0,0.1")]])
--file.writeline([[dofile('wifi_config.lc')]])
--file.writeline([[dofile('esutil.lc')]])
--file.writeline([[dofile('system.lc')]])
--file.close()
--collectgarbage("collect")
--dofile('cp_all.lua')


