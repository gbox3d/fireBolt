package com.bsq.udpterminal;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.time.LocalDateTime;
import java.util.HashMap;
import java.util.Map;

import javafx.application.Platform;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.control.*;

// import main.java.com.bsq.udpterminal.Appcontroller.Device;

import org.json.JSONObject;

public class Appcontroller implements Initializable {

    private boolean isRunning = false;
    private Thread udpThread;

    class Device {
        String ip;
        int port;
        String chipid;
        LocalDateTime lastUpdated;

        Device(String ip, int port, String chipid) {
            this.ip = ip;
            this.port = port;
            this.chipid = chipid;
            this.lastUpdated = LocalDateTime.now();
        }

        void update(String ip, int port) {
            this.ip = ip;
            this.port = port;
            this.lastUpdated = LocalDateTime.now();
        }

        public String toString() {
            return chipid + " (" + ip + ":" + port + ")";
        }
    }

    // device list
    private Map<String, Device> deviceList = new HashMap<>();

    private DatagramSocket socket;

    @Override
    public void initialize(java.net.URL location, java.util.ResourceBundle resources) {

        try {
            this.socket = new DatagramSocket(7204);
            this.deviceList = new HashMap<>();

            
            startUDPListener(); // UDP Listener thread 시작


        } catch (Exception e) {
            System.err.println("Error initializing socket: " + e.getMessage());
            // 여기서 사용자에게 오류 메시지를 표시할 수 있습니다.
        }

    }

    // --------------------------------------------------------------------------------

    @FXML
    private ChoiceBox<String> cbDevices;
    @FXML
    private TextArea TAOutput;
    @FXML
    private TextField TFCommand;
    @FXML
    private Button btnSend;
    // @FXML
    // private Button btnStart; // fxid 와 동일하게 맞춰줘야함

    // @FXML
    // private void onClickStartBtn() {
    //     System.out.println("onBtnConnect");

    //     if (!isRunning) {
    //         startUDPListener();
    //         btnStart.setText("Stop");
    //         isRunning = true;
    //     } else {
    //         stopUDPListener();
    //         btnStart.setText("Start");
    //         isRunning = false;
    //     }
    // }

    @FXML
    private void onClickSendBtn() {
        System.out.println("onBtnSend");
        String selectedDevice = cbDevices.getValue();

        if (selectedDevice != null) {
            // 선택된 디바이스의 chipid를 추출
            String chipid = selectedDevice.split(" ")[0];

            if (deviceList.containsKey(chipid)) {
                Device device = deviceList.get(chipid);
                String ip = device.ip;
                int port = device.port;
                String message = TFCommand.getText();

                message += "\n"; // Add newline to the end of the message

                try {
                    byte[] messageBytes = message.getBytes();
                    DatagramPacket packet = new DatagramPacket(messageBytes, messageBytes.length,
                            java.net.InetAddress.getByName(ip), port);
                    socket.send(packet);
                    System.out.println("Message sent to " + ip + ":" + port + " - " + message);
                } catch (IOException e) {
                    System.err.println("Error sending message: " + e.getMessage());
                }
            } else {
                System.out.println("Device not found in the device list.");
            }
        } else {
            System.out.println("No device selected.");
        }
    }

    // --------------------------------------------------------------------------------

    private void startUDPListener() {
        udpThread = new Thread(this::listenForUDP);
        udpThread.setDaemon(true);
        udpThread.start();
        isRunning = true;
    }

    private void stopUDPListener() {
        if (udpThread != null) {
            udpThread.interrupt();
            udpThread = null;
        }
    }

    private void listenForUDP() {
        // try (DatagramSocket socket = new DatagramSocket(7204)) {
        byte[] buffer = new byte[1024];
        DatagramPacket packet = new DatagramPacket(buffer, buffer.length);

        while (!Thread.currentThread().isInterrupted()) {
            try {
                this.socket.receive(packet);

                String received = new String(packet.getData(), 0, packet.getLength());
                Platform.runLater(() -> {
                    String ip = packet.getAddress().getHostAddress();
                    int port = packet.getPort();

                    System.out.println(ip + ":" + port);
                    System.out.println(received);

                    // JSON string check
                    // try {
                    if (isValidJSON(received)) {
                        JSONObject jsonObject = new JSONObject(received);

                        if (jsonObject.has("type")) {
                            String type = jsonObject.getString("type");

                            if (type.equals("broadcast")) {
                                if (jsonObject.has("chipid")) {
                                    String chipid = jsonObject.getString("chipid");
                                    updateDeviceList(chipid, ip, port);
                                } else {
                                    System.out.println("Received JSON does not contain 'chipid' field");
                                }
                            } else if (type.equals("data")) {

                                if (jsonObject.has("chipid")) {
                                    String chipid = jsonObject.getString("chipid");
                                    System.out.println("chipid: " + chipid);
                                    System.out.println("data : " + jsonObject.getString("data"));

                                    // updateDeviceList(chipid, ip, port);
                                } else {
                                }
                                // System.out.println("Received JSON does not contain 'chipid' field");
                            }
                        } else {
                            // System.out.println("Received JSON does not contain 'type' field");
                        }

                    } else {
                        System.out.println("Received data is not a valid JSON: " + received);
                    }
                    // } catch (Exception e) {
                    // System.out.println("Error processing received data: " + e.getMessage());
                    // }
                });
            } catch (IOException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }

        }

        System.out.println("UDP Listener stopped");

        // } catch (Exception e) {
        // if (!Thread.currentThread().isInterrupted()) {
        // e.printStackTrace();
        // }
        // }
    }

    // JSON 유효성 검사 메서드
    private boolean isValidJSON(String json) {
        try {
            new JSONObject(json);
        } catch (Exception e) {
            return false;
        }
        return true;
    }

    private void updateDeviceList(String chipid, String ip, int port) {
        if (deviceList.containsKey(chipid)) {
            Device device = deviceList.get(chipid);

            // Update device if IP or port has changed
            if(device.ip.equals(ip) && device.port == port) {
                return;
            }

            device.update(ip, port);
            TAOutput.appendText(device.toString() + " updated\n");
        } else {
            Device newDevice = new Device(ip, port, chipid);
            deviceList.put(chipid, newDevice);
            TAOutput.appendText("Added new device: " + chipid + "\n");
            cbDevices.getItems().add(newDevice.toString());
        }

        // Update ChoiceBox selection if needed
        if (cbDevices.getValue() == null) {
            cbDevices.setValue(deviceList.get(chipid).toString());
        }
    }
}
