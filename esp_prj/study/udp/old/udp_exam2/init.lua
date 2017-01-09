

wifi.setmode(wifi.SOFTAP)

cfg={}
cfg.ssid="esptest"
--패스워드를 8자이상입력하지않으면 ssid 가 ESP8266_XXX이런식으로 설정됨
cfg.pwd="12345678"
wifi.ap.config(cfg)
--ap 모드에서 기본 주소는 192.168.4.1 이 됨

--s 는 server sub module 이다.
--서버형식으로 자신의 포트를 지정할수 있다.
--이후에 cu를 통해 send를 해도 여기서 정해준 포트로 데이터가 들어오게 된다.
--단 cu에서 지정한 receive 콜백은 무시된다. s로 지정한 콜백만 유효하다.

s=net.createServer(net.UDP)
s:on("receive",function(s,c) print('listen : ' ..c) end)
s:listen(5683)
