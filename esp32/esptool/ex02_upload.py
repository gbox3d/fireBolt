import esptool
import sys

def upload_firmware(serial_port, firmware_path):
    """주어진 시리얼 포트에 펌웨어를 업로드합니다."""
    # esptool에서 사용할 기본 인자 설정
    esptool_args = [
        '--chip', 'esp32c3',  # 칩셋 유형
        '--port', serial_port,  # 시리얼 포트
        '--baud', '921600',  # 통신 속도
        'write_flash',  # 플래시 쓰기 명령
        '-z',  # 압축하여 플래시에 쓰기
        '0x10000', firmware_path  # 펌웨어 파일의 시작 주소와 경로
    ]

    # esptool 실행
    try:
        esptool.main(esptool_args)
        print(f"Firmware has been uploaded successfully to {serial_port}.")
    except Exception as e:
        print(f"Failed to upload firmware: {e}")

if __name__ == "__main__":
    # 사용자가 지정한 시리얼 포트와 펌웨어 파일 경로
    port = "COM8"
    firmware = r"C:\Users\user\Desktop\work\visionApp\fireBolt\esp32\pio\ex01_cmd\.pio\build\seeed_xiao_esp32c3\firmware.bin"
    
    # 펌웨어 업로드 함수 호출
    upload_firmware(port, firmware)
