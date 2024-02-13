# 마이크로 파이썬(마파)설치

https://micropython.org/download/esp32/ 여기서 인터프리터 펌웨어를 다운받는다.

esptools 설치하거나 Thonny(https://thonny.org/)를 설치하여 펌웨어를 업로드한다.

## esptools 로 펌웨어 업로드

esptools 설치는 아래와 같이 한다.  
```
pip install esptool
```

펌웨어 업로드
```
esptool.py --chip esp32 --port COM3 --baud 460800 write_flash -z 0x1000 esp32-idf3-20200902-v1.13.bin
```

## Thonny 로 펌웨어 업로드

https://thonny.org/ 에서 다운받아서 설치한다.  


thonny는 라즈베리에 기본으로 설치되어있는 파이썬 에디터이다.  
마이크로 파이썬을 사용하기 위해서는 thonny를 다음과 같이 설정해야한다.  
thnnoy -> 도구->옵션 -> 인터프리터 -> install & update micropython 으로 업로드 대화창이 열리면   
(참고 : https://kgu0724.tistory.com/272)

일반적인 보드의 경우 대부분은 esp32 를 선택하고  
variant 는 'espreeif ESP32' 을 선택한다.  

# 파이썬 스크립트 업로드 와 실행

## thony
main.py 또는 boot.py 를 업로드하면 재부팅후 자동으로 실행된다.  
호스트피씨에서 파이썬 코드를 작성하고 thonny에서 save as(crtl+sht+s) 로  main.py 또는 boot.py 로 저장하면 된다.  
대화창으로 호스트피씨에 저장할지 타겟디바이스에 저장할지 물어보면 타겟디바이스에 저장한다.

## ampy
ampy 를 이용하여 파이썬 스크립트를 업로드 할수있다.
```
ampy -p COM3 put main.py
```

그외 ampy 명령어는 다음과 같다.
``` 
ampy -p COM3 ls # 파일목록
ampy -p COM3 get main.py # 호스트pc로 파일가져오기
ampy -p COM3 rm main.py # 파일삭제
ampy -p COM3 run main.py # 파일실행
``` 


# ble 개념정리

preipheral : advertise 를 통해 연결을 요청하는 디바이스 서버 역할을 한다.  
central : advertise 를 통해 연결을 요청하는 디바이스 클라이언트 역할을 한다.  

하나의 central 에 여러개의 peripheral 이 연결될 수 있다.  










