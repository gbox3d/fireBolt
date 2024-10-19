import requests
import time
import os

from dotenv import load_dotenv

print('work directory :' + os.getcwd())
load_dotenv()

# Load environment variables for fan control settings
temperature_fan_on = int(os.getenv('TEMPERATURE_FAN_ON'))
temperature_fan_off = int(os.getenv('TEMPERATURE_FAN_OFF'))
api_port = os.getenv('GPIO_API_PORT')
api_url = os.getenv("API_URL")

# fan_pin_index is used to determine the PWM index controlling the fan , gpio 18
fan_pin_index = 2


loop_delay = 5

# Function to get the CPU temperature using the existing API
def get_cpu_temperature():
    response = requests.get(f"{api_url}:{api_port}/api/cpu_temp")
    if response.status_code == 200:
        data = response.json()
        return data.get("cpu_temp")
    else:
        raise Exception("Failed to get CPU temperature")

# Function to control the fan state (PWM control)
def control_fan(state):
    pwm_index = fan_pin_index  # PWM index for controlling the fan
    requests.get(f"{api_url}:{api_port}/api/gpio?pwm={pwm_index}&state={state}")

isFanActive = False

# Turn off fan initially
control_fan(state=0)
requests.get(f"{api_url}:{api_port}/api/gpio?led=1&state=0") # Turn off LED 1 ,  GND

# Main loop to monitor CPU temperature and control fan
while True:
    try:
        cpu_temperature = get_cpu_temperature()
        print(f"Current CPU Temperature: {cpu_temperature} °C")
    
        # Turn on fan if temperature exceeds threshold
        if not isFanActive:
            if cpu_temperature >= temperature_fan_on:
                control_fan(state=100)  # Set fan to 100% speed
                isFanActive = True
    
        # Turn off fan if temperature falls below threshold
        else:
            if cpu_temperature <= temperature_fan_off:
                control_fan(state=0)  # Turn off fan
                isFanActive = False
    
        # Delay between temperature checks
        time.sleep(loop_delay)
    except Exception as e:
        print(f"Error: {e}")