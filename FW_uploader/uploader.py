import sys
import os
import serial.tools.list_ports
import esptool

CHIP_TYPE = 'esp32c3'
BAUD_RATE = '921600'

# 펌웨어 바이너리 파일 정보 (주소, 파일명)
FIRMWARE_FILES = [
    ('0x0', 'bootloader.bin'),
    ('0x8000', 'partitions.bin'),
    ('0x10000', 'firmware.bin'),
]

def get_script_path():
    if getattr(sys, 'frozen', False):
        return os.path.dirname(sys.executable)
    else:
        return os.path.dirname(os.path.abspath(__file__))

def find_com_ports():
    return serial.tools.list_ports.comports()

def list_firmware_dirs(base_path):
    # 실행 폴더 내에서 'firmware'로 시작하는 디렉터리만 필터
    dirs = [d for d in os.listdir(base_path)
            if os.path.isdir(os.path.join(base_path, d)) and d.startswith('firmware')]
    return dirs

def run_esptool(com_port, firmware_subdir):
    base_path = get_script_path()
    firmware_path = os.path.join(base_path, firmware_subdir)

    command_args = [
        '--chip', CHIP_TYPE,
        '--port', com_port,
        '--baud', BAUD_RATE,
        'write-flash',
        '-z'
    ]
    for addr, filename in FIRMWARE_FILES:
        file_path = os.path.join(firmware_path, filename)
        if not os.path.exists(file_path):
            print(f"\n[오류] 펌웨어 파일이 없습니다: {file_path}")
            return False
        command_args.extend([addr, file_path])

    print("\n" + "="*50)
    print(f"포트: {com_port} | 속도: {BAUD_RATE}")
    print(f"펌웨어 폴더: {firmware_subdir}")
    print("업로드를 시작합니다...")
    print("="*50)

    try:
        esptool.main(command_args)
    except Exception as e:
        print(f"\n[실패] 업로드 중 오류가 발생했습니다: {e}")
        return False

    print("\n[성공] 펌웨어 업로드가 완료되었습니다.")
    return True

if __name__ == "__main__":
    base_path = get_script_path()
    ports = find_com_ports()
    if not ports:
        print("COM 포트를 찾을 수 없습니다. 보드 연결을 확인하세요.")
        sys.exit(1)

    print("사용 가능한 COM 포트:")
    for i, p in enumerate(ports, 1):
        desc = p.description[:37] + '...' if len(p.description) > 40 else p.description
        print(f"  {i}: {p.device} ({desc})")

    try:
        choice = int(input("\n업로드할 포트 번호 (0=종료): "))
        if choice == 0:
            sys.exit(0)
        selected_port = ports[choice-1].device
    except (ValueError, IndexError):
        print("잘못된 입력입니다.")
        sys.exit(1)

    # firmware 폴더 선택
    fw_dirs = list_firmware_dirs(base_path)
    if not fw_dirs:
        print("펌웨어 폴더를 찾을 수 없습니다. 'firmware'로 시작하는 폴더를 만들어 주세요.")
        sys.exit(1)

    print("\n사용 가능한 펌웨어 폴더:")
    for i, d in enumerate(fw_dirs, 1):
        print(f"  {i}: {d}")

    try:
        fw_choice = int(input("\n업로드할 펌웨어 폴더 번호 (0=종료): "))
        if fw_choice == 0:
            sys.exit(0)
        selected_fw = fw_dirs[fw_choice-1]
    except (ValueError, IndexError):
        print("잘못된 입력입니다.")
        sys.exit(1)

    # 실제 업로드 실행
    run_esptool(selected_port, selected_fw)

    input("\n엔터를 누르면 종료합니다...")
