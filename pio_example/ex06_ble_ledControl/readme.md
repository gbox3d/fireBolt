# ble led controller

## Description
This is a simple BLE LED controller that can be used to control an LED over BLE. The controller is implemented using the ESP32 microcontroller and the Arduino framework. The controller can be used to turn the LED on and off, as well as to change the color of the LED.

devoce name start with `ESP32_BLE'  



## uuid

The UUIDs for the service and characteristic are defined in the `ble_led_controller.ino` file. The UUIDs are as follows:
```cpp
#define SERVICE_UUID "02cb14bf-42e0-41bd-9d6d-68db02057e42"
#define CHARACTERISTIC_UUID "fef6efb5-fbf8-4d0a-a69f-57049733a87a"
```

## Commands

**setup led pins** : Setup the pins for the LED  

```txt
config setA ledpin [2,12,15,4,13,14,16,17,18] 
config save
reboot
```

**on** : Turn the LED on   
example:    
`on 1` : turn led index 1 on  
`on 3 6 9` : turn led index 3, 6, 9 on  
`on -1` : all leds on  

**off** : Turn the LED off  
example:  
`off 1` : turn led index 1 off  
`off -1` : all leds off  


