import os
import uvicorn
from fastapi import FastAPI, Query

from fastapi import FastAPI, Response
from fastapi.middleware.cors import CORSMiddleware
from pydantic import BaseModel
import uvicorn

import RPi.GPIO as GPIO
from dotenv import load_dotenv
import subprocess

print('work directory :' + os.getcwd())
load_dotenv()

# Load the GPIO API port from the environment variables
api_port = os.getenv('GPIO_API_PORT')

# Initialize FastAPI application
app = FastAPI()

# Setup CORS middleware to allow all origins (use caution for security reasons)
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],  # Allow all domains
    allow_credentials=True,
    allow_methods=["*"],  # Allow all HTTP methods (GET, POST, etc.)
    allow_headers=["*"],  # Allow all headers
)

# Define GPIO pins for PWM, LEDs, and buttons
pwmPins = [12, 13, 18, 19]
ledPins = [16, 17, 22, 23, 24, 25]
buttonPins = [26, 27]

# Dictionary to store PWM objects
pwmObjs = {}

def init_gpio():
    """
    Initialize GPIO pins for PWM, LEDs, and buttons.
    """
    global pwmObjs
    GPIO.setmode(GPIO.BCM)  # Use BCM GPIO pin numbering
    GPIO.setwarnings(False)  # Disable GPIO warnings
    
    # Initialize PWM pins
    for index, pin in enumerate(pwmPins):
        GPIO.setup(pin, GPIO.OUT)  # Set the pin as output
        _pwm = GPIO.PWM(pin, 50)  # Initialize PWM with 50Hz frequency
        _pwm.start(0)  # Start PWM with 0% duty cycle
        pwmObjs[index] = {
            "pwm": _pwm,
            "dutyCycle": 0  # Store initial duty cycle
        }
        
    # Initialize LED pins
    for pin in ledPins:
        GPIO.setup(pin, GPIO.OUT)  # Set the pin as output
        GPIO.output(pin, GPIO.LOW)  # Set initial state to LOW (off)
    
    # Initialize button pins
    for pin in buttonPins:
        GPIO.setup(pin, GPIO.IN, pull_up_down=GPIO.PUD_UP)  # Set the pin as input with pull-up resistor

# Initialize GPIO at the start
init_gpio()

__Version__ = "0.1.4"

@app.get("/")
def read_root():
    """
    Root endpoint to check if the server is running.
    """
    return {"message": f"gpio control server version : {__Version__}"}

@app.get("/api/gpu_temp")
def get_gpu_temp():
    """
    Endpoint to get the current GPU temperature.
    """
    try:
        # Use vcgencmd command to get the GPU temperature
        result = subprocess.run(["vcgencmd", "measure_temp"], capture_output=True, text=True)
        temp_str = result.stdout.strip()
        temp_value = float(temp_str.split("=")[1].replace("'C", ""))  # Extract and convert the temperature value
        return {"gpu_temp": temp_value}
    except Exception as e:
        # Return an error message if the command fails
        return {"error": f"Failed to get GPU temperature: {str(e)}"}

@app.get("/api/cpu_temp")
def get_cpu_temp():
    """
    Endpoint to get the current CPU temperature.
    """
    try:
        # Read the CPU temperature from the thermal zone file
        with open("/sys/class/thermal/thermal_zone0/temp", "r") as f:
            temp = int(f.read()) / 1000.0  # Convert millidegree Celsius to degree Celsius
            return {"cpu_temp": temp}
    except Exception as e:
        # Return an error message if reading the file fails
        return {"error": f"Failed to get CPU temperature: {str(e)}"}

"""_summary_
    
    /api/gpio?led=16&state=1
    /api/gpio?pwm=12&state=1
    /api/gpio?button=26

    Returns:
    {
        "message": "LED 16 set to 1"
    }
    {
        "message": "PWM 12 set to 1"
    }
    {
        "message": "Button 26 state is 1"
    }
"""

@app.get("/api/gpio")
def control_gpio(
    led: int = Query(None),
    pwm: int = Query(None),
    state: int = Query(None),
    button: int = Query(None)):
    """
    Endpoint to control GPIO pins for LEDs, PWM, and read button states.
    """
    # Control an LED pin
    if led is not None and 0 <= led < len(ledPins):  # Check if the led index is valid
        actual_pin = ledPins[led]  # Get the actual GPIO pin number from ledPins array
        GPIO.output(actual_pin, state)  # Set the GPIO output state (HIGH or LOW)
        return {"message": f"LED at index {led} (GPIO {actual_pin}) set to {state}"}
    
    # Control a PWM pin
    elif pwm is not None and pwm in pwmObjs:
        if state is not None:
            pwmObjs[pwm]["pwm"].ChangeDutyCycle(state)  # Change the duty cycle of the PWM
            pwmObjs[pwm]["dutyCycle"] = state  # Update the stored duty cycle
            return {"message": f"PWM at index {pwm} (GPIO {pwmPins[pwm]}) set to {state}"}
        else:
            current_duty_cycle = pwmObjs[pwm]["dutyCycle"]  # Get the current duty cycle
            return {
                "value" : current_duty_cycle,
                "message": f"Current PWM duty cycle at index {pwm} (GPIO {pwmPins[pwm]}) is {current_duty_cycle}"}
    
    # Read the state of a button pin
    elif button is not None and 0 <= button < len(buttonPins):
        actual_pin = buttonPins[button]  # Get the actual GPIO pin number from buttonPins array
        button_state = GPIO.input(actual_pin)  # Read the button state (HIGH or LOW)
        return {
            "value" : button_state,
            "message": f"Button at index {button} (GPIO {actual_pin}) state is {button_state}"}
    
    # Return an error if the request is invalid
    else:
        return {"error": "Invalid request"}

# Run the FastAPI application using Uvicorn
if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=int(api_port))