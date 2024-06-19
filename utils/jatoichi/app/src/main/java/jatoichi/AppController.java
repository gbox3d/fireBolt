package jatoichi;

import javafx.application.Platform;
import javafx.concurrent.Task;
import javafx.event.Event;
import javafx.fxml.FXML;
import javafx.scene.control.Alert;
import javafx.scene.control.Button;
import javafx.scene.control.ComboBox;
import javafx.scene.control.Label;
import javafx.scene.control.Tab;
import javafx.scene.control.TextArea;
import javafx.scene.control.TextField;

import org.json.JSONObject;

import com.fazecast.jSerialComm.SerialPort;

public class AppController {

    private SerialPort connectedPort; // 연결된 포트를 저장

    @FXML
    private ComboBox<String> cbPortList;

    @FXML
    private Button btnScan;
    @FXML
    private Button btnConnect;
    @FXML
    private Button btnSend;
    @FXML
    private Button btnClear;
    @FXML
    private TextField edtCommand;
    @FXML
    private TextArea taOutput;

    @FXML
    private TextField edtWifiSSID;

    @FXML
    private TextField edtWifiPasswd;

    // device config
    @FXML
    private TextField edtDeviceID;

    // about
    @FXML
    private Label txAboutOs;
    @FXML
    private Label txAboutApp;
    @FXML
    private Label txAboutChipID;
    @FXML
    private Label txAboutFWVersion;

    @FXML
    private void OnClickScanBtn() {
        btnScan.setDisable(true);
        btnScan.setText("Scanning...");

        Task<Void> scanTask = new Task<>() {
            @Override
            protected Void call() throws Exception {
                // 시리얼 포트 검색
                SerialPort[] ports = SerialPort.getCommPorts();

                // UI 업데이트를 위해 Platform.runLater 사용
                Platform.runLater(() -> {
                    // ComboBox 초기화
                    cbPortList.getItems().clear();

                    // 검색된 포트를 ComboBox에 추가
                    for (SerialPort port : ports) {
                        cbPortList.getItems().add(port.getSystemPortName());
                    }

                    // 포트가 하나 이상 검색되었을 때 마지막 포트 선택
                    if (ports.length > 0) {
                        cbPortList.getSelectionModel().select(cbPortList.getItems().size() - 1);
                    }
                });

                // 300ms 딜레이 추가
                Thread.sleep(300);

                return null;
            }

            @Override
            protected void succeeded() {
                btnScan.setDisable(false);
                btnScan.setText("Scan");
            }

            @Override
            protected void failed() {
                btnScan.setDisable(false);
                btnScan.setText("Scan");

                // 실패 시 에러 메시지 다이얼로그
                Throwable e = getException();
                e.printStackTrace();
                showErrorDialog("Error", "Failed to scan ports: " + e.getMessage());
            }
            // @Override
            // protected void finalize() {

            // }
        };

        Thread scanThread = new Thread(scanTask);
        scanThread.setDaemon(true); // 애플리케이션이 종료될 때 자동으로 종료
        scanThread.start();
    }

    @FXML
    private void OnClickConnectBtn() {
        if (connectedPort != null && connectedPort.isOpen()) {
            // 이미 연결된 경우, 연결 해제
            connectedPort.closePort();
            connectedPort = null;
            btnConnect.setText("Connect");
            showInfoDialog("Info", "Disconnected successfully.");
        } else {
            // 새 연결
            btnConnect.setDisable(true);
            btnConnect.setText("Connecting...");

            Task<Void> connectTask = new Task<>() {
                @Override
                protected Void call() throws Exception {
                    // ComboBox에서 선택된 포트 이름을 가져옴
                    String portName = cbPortList.getSelectionModel().getSelectedItem();

                    // 시리얼 포트 연결
                    connectedPort = SerialPort.getCommPort(portName);
                    connectedPort.setBaudRate(115200);
                    boolean portOpened = connectedPort.openPort();

                    if (!portOpened) {
                        throw new Exception("Failed to open port " + portName);
                    }

                    // 300ms 딜레이 추가
                    Thread.sleep(300);

                    return null;
                }

                @Override
                protected void succeeded() {
                    btnConnect.setDisable(false);
                    btnConnect.setText("Disconnect");
                    showInfoDialog("Success", "Connected successfully.");
                }

                @Override
                protected void failed() {
                    btnConnect.setDisable(false);
                    btnConnect.setText("Connect");

                    // 실패 시 에러 메시지 다이얼로그
                    Throwable e = getException();
                    e.printStackTrace();
                    showErrorDialog("Error", "Failed to connect: " + e.getMessage());
                }

                @Override
                protected void cancelled() {
                    btnConnect.setDisable(false);
                    btnConnect.setText("Connect");
                }
            };

            Thread connectThread = new Thread(connectTask);
            connectThread.setDaemon(true);
            connectThread.start();
        }
    }

    @FXML
    private void OnCommandEnter() {
        OnClickSendBtn();
    }

    @FXML
    private void OnClickSendBtn() {
        if (connectedPort == null || !connectedPort.isOpen()) {
            showErrorDialog("Error", "No port connected.");
            return;
        }

        String command = edtCommand.getText();
        if (command.isEmpty()) {
            showErrorDialog("Error", "Command is empty.");
            return;
        }

        Task<Void> sendTask = new Task<>() {
            @Override
            protected Void call() throws Exception {
                try {
                    // 명령어 전송
                    connectedPort.writeBytes(command.getBytes(), command.length());

                    // 응답 읽기
                    String response = readStringFromUart();

                    Platform.runLater(() -> {
                        taOutput.appendText("Sent: " + command + "\nReceived: " + response + "\n");
                    });

                } catch (Exception e) {
                    throw new Exception("Failed to send command: " + e.getMessage());
                }

                return null;
            }

            @Override
            protected void failed() {
                // 실패 시 에러 메시지 다이얼로그
                Throwable e = getException();
                e.printStackTrace();
                showErrorDialog("Error", "Failed to send command: " + e.getMessage());
            }
        };

        Thread sendThread = new Thread(sendTask);
        sendThread.setDaemon(true);
        sendThread.start();
    }

    @FXML
    private void OnClickClearBtn() {
        taOutput.clear();
    }

    @FXML
    private void OnClickFlushBtn(Event event) {

        Button _btn = (Button) event.getSource();

        Task<Void> sendTask = new Task<>() {
            @Override
            protected Void call() throws Exception {
                try {
                    // 응답 읽기
                    String response = readStringFromUart();

                    Platform.runLater(() -> {
                        taOutput.appendText(response);
                    });

                } catch (Exception e) {
                    throw new Exception("Failed to send command: " + e.getMessage());
                }

                return null;
            }

            @Override
            protected void failed() {
                // 실패 시 에러 메시지 다이얼로그
                Throwable e = getException();
                e.printStackTrace();
                showErrorDialog("Error", "Failed to send command: " + e.getMessage());
            }
        };

        Thread sendThread = new Thread(sendTask);
        sendThread.setDaemon(true);
        sendThread.start();

    }

    // basic tab
    @FXML
    private void OnClickDumpBtn() {

        System.err.println("OnClickDumpBtn");

        Task<Void> sendTask = new Task<>() {
            @Override
            protected Void call() throws Exception {
                try {
                    String command = "config dump\r\n";
                    // 명령어 전송
                    connectedPort.writeBytes(command.getBytes(), command.length());

                    // 응답 읽기
                    String response = readStringFromUart();

                    Platform.runLater(() -> {
                        taOutput.appendText("Sent: " + command + "\nReceived: " + response + "\n");
                    });

                } catch (Exception e) {
                    throw new Exception("Failed to send command: " + e.getMessage());
                }

                return null;
            }

            @Override
            protected void failed() {
                // 실패 시 에러 메시지 다이얼로그
                Throwable e = getException();
                e.printStackTrace();
                showErrorDialog("Error", "Failed to send command: " + e.getMessage());
            }
        };

        Thread sendThread = new Thread(sendTask);
        sendThread.setDaemon(true);
        sendThread.start();

    }

    @FXML
    private void OnClickConfSaveBtn() {

        Task<Void> sendTask = new Task<>() {
            @Override
            protected Void call() throws Exception {
                try {
                    String command = "config save\r\n";
                    // 명령어 전송
                    connectedPort.writeBytes(command.getBytes(), command.length());

                    // 응답 읽기
                    String response = readStringFromUart();

                    Platform.runLater(() -> {
                        taOutput.appendText("Sent: " + command + "\nReceived: " + response + "\n");
                    });

                } catch (Exception e) {
                    throw new Exception("Failed to send command: " + e.getMessage());
                }

                return null;
            }

            @Override
            protected void failed() {
                // 실패 시 에러 메시지 다이얼로그
                Throwable e = getException();
                e.printStackTrace();
                showErrorDialog("Error", "Failed to send command: " + e.getMessage());
            }
        };

        Thread sendThread = new Thread(sendTask);
        sendThread.setDaemon(true);
        sendThread.start();

    }

    @FXML
    private void OnClickConfLoadBtn() {

        Task<Void> sendTask = new Task<>() {
            @Override
            protected Void call() throws Exception {
                try {
                    String command = "config load\r\n";
                    // 명령어 전송
                    connectedPort.writeBytes(command.getBytes(), command.length());

                    // 응답 읽기
                    String response = readStringFromUart();

                    Platform.runLater(() -> {
                        taOutput.appendText("Sent: " + command + "\nReceived: " + response + "\n");
                    });

                } catch (Exception e) {
                    throw new Exception("Failed to send command: " + e.getMessage());
                }

                return null;
            }

            @Override
            protected void failed() {
                // 실패 시 에러 메시지 다이얼로그
                Throwable e = getException();
                e.printStackTrace();
                showErrorDialog("Error", "Failed to send command: " + e.getMessage());
            }
        };

        Thread sendThread = new Thread(sendTask);
        sendThread.setDaemon(true);
        sendThread.start();

    }

    @FXML
    private void OnClickRebootBtn() {

        Task<Void> sendTask = new Task<>() {
            @Override
            protected Void call() throws Exception {
                try {
                    String command = "reboot\r\n";
                    // 명령어 전송
                    connectedPort.writeBytes(command.getBytes(), command.length());

                    // 응답 읽기
                    String response = readStringFromUart();

                    Platform.runLater(() -> {
                        taOutput.appendText("Sent: " + command + "\nReceived: " + response + "\n");
                    });

                } catch (Exception e) {
                    throw new Exception("Failed to send command: " + e.getMessage());
                }

                return null;
            }

            @Override
            protected void failed() {
                // 실패 시 에러 메시지 다이얼로그
                Throwable e = getException();
                e.printStackTrace();
                showErrorDialog("Error", "Failed to send command: " + e.getMessage());
            }
        };

        Thread sendThread = new Thread(sendTask);
        sendThread.setDaemon(true);
        sendThread.start();

    }

    @FXML
    private void OnSelectAboutTab(Event event) {

        Tab selectedTab = (Tab) event.getSource();
        if (selectedTab.isSelected()) {

            if (connectedPort == null || !connectedPort.isOpen()) {
                showErrorDialog("Error", "No port connected.");
                return;
            }

            Task<Void> sendTask = new Task<>() {
                @Override
                protected Void call() throws Exception {
                    try {

                        String command = "about\r\n";
                        // 명령어 전송
                        connectedPort.writeBytes(command.getBytes(), command.length());

                        // 응답 읽기
                        String response = readStringFromUart();

                        // 응답을 줄 단위로 분리
                        String[] lines = response.split("\\r?\\n");

                        Platform.runLater(() -> {

                            String _jsonResult = "{}";

                            // find json string
                            for (String line : lines) {
                                if (isJson(line.trim())) {
                                    _jsonResult = line;

                                    JSONObject _json = new JSONObject(_jsonResult);

                                    if (_json.has("result") && _json.getString("result").equals("ok")) {

                                        if (_json.has("os")) {
                                            txAboutOs.setText(_json.getString("os"));
                                        }

                                        if (_json.has("app")) {
                                            txAboutApp.setText(_json.getString("app"));
                                        }

                                        if (_json.has("chipid")) {
                                            txAboutChipID.setText(_json.getInt("chipid") + "");
                                        }

                                        if (_json.has("version")) {
                                            txAboutFWVersion.setText(_json.getString("version"));
                                        }
                                    }

                                }
                            }

                            // taOutput.appendText("Sent: " + command + "\nReceived: " + response + "\n");
                        });

                    } catch (Exception e) {
                        throw new Exception("Failed to send command: " + e.getMessage());
                    }

                    return null;
                }

                @Override
                protected void failed() {
                    // 실패 시 에러 메시지 다이얼로그
                    Throwable e = getException();
                    e.printStackTrace();
                    showErrorDialog("Error", "Failed to send command: " + e.getMessage());
                }
            };

            Thread sendThread = new Thread(sendTask);
            sendThread.setDaemon(true);
            sendThread.start();

        }

    }

    // wifi tab
    // @FXML
    // private void OnClickWifiGetBtn(Event event) {

    // }

    @FXML
    private void OnClickWifiSetBtn(Event event) {

        Task<Void> sendTask = new Task<>() {
            @Override
            protected Void call() throws Exception {
                try {
                    // String command = "\r\n";

                    String command = "config set ssid " + edtWifiSSID.getText() + "\r\n";
                    command += "config set password " + edtWifiPasswd.getText() + "\r\n";

                    // 명령어 전송
                    connectedPort.writeBytes(command.getBytes(), command.length());

                    // 응답 읽기
                    String response = readStringFromUart();

                    Platform.runLater(() -> {
                        taOutput.appendText(response);
                    });

                } catch (Exception e) {
                    throw new Exception("Failed to send command: " + e.getMessage());
                }

                return null;
            }

            @Override
            protected void failed() {
                // 실패 시 에러 메시지 다이얼로그
                Throwable e = getException();
                e.printStackTrace();
                showErrorDialog("Error", "Failed to send command: " + e.getMessage());
            }
        };

        Thread sendThread = new Thread(sendTask);
        sendThread.setDaemon(true);
        sendThread.start();

    }

    @FXML
    private void OnSelectWifiTab(Event event) {

        Tab _Tab = (Tab) event.getSource();
        if (_Tab.isSelected()) {
            if (connectedPort == null || !connectedPort.isOpen()) {
                showErrorDialog("Error", "No port connected.");
                return;
            }

            Task<Void> sendTask = new Task<>() {
                @Override
                protected Void call() throws Exception {
                    try {
                        String command = "config dump\r\n";
                        // 명령어 전송
                        connectedPort.writeBytes(command.getBytes(), command.length());
                        // 응답 읽기
                        String response = readStringFromUart();

                        // 응답을 줄 단위로 분리
                        String[] lines = response.split("\\r?\\n");

                        Platform.runLater(() -> {

                            String _jsonResult = "{}";

                            // find json string
                            for (String line : lines) {
                                if (isJson(line.trim())) {
                                    _jsonResult = line;

                                    JSONObject _json = new JSONObject(_jsonResult);

                                    if (_json.has("result") && _json.getString("result").equals("ok")) {

                                        JSONObject _msJson = _json.getJSONObject("ms");

                                        if (_msJson.has("ssid")) {
                                            edtWifiSSID.setText(_msJson.getString("ssid"));
                                        }

                                        if (_msJson.has("password")) {
                                            edtWifiPasswd.setText(_msJson.getString("password"));
                                        }
                                        // edtWifiSSID.setText(_json.getString("ssid"));
                                        // edtWifiPasswd.setText(_json.getString("password"));
                                        // break;
                                    }
                                }
                            }

                            taOutput.appendText("Sent: " + command + "\nReceived: " + response + "\n");
                        });

                    } catch (Exception e) {
                        throw new Exception("Failed to send command: " + e.getMessage());
                    }

                    return null;
                }

                @Override
                protected void failed() {
                    // 실패 시 에러 메시지 다이얼로그
                    Throwable e = getException();
                    e.printStackTrace();
                    showErrorDialog("Error", "Failed to send command: " + e.getMessage());
                }
            };

            Thread sendThread = new Thread(sendTask);
            sendThread.setDaemon(true);
            sendThread.start();
        } else {
            edtWifiPasswd.clear();
            edtWifiSSID.clear();
        }
    }

    // device config tab
    @FXML
    private void OnSelectDEviceTab(Event event) {

        Tab _Tab = (Tab) event.getSource();

        if (_Tab.isSelected()) {

            if (connectedPort == null || !connectedPort.isOpen()) {
                showErrorDialog("Error", "No port connected.");
                return;
            }

            Task<Void> sendTask = new Task<>() {
                @Override
                protected Void call() throws Exception {
                    try {
                        String command = "config dump\r\n";
                        // 명령어 전송
                        connectedPort.writeBytes(command.getBytes(), command.length());
                        // 응답 읽기
                        String response = readStringFromUart();

                        // 응답을 줄 단위로 분리
                        String[] lines = response.split("\\r?\\n");

                        Platform.runLater(() -> {

                            String _jsonResult = "{}";

                            // find json string
                            for (String line : lines) {
                                if (isJson(line.trim())) {
                                    _jsonResult = line;

                                    JSONObject _json = new JSONObject(_jsonResult);

                                    if (_json.has("result") && _json.getString("result").equals("ok")) {

                                        JSONObject _msJson = _json.getJSONObject("ms");

                                        if (_msJson.has("device_id")) {
                                            edtDeviceID.setText(_msJson.getString("device_id"));
                                        }

                                    }
                                }
                            }

                            // taOutput.appendText("Sent: " + command + "\nReceived: " + response + "\n");
                        });

                    } catch (Exception e) {
                        throw new Exception("Failed to send command: " + e.getMessage());
                    }

                    return null;
                }

                @Override
                protected void failed() {
                    // 실패 시 에러 메시지 다이얼로그
                    Throwable e = getException();
                    e.printStackTrace();
                    showErrorDialog("Error", "Failed to send command: " + e.getMessage());
                }
            };

            Thread sendThread = new Thread(sendTask);
            sendThread.setDaemon(true);
            sendThread.start();
        }

    }

    @FXML
    private void OnClickDeviceSetBtn(Event event) {

        Task<Void> sendTask = new Task<>() {
            @Override
            protected Void call() throws Exception {
                try {
                    String command = "config set device_id " + edtDeviceID.getText() + "\r\n";

                    // 명령어 전송
                    connectedPort.writeBytes(command.getBytes(), command.length());

                    // 응답 읽기
                    String response = readStringFromUart();

                    Platform.runLater(() -> {
                        taOutput.appendText(response);
                    });

                } catch (Exception e) {
                    throw new Exception("Failed to send command: " + e.getMessage());
                }

                return null;
            }

            @Override
            protected void failed() {
                // 실패 시 에러 메시지 다이얼로그
                Throwable e = getException();
                e.printStackTrace();
                showErrorDialog("Error", "Failed to send command: " + e.getMessage());
            }
        };

        Thread sendThread = new Thread(sendTask);
        sendThread.setDaemon(true);
        sendThread.start();

    }

    // dialog functions
    private void showErrorDialog(String title, String message) {
        // 에러 메시지 다이얼로그
        Platform.runLater(() -> {
            Alert alert = new Alert(Alert.AlertType.ERROR);
            alert.setTitle(title);
            alert.setHeaderText(null);
            alert.setContentText(message);
            alert.showAndWait();
        });
    }

    private void showInfoDialog(String title, String message) {
        // 정보 메시지 다이얼로그
        Platform.runLater(() -> {
            Alert alert = new Alert(Alert.AlertType.INFORMATION);
            alert.setTitle(title);
            alert.setHeaderText(null);
            alert.setContentText(message);
            alert.showAndWait();
        });
    }

    // UART functions
    private synchronized String readStringFromUart() {

        int bufferSize = 1024;
        long timeoutLimit = 2000;

        SerialPort port = connectedPort;
        byte[] readBuffer = new byte[bufferSize];
        StringBuilder responseBuilder = new StringBuilder();

        if (port == null) {
            return "Not connected to any port";
        }

        port.setComPortTimeouts(SerialPort.TIMEOUT_READ_SEMI_BLOCKING, 100, 0);

        boolean bReadStarted = false;
        int numRead;
        long startTime = System.currentTimeMillis();
        while (true) {
            numRead = port.readBytes(readBuffer, readBuffer.length);
            if (numRead > 0) {
                bReadStarted = true;
                String partResponse = new String(readBuffer, 0, numRead);
                responseBuilder.append(partResponse);

                // 버퍼크기가 초과 되는지 검사
                if (responseBuilder.length() > bufferSize) {
                    break;
                }

                System.out.println("Received: " + numRead + " bytes , " + partResponse + "\n");
            } else {
                System.out.println("Received: 0 bytes\n");
            }

            if (!bReadStarted) {
                // 2초 동안 대기 후 루프 종료
                if (System.currentTimeMillis() - startTime > timeoutLimit) {
                    break;
                }
            } else {
                if (numRead == 0) {
                    break;
                }
            }

            // 잠시 대기 (예: 100ms)
            try {
                Thread.sleep(100);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        return responseBuilder.toString();
    }

    // json helper
    private boolean isJson(String str) {
        try {
            new JSONObject(str);
        } catch (Exception e) {
            return false;
        }
        return true;
    }
}
