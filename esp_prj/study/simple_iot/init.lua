

wifi.setmode(wifi.SOFTAP)

cfg={}
cfg.ssid="esptest"
--패스워드를 8자이상입력하지않으면 ssid 가 ESP8266_XXX이런식으로 설정됨
cfg.pwd="12345678"
wifi.ap.config(cfg)
--ap 모드에서 기본 주소는 192.168.4.1 이 됨

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


function startServer()

    srv=net.createServer(net.TCP)

    print("http server started")

    srv:listen(80,function(conn)
        conn:on("receive", function(conn, payload)

            _, _, method, path, vars = string.find(payload, "([A-Z]+) /([^?]*)%??(.*) HTTP")

            if path == "on" then
                gpio.write(pin_ap_led,gpio.HIGH)

            elseif path== "off" then
                gpio.write(pin_ap_led,gpio.LOW)
            else

            end

            --크로스도메인 허용하기위한 헤더
            conn:send("HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin:*\r\n\r\n")
            --결과 메씨지 보내기
            conn:send('{"result":"ok"}')
            conn:close()

        end)
    end)
end

startServer();
