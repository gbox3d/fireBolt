import os
import uvicorn
from fastapi import FastAPI, Query

from fastapi import FastAPI,Response
from fastapi.middleware.cors import CORSMiddleware
from pydantic import BaseModel
import uvicorn


import RPi.GPIO as GPIO
from dotenv import load_dotenv

print('work directory :' + os.getcwd())
load_dotenv()

api_port = os.getenv('GPIO_API_PORT')



app = FastAPI()

#setup CORS
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],  # 모든 도메인 허용, 보안에 주의하세요.
    allow_credentials=True,
    allow_methods=["*"],  # 모든 메소드 허용 (GET, POST 등)
    allow_headers=["*"],  # 모든 헤더 허용
)

pwmPins = [12, 13, 18, 19]
ledPins = [16, 17, 22, 23, 24, 25]
buttonPins = [26, 27]

pwmObjs = {}

def init_gpio():
    global pwmObjs, pwmDutyCycles
    GPIO.setmode(GPIO.BCM)
    GPIO.setwarnings(False)
    for index, pin in enumerate(pwmPins):
        GPIO.setup(pin, GPIO.OUT)
        _pwm = GPIO.PWM(pin, 50)  # 50Hz의 주파수로 PWM 설정
        _pwm.start(0)  # PWM 시작 (0% 듀티 사이클)
        # pwmObjs.append(_pwm)
        # pwmDutyCycles[index] = 0  # 초기 듀티 사이클을 0으로 설정
        pwmObjs[index] = {
            "pwm": _pwm,
            "dutyCycle": 0
        }
        
    for pin in ledPins:
        GPIO.setup(pin, GPIO.OUT)
        GPIO.output(pin, GPIO.LOW)
    for pin in buttonPins:
        GPIO.setup(pin, GPIO.IN, pull_up_down=GPIO.PUD_UP)

# Initialize GPIO at the start
init_gpio()

__Version__ = "0.1.0"

@app.get("/")
def read_root():
    return {"message": f"gpio control server version : {__Version__}"}


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
    if led is not None and 0 <= led < len(ledPins):  # led가 유효한 인덱스 범위 내에 있는지 확인
        actual_pin = ledPins[led]  # 실제 GPIO 핀 번호를 ledPins 배열에서 가져옴
        GPIO.output(actual_pin, state)  # 실제 핀 번호를 사용하여 GPIO 출력 설정
        return {"message": f"LED at index {led} (GPIO {actual_pin}) set to {state}"}
    
    elif pwm is not None and pwm in pwmObjs:
        if state is not None:
            pwmObjs[pwm]["pwm"].ChangeDutyCycle(state)
            pwmObjs[pwm]["dutyCycle"] = state
            return {"message": f"PWM at index {pwm} (GPIO {pwmPins[pwm]}) set to {state}"}
        else:
            current_duty_cycle = pwmObjs[pwm]["dutyCycle"]
            return {
                "value" : current_duty_cycle,
                "message": f"Current PWM duty cycle at index {pwm} (GPIO {pwmPins[pwm]}) is {current_duty_cycle}"}
    
    elif button is not None and 0 <= button < len(buttonPins):
        actual_pin = buttonPins[button]
        button_state = GPIO.input(actual_pin)
        return {
            "value" : button_state,
            "message": f"Button at index {button} (GPIO {actual_pin}) state is {button_state}"}
    else:
        return {"error": "Invalid request"}


if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=int(api_port))
