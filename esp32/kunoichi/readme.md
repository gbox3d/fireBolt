# 쿠노이찌 펌웨어 업로더

쿠노이찌 펌웨어 업로더는 ESP32 보드에 업로드하는 프로그램입니다.  

## 인스톨러 만들기

```bash
pyinstaller --onefile --icon=icon.ico app.py # 도스 커멘드 창 모드로 실행

pyinstaller --onefile --windowed --icon=icon.ico app.py # 윈도우 모드로 실행

```

## 플랫폼별 빌드

```bash

pyinstaller win_app.spec # 윈도우용 실행파일 빌드
pyinstaller mac_app.spec # 맥용 실행파일 빌드

```
