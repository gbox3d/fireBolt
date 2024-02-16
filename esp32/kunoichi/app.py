import tkinter as tk
from tkinter import ttk
from tkinter import filedialog
import serial.tools.list_ports
from tkinter import messagebox  # 메시지 박스를 사용하기 위해 임포트

import os

import esptool

import sys

class FauxStream(object):
    """콘솔이 없는 환경에서 stdout/stderr의 메소드를 대체하기 위한 가짜 스트림 클래스."""
    def write(self, data):
        pass  # 실제로는 데이터를 어디에도 쓰지 않음

    def flush(self):
        pass  # flush 호출을 무시함

# 콘솔 창이 없는 경우에 대비하여 sys.stdout와 sys.stderr를 대체
if not sys.stdout:
    sys.stdout = FauxStream()
if not sys.stderr:
    sys.stderr = FauxStream()


# 사용 가능한 칩셋 목록
chipsets = [
    "esp32", "esp32s2", "esp32s3", "esp32c3", "esp32c6", "esp8266"
]

# 사용 가능한 Baud rates
baud_rates = ["115200", "230400", "460800", "921600"]

class ESPUploaderUI(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("UFW Tool v1.0 , Copyright 2024, (c)BlueSquare")
        self.iconbitmap("./res/icon.ico")
    
        self.geometry("640x480")

        # 시리얼 포트 선택
        tk.Label(self, text="Serial Port:").pack(anchor="w")
        self.serial_port_var = tk.StringVar()
        self.serial_port_combobox = ttk.Combobox(self, textvariable=self.serial_port_var)
        self.serial_port_combobox["values"] = [port.device for port in serial.tools.list_ports.comports()]
        self.serial_port_combobox.pack(anchor="w", fill="x", expand=True)
        
        # 시리얼 포트 새로고침 버튼
        self.refresh_button = tk.Button(self, text="Refresh Ports", command=self.refresh_serial_ports)
        self.refresh_button.pack(anchor="w", pady=5)
        

        # 칩셋 선택
        tk.Label(self, text="Chipset:").pack(anchor="w")
        self.chipset_var = tk.StringVar(value=chipsets[0])
        self.chipset_combobox = ttk.Combobox(self, textvariable=self.chipset_var)
        self.chipset_combobox["values"] = chipsets
        self.chipset_combobox.pack(anchor="w", fill="x", expand=True)

        # 펌웨어 경로 입력
        tk.Label(self, text="Firmware Path:").pack(anchor="w")
        self.firmware_path_var = tk.StringVar()
        tk.Entry(self, textvariable=self.firmware_path_var).pack(anchor="w", fill="x", expand=True)
        tk.Button(self, text="Browse...", command=self.browse_firmware_path).pack(anchor="w")

        # Baud rate 선택
        tk.Label(self, text="Baud Rate:").pack(anchor="w")
        self.baud_rate_var = tk.StringVar(value=baud_rates[3])
        self.baud_rate_combobox = ttk.Combobox(self, textvariable=self.baud_rate_var)
        self.baud_rate_combobox["values"] = baud_rates
        self.baud_rate_combobox.pack(anchor="w", fill="x", expand=True)

        # 업로드 버튼
        self.upload_button = tk.Button(self, text="Upload Firmware", command=self.upload_firmware)
        self.upload_button.pack(anchor="w", pady=10)
        
        # 업로드 버튼 아래에 진행 상황 메시지를 표시할 Label 추가
        self.status_message = tk.StringVar()
        self.status_label = tk.Label(self, textvariable=self.status_message)
        self.status_label.pack(anchor="w", fill="x", expand=True)
        
    def refresh_serial_ports(self):
        """시리얼 포트 리스트를 새로고침하는 함수"""
        available_ports = [port.device for port in serial.tools.list_ports.comports()]
        self.serial_port_combobox["values"] = available_ports
        if available_ports:
            self.serial_port_combobox.set(available_ports[-1])
        else:
            self.serial_port_combobox.set('')  # 사용 가능한 포트가 없을 때
        
    def update_status(self, message):
        """진행 상황 메시지를 업데이트하는 함수"""
        self.status_message.set(message)
        self.update_idletasks()  # UI를 즉시 업데이트

    def browse_firmware_path(self):
        """파일 선택 대화상자를 열어 펌웨어 파일 경로를 선택합니다."""
        initial_dir = os.getcwd()  # 현재 작업 디렉토리를 기본 디렉토리로 설정
        # .bin 확장자를 가진 파일만 표시
        file_path = filedialog.askopenfilename(
            initialdir=initial_dir, 
            filetypes=[("Binary files", "*.bin")],  # 확장자 필터 설정
            title="Select Firmware File"
        )
        self.firmware_path_var.set(file_path)


    from tkinter import messagebox  # 메시지 박스를 사용하기 위해 임포트

    def upload_firmware(self):
        """선택된 설정으로 펌웨어를 업로드합니다."""
        serial_port = self.serial_port_var.get()
        chipset = self.chipset_var.get()
        firmware_path = self.firmware_path_var.get()
        baud_rate = self.baud_rate_var.get()

        # 빠진 항목이 있는지 확인
        missing_items = []
        if not serial_port:
            missing_items.append("Serial Port")
        if not chipset:
            missing_items.append("Chipset")
        if not firmware_path:
            missing_items.append("Firmware Path")
        if not baud_rate:
            missing_items.append("Baud Rate")

        # 빠진 항목이 있으면 경고 메시지를 표시하고 함수를 종료
        if missing_items:
            messagebox.showwarning("Missing Items", "Please select: " + ", ".join(missing_items))
            return
        
        esptool_args = [
            '--chip', chipset,  # 칩셋 유형
            '--port', serial_port,  # 시리얼 포트
            '--baud', baud_rate,  # 통신 속도
            'write_flash',  # 플래시 쓰기 명령
            '-z',  # 압축하여 플래시에 쓰기
            '0x10000', firmware_path  # 펌웨어 파일의 시작 주소와 경로
        ]

        # 모든 항목이 제공되었을 경우, 업로드 로직 실행
        print(f"Uploading {esptool_args}")
        # 여기서 esptool을 사용하여 펌웨어 업로드 로직을 추가하세요.
        # 예를 들어:
        # esptool.main(['--port', serial_port, '--baud', baud_rate, '--chip', chipset, 'write_flash', '-z', '0x1000', firmware_path])
        
        self.update_status("Uploading firmware...")
        
        # esptool 실행
        try:
            esptool.main(esptool_args)
            _msg = f"Firmware has been uploaded successfully to {serial_port}."
            
        except Exception as e:
            _msg = f"Failed to upload firmware: {e}"
            
        print(_msg)
        self.update_status(_msg)
        
if __name__ == "__main__":
    app = ESPUploaderUI()
    app.mainloop()
