wifi.setmode(wifi.STATION)

cfg = {
  ip = "192.168.9.30",
  netmask = "255.255.255.0",
  gateway="192.168.9.177"
}
wifi.sta.setip(cfg)
wifi.sta.connect()
wifi.sta.config("GUNPOWER2","")

tmr.alarm(0,1000,1,function() 
    local ipaddr = wifi.sta.getip();
    --print(ipaddr);
    if ipaddr == nil then
      
     else
      print("connect ok");
      print(ipaddr);
      print("\n")
      tmr.stop(0)
     end
   end)

    