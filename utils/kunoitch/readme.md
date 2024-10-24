# kunoiotchi

## build ui
```bash

pyside6-designer

pyside6-uic mainWindow.ui -o mainWindow.py
pyside6-uic optionDlg.ui -o optionDlg.py  
pyside6-uic aboutDlg.ui -o aboutDlg.py

```

## make exe


```bash
pyinstaller --onefile --windowed app.py
pyinstaller --onefile app.py
pyinstaller --onefile --icon=icon.ico app.py --add-data ".venv\Lib\site-packages\esptool\targets\stub_flasher\*.json;esptool/targets/stub_flasher"
```

**주의 사항**  
- 업로더에 필요한 esptool의 stub_flasher폴더는 esptool의 실행파일이 실행될때 필요한 파일들이 있음  
- pyinstaller로 exe파일을 만들때는 --add-data 옵션을 사용하여 esptool의 stub_flasher폴더를 추가해야함   
- 예) --add-data ".venv\Lib\site-packages\esptool\targets\stub_flasher\*.json;esptool/targets/stub_flasher" 
  


## UPLOAD
```bash
esptool.py --chip esp32 --port COM5 --baud 921600 write_flash 0x1000 bootloader.bin 0x8000 partitions.bin 0x10000 firmware.bin

```

## 기타

- **ttgo-t1-battery 모듈 업로드시 주의사항**  
업로드 모드로 전환 후 업로드 진행  
boot버튼을 누른 상태로 EN(리셋)버튼을 누르면 업로드 모드로 전환됨  
일단 업로드가 진행되기 시작하면 업로드 버튼은 떼도 됨  
업로드버트을 누르고 업로드 대기상태에서 업로드 모드로 수동전환시켜야함(윈도우 10)  

