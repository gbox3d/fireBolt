import esptool
import os

# esptool 모듈의 설치 경로를 얻기
esptool_path = os.path.dirname(esptool.__file__)
print(f"esptool 설치 경로: {esptool_path}")

# stub_flasher 디렉토리의 경로를 구성
stub_flasher_path = os.path.join(esptool_path, 'targets', 'stub_flasher')
print(f"stub_flasher 경로: {stub_flasher_path}")

# 해당 경로가 실제로 존재하는지 확인
if os.path.exists(stub_flasher_path):
    print("stub_flasher 디렉토리를 찾았습니다.")
else:
    print("stub_flasher 디렉토리를 찾을 수 없습니다.")
