# filename : uploadThread.py
# author : gbox3d & GPT4-o

import sys
import os
import esptool
import yaml
from PySide6.QtCore import QThread, Signal

class FirmwareUploadThread(QThread):
    output_signal = Signal(str)
    finished_signal = Signal(bool)

    def __init__(self, port, folder_path, yaml_file="cmd.yaml"):
        super().__init__()
        self.port = port
        self.folder_path = folder_path
        self.yaml_file = os.path.join(self.folder_path, yaml_file)
        self.upload_speed = 921600
        self.flash_files = []

        # YAML 파일 읽기
        self.load_yaml()

    def load_yaml(self):
        """Load the YAML configuration file."""
        try:
            with open(self.yaml_file, 'r') as file:
                data = yaml.safe_load(file)

                # 환경설정과 업로드 속도 설정
                if 'env' not in data or data['env'] != 'esp32':
                    self.output_signal.emit("Error: Unsupported environment in cmd.yaml.")
                    self.finished_signal.emit(False)
                    return

                self.upload_speed = data.get('upload_speed', 921600)  # 기본값 921600
                self.flash_files = data.get('write_flash', [])

                if not self.flash_files:
                    self.output_signal.emit("Error: No flash files specified in cmd.yaml.")
                    self.finished_signal.emit(False)
                    return
        except FileNotFoundError:
            self.output_signal.emit("Error: cmd.yaml file not found.")
            self.finished_signal.emit(False)
        except yaml.YAMLError as e:
            self.output_signal.emit(f"Error parsing YAML: {e}")
            self.finished_signal.emit(False)

    def run(self):
        """Run the firmware upload process using esptool script."""
        if self.flash_files:
            try:
                # esptool 스크립트 방식으로 플래시 설정
                command = [
                    "--chip", "esp32",
                    "--port", self.port,
                    "--baud", str(self.upload_speed),
                    "write_flash"
                ]

                # cmd.yaml에서 로드한 파일과 주소 추가
                for address_file_pair in self.flash_files:
                    for address, filename in address_file_pair.items():
                        file_path = os.path.join(self.folder_path, filename)
                        if not os.path.exists(file_path):
                            self.output_signal.emit(f"Error: {filename} not found in the specified folder.")
                            self.finished_signal.emit(False)
                            return
                        command.append(str(address))
                        command.append(file_path)

                # esptool의 파이썬 스크립트 호출
                esptool.main(command)
                self.output_signal.emit("Firmware upload completed successfully.")
                self.finished_signal.emit(True)

            except Exception as e:
                self.output_signal.emit(f"Error during firmware upload: {str(e)}")
                self.finished_signal.emit(False)
        else:
            self.output_signal.emit("No flash files found in cmd.yaml.")
            self.finished_signal.emit(False)
