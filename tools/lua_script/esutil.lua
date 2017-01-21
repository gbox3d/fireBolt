new_gpio = {[0]=3,[1]=10,[2]=4,[3]=9,[4]=2,[5]=1,[10]=12,[12]=6,[13]=7,[14]=5,[15]=8,[16]=0}

function esutl_version()
  print("0.0.2")  
end
function esutl_file_list()
  local l = file.list()
  for k,v in pairs(l) do
    print("name :"..k..", size size:"..v)
  end
end
function esutl_file_remove(filename)
  file.remove(filename)
end
function esutl_wifi_list()
 
  local function listap(t)
      for ssid,v in pairs(t) do
        authmode, rssi, bssid, channel = string.match(v, "(%d),(-?%d+),(%x%x:%x%x:%x%x:%x%x:%x%x:%x%x),(%d+)")
        print(ssid,authmode,rssi,bssid,channel)
      end
    end
  wifi.sta.getap(listap)
 
end
function esutl_cat(filename)
  file.open(filename, "r")
  local strLine = nil
  while true do
      strLine = file.readline()
    if strLine == nil then
      break;
    else
      print(strLine)
    end
  end
  file.close()
 
end
function esutl_ledon(pin)
  gpio.mode(new_gpio[pin],gpio.OUTPUT)
  gpio.write(new_gpio[pin],gpio.HIGH)
  
end