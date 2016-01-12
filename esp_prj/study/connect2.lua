
ht_ssid = {["GUNPOWER2"]=0,["GUNPOWER_WIFI"]=0}

gpio.mode(new_gpio[4],gpio.OUTPUT);
gpio.mode(new_gpio[5],gpio.OUTPUT);

function scan_wifi()
  
  
  local function listap(t)
        
    for ssid,v in pairs(t) do
      authmode, rssi, bssid, channel = string.match(v, "(%d),(-?%d+),(%x%x:%x%x:%x%x:%x%x:%x%x:%x%x),(%d+)")
        --print(ssid,authmode,rssi,bssid,channel)
  
      if ht_ssid[ssid] ~= nil then
        
          ht_ssid[ssid] = rssi;
        
        end      
      end    
  end
    
  wifi.sta.getap(listap)
    
  print("----------");
  
  for key,v in pairs(ht_ssid) do
    print(key,v);
  end
  
  gpio.write(new_gpio[4],gpio.HIGH)
  gpio.write(new_gpio[5],gpio.HIGH)
  
  if ht_ssid["GUNPOWER2"] > ht_ssid["GUNPOWER_WIFI"] then
    gpio.write(new_gpio[4],gpio.LOW)
  else
    gpio.write(new_gpio[5],gpio.LOW)
  end
  
end


tmr.alarm(0,3000,1,scan_wifi)
   