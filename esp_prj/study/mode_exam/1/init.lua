
wifi.setmode(wifi.STATIONAP)
--wifi.setmode(wifi.SOFTAP)
-- ap setup
cfg={}
cfg.ssid="esptest3"
--패스워드를 8자이상입력하지않으면 ssid 가 ESP8266_XXX이런식으로 설정됨
cfg.pwd="12345678"
wifi.ap.config(cfg)