
from PyQt6 import QtWidgets, uic
from PyQt6.QtCore import QFile, QThread, pyqtSignal
from PyQt6.QtCore import Qt
from PyQt6.QtGui import QStandardItemModel, QStandardItem
import socket

import json

class UDPListener(QThread):
    
    data_received = pyqtSignal(str, str)  # 데이터와 주소를 함께 전달하는 시그널

    def __init__(self):
        super().__init__()
        self.running = True

    def run(self):
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.bind(('', 7204))
        sock.settimeout(1)

        while self.running:
            try:
                data, addr = sock.recvfrom(1024)
                self.data_received.emit(data.decode(),addr[0])
            except socket.timeout:
                pass
        sock.close()
        print("UDP listener stopped")

    def stop(self):
        self.running = False

class FindDeviceDialog(QtWidgets.QDialog):
    def __init__(self, parent=None):
        super().__init__(parent)
        
        # UI 파일 로드
        ui_file = QFile("findDeviceDlg.ui")
        ui_file.open(QFile.OpenModeFlag.ReadOnly)
        uic.loadUi(ui_file, self)
        ui_file.close()
        
        self.device_dict = {}  # chipid를 키로 사용하여 중복 제거

        # ListView 모델 설정
        self.modelForlvAdress = QStandardItemModel()
        self.lvAdress.setModel(self.modelForlvAdress)

        # UDP 리스너 스레드 시작
        self.udp_listener = UDPListener()
        self.udp_listener.data_received.connect(self.on_data_received)
        self.udp_listener.start()
    
    def get_selected_address(self):
        selected_indexes = self.lvAdress.selectedIndexes()
        if selected_indexes:
            selected_item = self.modelForlvAdress.itemFromIndex(selected_indexes[0])
            chipid = selected_item.data(Qt.ItemDataRole.UserRole)
            address = selected_item.text().split(" - ")[1]  # "chipid - address" 형식에서 주소 추출
            return chipid, address
        return None, None

    def on_data_received(self, data,addr):
        try:
            # JSON 파싱
            json_data = json.loads(data)
            chipid = json_data.get("chipid")
            device_type = json_data.get("type")
            
            print(f"addr: {addr}, chipid: {chipid}, device_type: {device_type}")

            if chipid and device_type == "broadcast":
                display_text = f"{chipid} - {addr}"
                
                if chipid not in self.device_dict:
                    item = QStandardItem(display_text)
                    item.setData(chipid, Qt.ItemDataRole.UserRole)  # chipid를 사용자 데이터로 저장
                    self.modelForlvAdress.appendRow(item)
                    self.device_dict[chipid] = item
                else:
                    # 이미 존재하는 항목 업데이트
                    existing_item = self.device_dict[chipid]
                    existing_item.setText(display_text)

        except json.JSONDecodeError:
            print(f"Invalid JSON data received: {data}")
        except Exception as e:
            print(f"Error processing received data: {e}")

    def cleanup(self):
        print("Cleaning up UDP listener")
        if hasattr(self, 'udp_listener'):
            self.udp_listener.stop()
            self.udp_listener.wait()


    def accept(self):
        self.cleanup()
        super().accept()

    def reject(self):
        self.cleanup()
        super().reject()

    def closeEvent(self, event):
        # print("closeEvent called : FindDeviceDialog")
        self.cleanup()
        super().closeEvent(event)
        
if __name__ == "__main__":
    app = QtWidgets.QApplication([])
    dialog = FindDeviceDialog()
    dialog.exec()
    app.exit()
