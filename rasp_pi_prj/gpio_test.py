#%%
from gpiozero import PWMOutputDevice
from time import sleep
# GPIO 핀 번호 설정 (PWM을 지원하는 핀)
fan = PWMOutputDevice(18)

#%%

_value = 0

while True:
    fan.value = _value
    sleep(0.1)
    _value += 0.01
    if _value > 1:
        _value = 0
    print(f"fan value : {_value}")
    
    
    
