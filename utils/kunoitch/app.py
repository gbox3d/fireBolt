#filename : app.py
#author : gbox3d & GPT4-o , 2024.09.09

import os
import sys
# import subprocess
import json  # JSON 데이터 처리를 위해 추가
from PySide6.QtWidgets import QApplication, QMainWindow, QMessageBox, QFileDialog
from PySide6.QtCore import Slot
from serial_handler import SerialThread, scan_serial_ports

# UI 파일 임포트
from UI.mainWindow import Ui_MainWindow
from optionDialog import OptionDialog

# 업로드 스레드 클래스 임포트
from uploadThread import FirmwareUploadThread  # uploadThread.py에서 클래스 임포트

__VERSION__ = "0.8.0"

class MainWindow(QMainWindow, Ui_MainWindow):
    def __init__(self):
        super().__init__()
        self.setupUi(self)
        
        #set Title version
        self.setWindowTitle(f"Qunoitch v{__VERSION__}")
        
        # home menu
        self.actionAbout.triggered.connect(self.show_about_dialog)
        
        
        # Serial Terminal UI
        self.serial_thread = None
        self.selected_baudrate = 115200  # 기본 baudrate 설정

        self.actionScan.triggered.connect(self.scan_serial_ports)
        self.actionOption.triggered.connect(self.open_option_dialog)
        
        self.btnConnect.clicked.connect(self.connect_serial_port)
        self.btnSend.clicked.connect(self.send_data)
        
        ### end of Serial Terminal UI
        
        ### Egcs Setup UI
        self.btnEgcsSave.clicked.connect(self.save_egcs_setup)
        self.btnEgcsLoad.clicked.connect(self.load_egcs_setup)
        self.cbSystemMode.addItems(["BLE","WIFI","ESPNOW"])
        
        # Firmware upload UI
        # self.firmware_file = ""
        self.folder_path = ""
        self.selected_port = ""
        self.chipSelect = "esp32"
        self.uploadSpeed = 921600
        
        self.load_firmware_folder()

        # Connect button actions to respective slots
        self.btnFindFWFile.clicked.connect(self.find_firmware_folder)
        self.btnFWUpload.clicked.connect(self.upload_firmware)
        self.btnClearUploadLog.clicked.connect(self.clearUploadLog)
        
        ### end of Firmware upload UI
        
        # BleTC UI
        self.btnBleTCLoad.clicked.connect(self.load_ble_tc)
        self.btnBleTCSave.clicked.connect(self.save_ble_tc)
        
        ### end of BleTC UI
        
    @Slot()
    def save_egcs_setup(self):
        """Save the EGCS setup to a file."""
        # Get the values from the line edits
        
        try :
            egcs_setup = {
                "systemMode": self.cbSystemMode.currentIndex(),
                "ssid": self.le_ssid.text(),
                "password": self.lePasswd.text(),
                # "trigger_delay" : int(self.leTriggerDelay.text()),
                "remoteHost" : self.leRemoteHost.text()
            }
            
            # trigger_delay 값이 공백이 아니면 int로 변환하여 추가
            trigger_delay_text = self.leTriggerDelay.text()
            if trigger_delay_text.strip():  # 공백 제거 후 값이 있는지 확인
                egcs_setup["trigger_delay"] = int(trigger_delay_text)
            
            if  "systemMode" in egcs_setup and egcs_setup['systemMode'] is not None:
                self._sendSerialData(f"config set systemMode {egcs_setup['systemMode']}\n")
            if "ssid" in egcs_setup and egcs_setup['ssid'] is not None:
                self._sendSerialData(f"config set ssid {egcs_setup['ssid']}\n")
            if "password" in egcs_setup and egcs_setup['password'] is not None:
                self._sendSerialData(f"config set password {egcs_setup['password']}\n")
            if "trigger_delay" in egcs_setup and egcs_setup['trigger_delay'] is not None:
                self._sendSerialData(f"config set trigger_delay {egcs_setup['trigger_delay']}\n")
            if "remoteHost" in egcs_setup and egcs_setup['remoteHost'] is not None and isinstance(egcs_setup['remoteHost'], str):
                self._sendSerialData(f"config set remoteHost {egcs_setup['remoteHost']}\n")
                
            self._sendSerialData("config save\n")
        except Exception as e:
            QMessageBox.warning(self, "Error", f"Failed to save configuration: {e}")
        
    def load_egcs_setup(self):
        """Load the EGCS setup from a file."""
        self._sendSerialData("config dump\n")
    
    @Slot()
    def show_about_dialog(self):
        """Show the about dialog."""
        QMessageBox.about(self, "About", f"Qunoitch version {__VERSION__}")
        
    def load_firmware_folder(self):
        """Load the previously saved firmware file path if it exists."""
        if os.path.exists("_fw.txt"):
            with open("_fw.txt", "r") as f:
                self.folder_path = f.read().strip()
                self.lineEdit.setText(self.folder_path)
                print(f"Loaded saved firmware file: {self.folder_path}")
        else:
            print("No saved firmware file found.")

    def save_firmware_folder(self):
        """Save the current firmware file path to _fw.txt."""
        with open("_fw.txt", "w") as f:
            f.write(self.folder_path)
        print(f"Saved firmware file: {self.folder_path}")
        
    @Slot()
    def find_firmware_folder(self):
        """Open a directory dialog to select the firmware folder."""
        options = QFileDialog.Options()
        
        # QFileDialog.getExistingDirectory()로 디렉토리 선택
        folder_path = QFileDialog.getExistingDirectory(self, "Select Firmware Folder", "", options=options)
        
        if folder_path:
            # 선택된 폴더 경로를 lineEdit에 표시
            self.folder_path = folder_path
            self.lineEdit.setText(self.folder_path)
            
            # 디렉토리 경로를 저장 (필요할 경우)
            self.save_firmware_folder()

    @Slot()
    def scan_serial_ports(self):
        """Scan for available serial ports and update the combo box."""
        ports = scan_serial_ports()
        self.cbPortList.clear()  # Clear the combo box before populating
        
        self.seiralPorts = ports
        
        #for loop dict
        for port in ports:
            self.cbPortList.addItem(f"{port['device']}[{port['description']}]",port)
            print(port)
        
        # for port in ports:
        #     print(port.device)
        #     # self.cbPortList.addItem(port.device)

        if not ports:
            QMessageBox.information(self, "No Ports Found", "No serial ports were found.")

    @Slot()
    def connect_serial_port(self):
        """Connect to or disconnect from the selected serial port."""
        
        # If already connected, disconnect
        if self.serial_thread and self.serial_thread.isRunning():
            self.serial_thread.stop()
            self.serial_thread.wait()
            self.serial_thread = None
            
            QMessageBox.information(self, "Disconnection", "Disconnected from serial port.")
            
            # Change button text back to "Connect"
            self.btnConnect.setText("Connect")
            
            return
        
        

        # port_name = self.cbPortList.currentText()
        port_name = self.cbPortList.currentData()['device']
        if port_name:
            try:
                self.btnConnect.setText("Connecting...")
                self.btnConnect.setEnabled(False)
                
                #repaint
                self.repaint()
                
                # Connect to the serial port
                self.serial_thread = SerialThread( port_name= port_name,baudrate=115200 )
                self.serial_thread.data_received.connect(self.update_received_data)
                self.serial_thread.start()
                QMessageBox.information(self, "Connection", f"Connected to {port_name}")
                
                # Change button text to "Disconnect"
                self.btnConnect.setText("Disconnect")
                # self.btnConnect.setEnabled(True)
                
            except Exception as e:
                QMessageBox.critical(self, "Error", f"Could not open serial port: {e}")
                self.btnConnect.setText("Connect")
            self.btnConnect.setEnabled(True)
        else:
            QMessageBox.warning(self, "Warning", "Please select a serial port.")


    def _sendSerialData(self, data):
        if self.serial_thread and self.serial_thread.serial_port.is_open:
            if data:
                try:
                    self.serial_thread.serial_port.write(data.encode('utf-8'))
                except Exception as e:
                    QMessageBox.warning(self, "Error", f"Failed to send data: {e}")
            else:
                QMessageBox.warning(self, "Warning", "Please enter data to send.")
        else:
            QMessageBox.warning(self, "Warning", "No serial port connected.")

    
    @Slot()
    def send_data(self):
        """Send data through the serial port."""
        data = self.leReqData.text()
        self._sendSerialData(data)
        
        # if self.serial_thread and self.serial_thread.serial_port.is_open:
            
        #     if data:
        #         self.serial_thread.serial_port.write(data.encode('utf-8'))
        #     else:
        #         QMessageBox.warning(self, "Warning", "Please enter data to send.")
        # else:
        #     QMessageBox.warning(self, "Warning", "No serial port connected.")
    
    @Slot(str)
    def update_received_data(self, data):
        """Update the QTextEdit with received data."""
        
        
        #현제 활성화된 탭의 이름을 출력
        # print(self.tabWidget.currentWidget().objectName())
        
        # 활성화된 탭이 "tabBleTC"일 때만 처리
        if self.tabWidget.currentWidget().objectName() == "tabBleTC":
            try:
                # data가 JSON인지 확인하고 파싱
                json_data = json.loads(data)  # JSON 데이터를 파싱
                
                # if "ms" in json_data and "debounceDelay" in json_data["ms"]:
                #     _deBounceDelay = json_data["ms"]["debounceDelay"]
                #     self.leBleTC_debounceDelay.setText(str(_deBounceDelay))
                
                if "ms" in json_data :
                    if json_data["ms"] == "config saved":
                        self.leBleTC_debounceDelay.setText("")
                        QMessageBox.information(self, "Success", "Configuration saved successfully!")
                    elif "debounceDelay" in json_data["ms"]:
                        # debounceDelay 값이 있는지 확인하고 값을 설정
                        _deBounceDelay = json_data["ms"]["debounceDelay"]
                        self.leBleTC_debounceDelay.setText(str(_deBounceDelay))
                    
            except json.JSONDecodeError:
                print("유효한 JSON 데이터가 아닙니다.")
        elif self.tabWidget.currentWidget().objectName() == "tabTerminal":
            self.teRecvData.append(data)
            
        elif self.tabWidget.currentWidget().objectName() == "tabEgcsSetup":
            try :
                json_data = json.loads(data)  # JSON 데이터를 파싱
                if "ms" in json_data :
                    if isinstance(json_data["ms"], str):
                        strResult = json_data["ms"]
                        if strResult == "config saved":
                            QMessageBox.information(self, "Success", "Configuration saved successfully!")
                        else :
                            print(strResult)
                    elif isinstance(json_data["ms"], dict):
                        _config_json = json_data["ms"]

                        # "ssid" 키가 존재하고 값이 유효한지 확인
                        if "ssid" in _config_json :
                            self.le_ssid.setText(_config_json["ssid"])

                        # "password" 키가 존재하고 값이 유효한지 확인
                        if "password" in _config_json:
                            self.lePasswd.setText(_config_json["password"])

                        # "systemMode" 키가 존재하고 값이 정수인지 확인
                        if "systemMode" in _config_json:
                            if isinstance(_config_json["systemMode"], str):
                                self.cbSystemMode.setCurrentIndex(int(_config_json["systemMode"]))
                            else :
                                self.cbSystemMode.setCurrentIndex(_config_json["systemMode"])

                        # "trigger_delay" 키가 존재하고 값이 정수인지 확인 (setText는 문자열을 받으므로 str로 변환)
                        if "trigger_delay" in _config_json:
                            if isinstance(_config_json["trigger_delay"], int):
                                self.leTriggerDelay.setText(str(_config_json["trigger_delay"]))
                            else :
                                self.leTriggerDelay.setText(_config_json["trigger_delay"])

                        # "remoteHost" 키가 존재하고 값이 유효한지 확인
                        if "remoteHost" in _config_json and isinstance(_config_json["remoteHost"], str):
                            self.leRemoteHost.setText(_config_json["remoteHost"])
                        
            except json.JSONDecodeError:
                print("유효한 JSON 데이터가 아닙니다." , data)

    @Slot()
    def open_option_dialog(self):
        """Open the option dialog."""
        _dlg = OptionDialog(self)
        if _dlg.exec() == OptionDialog.Accepted:
            baudrate = _dlg.get_baudrate()
            self.selected_baudrate = baudrate
            print(f"Selected Baudrate: {baudrate}")
        else:
            print("Dialog was cancelled.")
            
    @Slot()
    def upload_firmware(self):
        """Upload the selected firmware to the connected device via serial port."""
        if not self.folder_path:
            QMessageBox.warning(self, "Warning", "Please select a firmware file first.")
            return

        # Get the selected serial port from the combo box
        self.selected_port = self.cbPortList.currentData()['device']
        if not self.selected_port:
            QMessageBox.warning(self, "Warning", "Please select a serial port.")
            return

        self.btnFWUpload.setEnabled(False)
        self.btnFWUpload.setText("Uploading...")
        
        # Start firmware upload in a separate thread to avoid freezing the GUI
        self.upload_thread = FirmwareUploadThread(
            port=self.selected_port,folder_path=self.folder_path )
        
        self.upload_thread.output_signal.connect(self.update_log)
        self.upload_thread.finished_signal.connect(self.upload_finished)
        self.upload_thread.start()
    
    @Slot()  
    def clearUploadLog(self):
        """Clear the upload log in the QPlainTextEdit."""
        self.teUploadLog.clear()

    @Slot(str)
    def update_log(self, text):
        """Update the log in the QTextEdit with output from the subprocess."""
        # self.teRecvData.append(text)
        print(text)
        self.teUploadLog.appendPlainText(text)
        
    @Slot(bool)
    def upload_finished(self, success):
        """Handle the completion of the firmware upload."""
        self.btnFWUpload.setEnabled(True)
        self.btnFWUpload.setText("Upload")
        if success:
            QMessageBox.information(self, "Success", "Firmware uploaded successfully!")
        else:
            QMessageBox.critical(self, "Error", "Firmware upload failed!")
            
            
    @Slot()
    def load_ble_tc(self):
        self._sendSerialData("config dump\n")
        
    def save_ble_tc(self):
        
        debounceDelay = self.leBleTC_debounceDelay.text()
        
        self._sendSerialData(f"config set debounceDelay {debounceDelay}\n")
        self._sendSerialData("config save\n")

    
    def closeEvent(self, event):
        """Handle the window close event."""
        if self.serial_thread and self.serial_thread.isRunning():
            self.serial_thread.stop()
            self.serial_thread.wait()
        event.accept()
        

        


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec())
