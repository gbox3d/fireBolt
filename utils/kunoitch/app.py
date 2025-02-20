#filename : app.py
#author : gbox3d & GPT4-o , 2024.09.09

import os
import sys
# import subprocess
import json
from time import sleep  # JSON 데이터 처리를 위해 추가
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
        
        # ice man UI
        self.btn_icaman_load.clicked.connect(self.load_iceman)
        self.btn_icaman_save.clicked.connect(self.save_iceman)
        self.btn_iceman_Reboot.clicked.connect(self.rebootIceman)
        
        
        # moai UI
        # self.moai_btn_load.clicked.connect(self.load_moai)
        self.moai_btn_load.clicked.connect(lambda : self._sendSerialData("print\n") )
        self.moai_btn_save.clicked.connect(self.save_moai)
        self.moai_btn_clear.clicked.connect(self.clear_moai)
        self.moai_btn_reboot.clicked.connect(lambda : self._sendSerialData("reboot\n") ) 
        #람다 함수로 처리
        # self.moai_btn_reboot.clicked.connect(lambda : self._sendSerialData("print\n"))
    
    
    @Slot()
    def clear_moai(self):
        self.moai_le_ssid.setText("")
        self.moai_le_passwd.setText("")
        self.moai_le_targetip.setText("")
        self.moai_le_targetport.setText("")
        self.moai_le_devicenumber.setText("")
        self.moai_le_triggerdelay.setText("")
        self.moai_checkbox_imu.setChecked(False)
    # @Slot()
    # def load_moai(self):
    #     self._sendSerialData("print\n")
    @Slot()
    def save_moai(self) :
        try :
            moai_setup = {
                "mStrAp": self.moai_le_ssid.text(),
                "mStrPassword": self.moai_le_passwd.text(),
                "mTargetIp": self.moai_le_targetip.text(),
                # "mTargetPort": self.moai_le_targetport.text(),
                # "mDeviceNumber": self.moai_le_devicenumber.text(),
                # "mTriggerDelay": self.moai_le_triggerdelay.text(),
                "mIsUseImu": 1 if self.moai_checkbox_imu.isChecked() else 0
            }
            
            moai_setup["mTargetPort"] = int(self.moai_le_targetport.text())
            moai_setup["mDeviceNumber"] = int(self.moai_le_devicenumber.text())
            moai_setup["mTriggerDelay"] = int(self.moai_le_triggerdelay.text())
            
            self._sendSerialData(f"wifi connect {moai_setup['mStrAp']} {moai_setup['mStrPassword']}\n")
            # sleep(0.5)
            
            self._sendSerialData(f"config devid {moai_setup['mDeviceNumber']} \n")
            # sleep(0.5)
            
            self._sendSerialData(f"config triggerdelay {moai_setup['mTriggerDelay']} \n")
            # sleep(0.5)
            
            strTemp = f"config target {moai_setup['mTargetIp']} {moai_setup['mTargetPort']} \n"
            print(strTemp)
            self._sendSerialData(strTemp)
            
            if moai_setup['mIsUseImu'] == 1:
                self._sendSerialData(f"imu use\n")
            else :
                self._sendSerialData(f"imu notuse\n")
                
            sleep(0.5)
            self._sendSerialData("save\n")
            
            QMessageBox.information(self, "Success", "Configuration saved successfully!")
            
        except Exception as e:
            QMessageBox.warning(self, "Error", f"Failed to save configuration: {e}")
    
    @Slot()
    def rebootIceman(self):
        self.lineEdit_iceman_ssid.setText("")
        self.lineEdit_iceman_passwd.setText("")
        self.lineEdit_icaman_apiurl.setText("")
        self.lineEdit_iceman_restcall_term.setText("")
        self.lineEdit_iceman_logid.setText("")
        self.label_iceman_devid.setText("device id : ")
        self._sendSerialData("reboot\n")
        
    @Slot()
    def load_iceman(self):
        self._sendSerialData("config dump\n")
    
    @Slot()
    def save_iceman(self):
        
        """Save the iceman setup to a file."""
        
        print("save iceman")
        try :
            iceman_setup = {
                "ssid": self.lineEdit_iceman_ssid.text(),
                "password": self.lineEdit_iceman_passwd.text(),
                "api_url": self.lineEdit_icaman_apiurl.text(),
                "rest_call_term": self.lineEdit_iceman_restcall_term.text(),
                "log_id": self.lineEdit_iceman_logid.text()
            }
            
            if "ssid" in iceman_setup and iceman_setup['ssid'] is not None:
                self._sendSerialData(f"config set ssid {iceman_setup['ssid']}\n")
            if "password" in iceman_setup and iceman_setup['password'] is not None:
                self._sendSerialData(f"config set password {iceman_setup['password']}\n")
            if "api_url" in iceman_setup and iceman_setup['api_url'] is not None:
                self._sendSerialData(f"config set api_url {iceman_setup['api_url']}\n")
            if "rest_call_term" in iceman_setup and iceman_setup['rest_call_term'] is not None:
                self._sendSerialData(f"config set rest_call_term {iceman_setup['rest_call_term']}\n")
            if "log_id" in iceman_setup and iceman_setup['log_id'] is not None:
                self._sendSerialData(f"config set log_id {iceman_setup['log_id']}\n")
                
            self._sendSerialData("config save\n")
        except Exception as e:
            QMessageBox.warning(self, "Error", f"Failed to save configuration: {e}")
        
        
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
                
                self._sendSerialData("about\n")
                
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
        print(data)
        
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
        elif self.tabWidget.currentWidget().objectName() == "tabIcaman":
            
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
                        
                        if "ssid" in _config_json :
                            self.lineEdit_iceman_ssid.setText(_config_json["ssid"])
                        # else : # default value
                        #     print("ssid not found and set default value")
                        #     self.lineEdit_iceman_ssid.setText("redstar0427")
                        if "password" in _config_json:
                            self.lineEdit_iceman_passwd.setText(_config_json["password"])
                        # else : # default value
                        #     print("password not found and set default value")
                        #     self.lineEdit_iceman_passwd.setText("123456789a")
                        if "api_url" in _config_json :
                            self.lineEdit_icaman_apiurl.setText(_config_json["api_url"])
                        if "rest_call_term" in _config_json :
                            if isinstance(_config_json["rest_call_term"], int):
                                self.lineEdit_iceman_restcall_term.setText(str(_config_json["rest_call_term"]))
                            else :
                                self.lineEdit_iceman_restcall_term.setText(_config_json["rest_call_term"])
                        if "log_id" in _config_json :
                            self.lineEdit_iceman_logid.setText(_config_json["log_id"])
                    
                            
                        QMessageBox.information(self, "Success", "Configuration loaded successfully!")
                if "chipid" in json_data :
                    self.label_iceman_devid.setText(f"device id : {json_data['chipid']}")
                        
            except json.JSONDecodeError:
                print("유효한 JSON 데이터가 아닙니다." , data)
        elif self.tabWidget.currentWidget().objectName() == "tabMoai":
            
            """
            data 문자열 형식 
            mStrAp: redstar0427
            mStrPassword:     3117-2001
            mTargetIp:        
            mTargetPort: 0    
            mDeviceNumber: 0  
            mTriggerDelay: 150
            mIsUseImu: 0      
            mOffsets:
            offset0: 0
            offset1: 0
            offset2: 0
            offset3: 0
            offset4: 0
            offset5: 0
            """
            try:
                # 한 라인씩 분리하고 ':'로 분리해서 딕셔너리로 만들기
                # _lines = data.split("\n")
                
                # current_key = None
                # for _line in _lines:
                #     _line = _line.strip()
                #     if _line == "":
                #         continue
                #     if ":" in _line:
                #         _key, _value = _line.split(":", 1)
                #         _key = _key.strip()
                #         _value = _value.strip()
                        
                #         # 오프셋 키 값을 리스트 형태로 저장
                #         if "offset" in _key:
                #             if "mOffsets" not in _moai_setup:
                #                 _moai_setup["mOffsets"] = []
                #             _moai_setup["mOffsets"].append(int(_value))
                #         else:
                #             _moai_setup[_key] = _value
                
                # : 가 있는지 검사
                if ":"  in data:
                        
                    
                    _key, _value = data.split(":", 1)
                    
                    #앞뒤 공백 제거
                    _key = _key.strip()
                    _value = _value.strip()
                    
                    if _key == "mStrAp":
                        self.moai_le_ssid.setText(_value)
                    elif _key == "mStrPassword":
                        self.moai_le_passwd.setText(_value)
                    elif _key == "mTargetIp":
                        self.moai_le_targetip.setText(_value)
                    elif _key == "mTargetPort":
                        self.moai_le_targetport.setText(_value)
                    elif _key == "mDeviceNumber":
                        self.moai_le_devicenumber.setText(_value)
                    elif _key == "mTriggerDelay":
                        self.moai_le_triggerdelay.setText(_value)
                    elif _key == "mIsUseImu":
                        self.moai_checkbox_imu.setChecked(int(_value))
                    
                # print(_moai_setup)
            
            except ValueError as e:
                print(f"Error parsing line: {data}, {str(e)}")
            except Exception as e:
                print(f"Unexpected error: {str(e)}")

                
                
              
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
        
    @Slot()
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
