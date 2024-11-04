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
        """Initialize the thread with port, folder path, and YAML config file."""
        super().__init__()
        self.port = port
        self.folder_path = folder_path
        self.yaml_file = os.path.join(self.folder_path, yaml_file)
        self.upload_speed = 921600
        self.flash_files = []
        self.chip = None  # 칩 유형을 저장할 변수
        self.load_yaml()

    def load_yaml(self):
        """Load flash configuration from YAML file."""
        try:
            with open(self.yaml_file, 'r') as file:
                data = yaml.safe_load(file)

                # 지원하는 칩 유형 확인 및 설정
                supported_chips = ['esp32', 'esp8266']
                env_chip = data.get('env')
                if env_chip not in supported_chips:
                    self.output_signal.emit("Error: Unsupported environment in cmd.yaml.")
                    self.finished_signal.emit(False)
                    return
                else:
                    self.chip = env_chip  # 칩 유형 설정

                # 기본 업로드 속도 설정
                self.upload_speed = data.get('upload_speed', 460800 if self.chip == 'esp8266' else 921600)
                self.flash_files = data.get('write_flash', [])
                if not self.flash_files:
                    self.output_signal.emit("Error: No flash files specified in cmd.yaml.")
                    self.finished_signal.emit(False)
        except FileNotFoundError:
            self.output_signal.emit("Error: cmd.yaml file not found.")
            self.finished_signal.emit(False)
        except yaml.YAMLError as e:
            self.output_signal.emit(f"Error parsing YAML: {e}")
            self.finished_signal.emit(False)

    def build_command(self):
        """Build the command list for esptool based on YAML configuration."""
        command = [
            "--chip", self.chip,
            "--port", self.port,
            "--baud", str(self.upload_speed),
            "write_flash"
        ]
        for address_file_pair in self.flash_files:
            for address, filename in address_file_pair.items():
                file_path = os.path.join(self.folder_path, filename)
                if not os.path.exists(file_path):
                    self.output_signal.emit(f"Error: {filename} not found in the specified folder.")
                    self.finished_signal.emit(False)
                    return None
                command.extend([str(address), file_path])
        return command

    def run(self):
        """Run the firmware upload process using esptool script."""
        if self.flash_files:
            try:
                command = self.build_command()
                if command is None:
                    return

                self.output_signal.emit(f"Executing command: {' '.join(command)}")
                esptool.main(command)
                self.output_signal.emit("Firmware upload completed successfully.")
                self.finished_signal.emit(True)

            except Exception as e:
                self.output_signal.emit(f"Error during firmware upload: {str(e)}")
                self.finished_signal.emit(False)
        else:
            self.output_signal.emit("No flash files found in cmd.yaml.")
            self.finished_signal.emit(False)
