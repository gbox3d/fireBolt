# Firmware Uploader

## setup
```
pip install pyserial esptool
```

## build
``` 
pip install pyinstaller
pyinstaller --onefile --name FW_uploader FW_uploader.py

pyinstaller --onefile --name FW_uploader   --add-data ".\.venv\Lib\site-packages/esptool/targets/stub_flasher;esptool/targets/stub_flasher"  uploader.py

```
