import tkinter as tk
from tkinter import ttk, messagebox
import serial
import serial.tools.list_ports
import time

class SerialApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Serial Communication App")
        
        self.root.geometry('600x400')  # 창의 크기를 설정합니다.

        # 서브 패널 생성
        self.port_frame = ttk.Frame(root)
        self.port_frame.grid(column=0, row=0, padx=10, pady=10, sticky=tk.W)

        self.port_label = ttk.Label(self.port_frame, text="Select Port:")
        self.port_label.grid(column=0, row=0, padx=10, pady=10)
        
        self.port_combobox = ttk.Combobox(self.port_frame, values=self.scan_ports())
        self.port_combobox.grid(column=1, row=0, padx=10, pady=10)
        
        self.scan_button = ttk.Button(self.port_frame, text="Scan Ports", command=self.update_ports)
        self.scan_button.grid(column=2, row=0, padx=10, pady=10)
        
        self.connect_button = ttk.Button(self.port_frame, text="Connect", command=self.connect_port)
        self.connect_button.grid(column=3, row=0, padx=10, pady=10)
        
        # Command Frame
        self.command_frame = ttk.Frame(root)
        self.command_frame.grid(column=0, row=1, padx=10, pady=10, sticky=tk.W)

        self.command_label = ttk.Label(self.command_frame, text="Command:")
        self.command_label.grid(column=0, row=1, padx=10, pady=10, sticky=tk.W)
        
        self.command_entry = ttk.Entry(self.command_frame, width=30)
        self.command_entry.grid(column=1, row=1, padx=10, pady=10, sticky=tk.W)
        
        self.send_button = ttk.Button(self.command_frame, text="Send", command=self.send_command)
        self.send_button.grid(column=2, row=1, padx=10, pady=10, sticky=tk.W)
        
        self.output_text = tk.Text(self.command_frame, height=10, width=50)
        self.output_text.grid(column=0, row=2, columnspan=4, padx=10, pady=10, sticky=tk.W)
        
        self.clear_button = ttk.Button(self.command_frame, text="Clear", command=lambda: self.output_text.delete(1.0, tk.END))
        self.clear_button.grid(column=0, row=3, padx=10, pady=10, sticky=tk.W)
        
        #wifi setup frame
        
        
        
        
        self.serial_connection = None
    
    def scan_ports(self):
        ports = serial.tools.list_ports.comports()
        print(ports)
        return [port.device for port in ports]
    
    def update_ports(self):
        ports = self.scan_ports()
        self.port_combobox['values'] = ports
        
    def disconnect_port(self):
        if self.serial_connection:
            self.serial_connection.close()
            self.serial_connection = None
            messagebox.showinfo("Disconnected", "Serial connection closed")
            # self.connect_button.config(text="Connect")
    
    def connect_port(self):
        
        if self.connect_button.cget("text") == "Disconnect" :
            if self.serial_connection:
                self.disconnect_port()
            self.connect_button.config(text="Connect")
            return
        
        selected_port = self.port_combobox.get()
        if not selected_port:
            messagebox.showerror("Error", "Please select a port first")
            return
        
        try:
            
            #버튼 비활성화
            self.connect_button.config(state="disabled")
            
            self.serial_connection = serial.Serial(selected_port, 115200, timeout=1)
            
            
            
            
            self.connect_button.config(state="normal")
            # disconnect button
            self.connect_button.config(text="Disconnect")
            
            messagebox.showinfo("Success", f"Connected to {selected_port}")
            
            
        except Exception as e:
            messagebox.showerror("Error", f"Failed to connect to {selected_port}: {e}")
    
    def send_command(self):
        if not self.serial_connection:
            messagebox.showerror("Error", "Not connected to any port")
            return
        
        command = self.command_entry.get()
        if not command:
            messagebox.showerror("Error", "Please enter a command")
            return
        
        try:
            self.serial_connection.reset_input_buffer()  # 시리얼 입력 버퍼 클리어
            time.sleep(0.5)  # 0.5초 대기
            
            self.serial_connection.write(command.encode())
            # response = self.serial_connection.readline().decode('utf-8')
            
            time.sleep(0.1)  # 작은 대기 시간
            
            response = b""
            start_time = time.time()
            while True:
                if self.serial_connection.in_waiting > 0:
                    response += self.serial_connection.read(self.serial_connection.in_waiting)
                    start_time = time.time()  # 데이터가 들어왔으면 타이머를 리셋
                else:
                    if time.time() - start_time > 1:  # 1초 동안 데이터가 없으면 멈춤
                        break
                time.sleep(0.01)  # 너무 빈번한 체크를 방지
            
            response_text = response.decode('utf-8')
            self.output_text.insert(tk.END, f"Sent: {command}\nReceived: {response_text}\n")
            
            
        except Exception as e:
            messagebox.showerror("Error", f"Failed to send command: {e}")

if __name__ == "__main__":
    root = tk.Tk()
    app = SerialApp(root)
    root.mainloop()
