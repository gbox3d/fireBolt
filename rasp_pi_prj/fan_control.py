import requests
import time
import os


from dotenv import load_dotenv

print('work directory :' + os.getcwd())
load_dotenv()


temperature_fan_on = int(os.getenv('TEMPERATURE_FAN_ON'))
temperature_fan_off = int(os.getenv('TEMPERATURE_FAN_OFF'))
# fan_pin_index = int(os.getenv('FAN_PIN_INDEX'))
fan_pin_index = 2

requests.get(f"http://localhost:8001/api/gpio?led=1&state=0") # make GND 

loop_delay = 5
# fan_speed = 20

def get_cpu_temperature():
    with open("/sys/class/thermal/thermal_zone0/temp", "r") as temp_file:
        cpu_temp = int(temp_file.read()) / 1000
    return cpu_temp

def control_fan(state):
    pwm_index = fan_pin_index  # 팬을 제어하는 PWM 인덱스, 실제 환경에 맞게 조정해야 함
    requests.get(f"http://localhost:8001/api/gpio?pwm={pwm_index}&state={state}")
    
isFanActive = False

while True:
    cpu_temperature = get_cpu_temperature()
    print(f"Current CPU Temperature: {cpu_temperature}°C")
    
    
    
    if isFanActive == False :
        isFanActive = True
        if cpu_temperature >= temperature_fan_on :
            control_fan(state=100)
            
    else :
        if cpu_temperature <= temperature_fan_off :
            control_fan(state=0)  # 팬을 끔
            isFanActive = False
            
            
            
    
        
    
    # if cpu_temperature >= temperature_fan_on + 20:
    #     control_fan(state=100)  # 팬을 최대 속도로 켬
    # elif cpu_temperature >= temperature_fan_on + 10:
    #     control_fan(state=50)
    # elif cpu_temperature >= temperature_fan_on:
    #     control_fan(state=20)
    # elif cpu_temperature <= temperature_fan_off:
    #     print("Turning fan OFF")
    #     control_fan(state=0)  # 팬을 끔
    
    time.sleep(loop_delay)
