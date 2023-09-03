# esp32 예제  

etc.hpp 는 아두이노 라이브러리 폴더에 복사해 놓아야 합니다.  

윈도우의 경우 **문서/arduino/libraries/mylibs/** 폴더에 복사해 놓으면 됩니다.  

## example list

### ex00 : Blink
내장된 led를 깜박이면서 , 디지털핀들을 테스트 하는 예제입니다.  

### ex01 : 
esp32에서 tonkey 라이브러리를 사용하여 tonkey를 제어하는 예제입니다.  

### ex02 : 
esp32에서 BluetoothSerial 라이브러리를 사용하는 예제입니다. 

### ex03 : BLE Serial Battery Check
베터리 잔량을 측정하는 예제입니다.  
블루투스 씨리얼사용하고 연결된 측에서 'a'를 보내면 현재 베터리 잔량을 보내줍니다.  

### ex04 : ble central peripheral
central , preiperhal 을 기본예제입니다.  
esp32 는 peripheral 로 동작하고 파이썬 예제는 central 로 동작합니다.  
맥주소로 접속하여 환영메씨지를 받아서 출력합니다.  

## ex05 : ble read write
ble 읽고 쓰기 예제입니다. 
에코 요청을하고 응답받습니다. led 제어 합니다.    

## ex06 : manage config file by Ble
ble 통신을 통해 config 파일을 관리 하는 예제 입니다.  
preipheral : esp32  
centeral : python, android  

## ex08 :  SPIFFS
spiffs 파일시스템을 사용하는 예제 입니다. 포멧이 필요한경우 E (52) SPIFFS: mount failed, -10025 에러가 발생합니다. 이때는 그냥 기다리면 자동으로 포멧됩니다.   
18650 베터리 일체형 모듈의 경우 보드 선택은 ESP32 Dev Module, Esp32 Wrover Module 같은 모듈을 선택하면 됩니다.   
patrition sheme 는 default 4MB with spiffs 을 선택하면 됩니다.   

## ex20 : imu sensor 
MPU9250_asukiaaa 라이브러리를 사용하여 imu 센서를 읽어오는 예제입니다.  
imu 센서는 i2c 로 연결되어 있습니다.  
3축가속도( x,y,z ) 를 가지고 x,y 각도를 구하는 예제 입니다.  

## ex21 : check imu connection
imu 센서와 연결상태 확인하기 예제  

## ex22 : MPU9250 9축 센서
MPU9250 라이브러리를 사용하여 9축 imu 센서를 읽어오는 예제입니다.   
mpu6500 + AK8963 센서가 내장된 모듈(9축)이 필요합니다.  

## 참고자료

tonkey : https://github.com/gbox3d/tonkey  



## 라이브러리 설치
위에서 받은 tonkey 라이브러리를 다음과 같이 추가 합니다.  
Arduino IDE에서 라이브러리 추가: Arduino IDE에서 "스케치" > "라이브러리 포함하기" > "zip 라이브러리 추가"를 선택합니다.  

