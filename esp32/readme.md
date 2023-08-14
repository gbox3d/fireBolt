# esp32 예제  

etc.hpp 는 아두이노 라이브러리 폴더에 복사해 놓아야 합니다.  

윈도우의 경우 **문서/arduino/libraries/mylibs/** 폴더에 복사해 놓으면 됩니다.  

## example list

ex00 : 내장된 led를 깜박이면서 , 디지털핀들을 테스트 하는 예제입니다.

ex01 : esp32에서 tonkey 라이브러리를 사용하여 tonkey를 제어하는 예제입니다.  

ex02 : esp32에서 BluetoothSerial 라이브러리를 사용하는 예제입니다. 

ex03 : 베터리 잔량을 측정하는 예제입니다.  
블루투스 씨리얼사용하고 연결된 측에서 'a'를 보내면 현재 베터리 잔량을 보내줍니다.  

ex04 : central , preiperhal 을 기본예제입니다. esp32 는 peripheral 로 동작하고 파이썬 예제는 central 로 동작합니다.  
맥주소로 접속하여 환영메씨지를 받아서 출력합니다.  


## 참고자료

tonkey : https://github.com/gbox3d/tonkey  



## 라이브러리 설치
위에서 받은 tonkey 라이브러리를 다음과 같이 추가 합니다.  
Arduino IDE에서 라이브러리 추가: Arduino IDE에서 "스케치" > "라이브러리 포함하기" > "zip 라이브러리 추가"를 선택합니다.  

