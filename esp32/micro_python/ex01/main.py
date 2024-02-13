# main.py -- put your code here!
from machine import Pin
import time
import gc

led = Pin(5, Pin.OUT) # GPIO 2에 연결된 LED

gc.collect() # 가비지 컬렉션 실행
free_memory = gc.mem_free()
total_memory = gc.mem_alloc() + free_memory

print("Free memory:", free_memory, "bytes")
print("Total memory:", total_memory, "bytes")

while True:
    #print("LED ON")
    led.on()
    time.sleep(0.5)
    #print("LED OFF")
    led.off()
    time.sleep(0.5)
