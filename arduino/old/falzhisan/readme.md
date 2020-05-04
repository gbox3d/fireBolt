## falzhisan 
i2c 통신으로 아두이노 제어 하기 위한 프레임웍

디바이스 id : 27(0x1b)

## T1 
명령어 형식 
opcode = { reg , p1,p2…}

reg에는 명령어 코드가 들어간다. 명령어 코드들은 아래와 같다.  

### 1)  no operation
0x00 : nop

### 2) 모드 쓰기  
0x01 : pin mode reg

### 3) 상태 쓰기 (현제 포트값 관리)
0x02 : pin status reg (2바이트)  
p1 -> port bank (0 상위,1 하위 바이트)  
p2 -> value(1byte)  

예> 
0000 0000 0000 0100 => 13번 포트  HIGH  
bank 0 -> 0x00 
bank 1 -> 0x04

{0x02,0x01,0x04} => 13번 포트를 켜는 명령어(13번 포트를 high로 만든다.)   
```lua
sendData(27,{0x02,0x01,0x04})
````

모드 레지스터  디폴트 값 
xx11 0011 , 0011 11xx = {0x33, 0xcc};

쓰기용 포트 : 13,12,11,10,7,6,2,3  -> 11,10,6,3 은 pwm 효과용   
읽기용 포트  : 9,8,5,4

포트값 읽기
```lua
st = readData(0,27,2)
bnk0 = st:byte(1) -- bank 0
bnk1 = st:byte(2) -- bank 1

if bit.band(st:byte(1), 0x04) == 0 then -- 5번 포트 켜있는지 검사 
-- todo
end


```





## T2
JSON 으로 데이터를 주고 받는다.


