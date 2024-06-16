import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import com.fazecast.jSerialComm.SerialPort;
import org.json.JSONObject;

public class MainFrame extends JFrame {
    private JComboBox<String> portComboBox;
    private JButton scanButton;
    private JButton connectButton;
    private JTextField commandField;
    private JButton sendButton;
    private JTextArea outputArea;
    private JButton aboutButton;

    // JPanel bottomPanel;
    // JPanel commandPanel;

    private jatoichApp app;

    public MainFrame(jatoichApp app) {
        // App 객체를 생성자에서 전달받아 사용
        this.app = app;

        JFrame _thisFrame = this;

        // 설정
        setTitle(app.applicationName);
        setSize(640, 480);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        // setLayout(new BorderLayout());
        setLayout(new GridLayout(3, 1));

        // 상단 패널 생성
        JPanel topPanel = new JPanel(new FlowLayout());

        portComboBox = new JComboBox<>();
        scanButton = new JButton("Scan Ports");
        connectButton = new JButton("Connect");

        topPanel.add(portComboBox);
        topPanel.add(scanButton);
        topPanel.add(connectButton);

        // add(topPanel, BorderLayout.NORTH);
        add(topPanel);

        // 명령어 패널 생성
        JPanel commandPanel = new JPanel(new FlowLayout());

        aboutButton = new JButton("About");
        aboutButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                try {

                    SerialPort port = app.getSerialPort();

                    String command = "about\r\n";

                    // flush input buffer
                    port.flushIOBuffers();
                    port.writeBytes(command.getBytes(), command.length());

                    String response = readStringFromUart();
                    outputArea.append("Sent: " + command + "\nReceived: " + response + "\n");

                } catch (Exception ex) {
                    JOptionPane.showMessageDialog(_thisFrame, "Failed to send command: " + ex.getMessage(), "Error",
                            JOptionPane.ERROR_MESSAGE);
                }

            }
        });
        // aboutButton.setEnabled(false);
        commandPanel.add(aboutButton);

        // config dump 버튼 추가
        {
            JButton _button = new JButton("Config Dump");
            _button.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent e) {
                    try {

                        SerialPort port = app.getSerialPort();

                        String command = "config dump\r\n";

                        // flush input buffer
                        port.flushIOBuffers();
                        port.writeBytes(command.getBytes(), command.length());

                        String response = readStringFromUart();
                        outputArea.append("Sent: " + command + "\nReceived: " + response + "\n");

                    } catch (Exception ex) {
                        JOptionPane.showMessageDialog(_thisFrame, "Failed to send command: " + ex.getMessage(), "Error",
                                JOptionPane.ERROR_MESSAGE);
                    }

                }
            });
            commandPanel.add(_button);
        }

        // config save 버튼 추가
        {
            JButton _button = new JButton("Config Save");
            _button.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent e) {
                    try {

                        SerialPort port = app.getSerialPort();

                        String command = "config save\r\n";

                        // flush input buffer
                        port.flushIOBuffers();
                        port.writeBytes(command.getBytes(), command.length());

                        String response = readStringFromUart();
                        outputArea.append("Sent: " + command + "\nReceived: " + response + "\n");

                    } catch (Exception ex) {
                        JOptionPane.showMessageDialog(_thisFrame, "Failed to send command: " + ex.getMessage(), "Error",
                                JOptionPane.ERROR_MESSAGE);
                    }

                }
            });
            commandPanel.add(_button);
        }

        // reboot 버튼 추가
        {
            JButton _button = new JButton("Reboot");
            _button.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent e) {
                    try {

                        SerialPort port = app.getSerialPort();

                        String command = "reboot\r\n";

                        // flush input buffer
                        port.flushIOBuffers();
                        port.writeBytes(command.getBytes(), command.length());

                        String response = readStringFromUart();
                        outputArea.append("Sent: " + command + "\nReceived: " + response + "\n");

                    } catch (Exception ex) {
                        JOptionPane.showMessageDialog(_thisFrame, "Failed to send command: " + ex.getMessage(), "Error",
                                JOptionPane.ERROR_MESSAGE);
                    }

                }
            });
            commandPanel.add(_button);
        }

        // config wifi set
        {
            JPanel _panel = new JPanel(new GridLayout(3, 2));

            JLabel _label = new JLabel("SSID:");
            _panel.add(_label);
            JTextField _field_ssid = new JTextField(20);
            _panel.add(_field_ssid);

            _label = new JLabel("Password:");
            _panel.add(_label);
            JTextField _field_passwd = new JTextField(20);
            _panel.add(_field_passwd);

            JButton _button = new JButton("Set");
            _button.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent e) {
                    try {

                        SerialPort port = app.getSerialPort();

                        String command = "config set ssid " + _field_ssid.getText() + "\r\n";
                        command += "config set password " + _field_passwd.getText() + "\r\n";

                        // String command = "config set\r\n";

                        // flush input buffer
                        port.flushIOBuffers();
                        port.writeBytes(command.getBytes(), command.length());

                        String response = readStringFromUart();
                        outputArea.append("Sent: " + command + "\nReceived: " + response + "\n");

                    } catch (Exception ex) {
                        JOptionPane.showMessageDialog(_thisFrame, "Failed to send command: " + ex.getMessage(), "Error",
                                JOptionPane.ERROR_MESSAGE);
                    }

                }
            });
            _panel.add(_button);

            _button = new JButton("Get");
            _button.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent e) {
                    try {

                        SerialPort port = app.getSerialPort();

                        String command = "config get ssid\r\n";

                        // flush input buffer
                        port.flushIOBuffers();
                        port.writeBytes(command.getBytes(), command.length());

                        String response = readStringFromUart();
                        outputArea.append("Sent: " + command + "\nReceived: " + response + "\n");

                        // 응답을 줄 단위로 분리
                        String[] lines = response.split("\\r?\\n");

                        String _jsonResult = "{}";

                        // find json string
                        for (String line : lines) {
                            if (isJson(line.trim())) {
                                _jsonResult = line;
                                break;
                            }
                        }
                        // {"result":"ok","value":"redstar0427"} 파싱하기

                        JSONObject jsonObject = new JSONObject(_jsonResult);
                        String result = jsonObject.getString("result");
                        String value = jsonObject.getString("value");

                        if (result.equals("ok")) {
                            _field_ssid.setText(value);
                        }

                        command = "config get password\r\n";
                        port.flushIOBuffers();
                        port.writeBytes(command.getBytes(), command.length());

                        response = readStringFromUart();
                        outputArea.append("Sent: " + command + "\nReceived: " + response + "\n");

                        // {"result":"ok","value":"123456789a"} json 파싱하기

                        lines = response.split("\\r?\\n");
                        _jsonResult = "{}";

                        // find json string
                        for (String line : lines) {
                            if (isJson(line.trim())) {
                                _jsonResult = line;
                                break;
                            }
                        }

                        jsonObject = new JSONObject(_jsonResult);
                        result = jsonObject.getString("result");
                        value = jsonObject.getString("value");

                        if (result.equals("ok")) {
                            _field_passwd.setText(value);
                        }

                    } catch (Exception ex) {
                        JOptionPane.showMessageDialog(_thisFrame, "Failed to send command: " + ex.getMessage(), "Error",
                                JOptionPane.ERROR_MESSAGE);
                    }

                }
            });

            _panel.add(_button);

            commandPanel.add(_panel);
        }

        // add(commandPanel, BorderLayout.CENTER);
        add(commandPanel);

        // 출력 영역 생성
        JPanel bottomPanel = new JPanel(new FlowLayout());

        outputArea = new JTextArea(10, 50);
        JScrollPane scrollPane = new JScrollPane(outputArea);

        JLabel commandLabel = new JLabel("Command:");
        commandField = new JTextField(20);
        sendButton = new JButton("Send");

        bottomPanel.add(commandLabel);
        bottomPanel.add(commandField);
        bottomPanel.add(sendButton);

        JButton clearButton = new JButton("Clear");
        clearButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                outputArea.setText("");
            }
        });
        bottomPanel.add(clearButton);

        bottomPanel.add(scrollPane);

        // add(scrollPane, BorderLayout.SOUTH);
        add(bottomPanel);

        // 이벤트 리스너 추가
        scanButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                scanPorts();
            }
        });

        connectButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                if ("Connect".equals(connectButton.getText())) {
                    connectPort();
                } else {
                    disconnectPort();
                }
            }
        });

        sendButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                sendCommand();
            }
        });

        // commandPanel.setVisible(false);
        // bottomPanel.setVisible(false);
    }

    private boolean isJson(String str) {
        try {
            new JSONObject(str);
        } catch (Exception e) {
            return false;
        }
        return true;
    }

    private void scanPorts() {
        SerialPort[] ports = SerialPort.getCommPorts();
        portComboBox.removeAllItems();
        for (SerialPort port : ports) {
            portComboBox.addItem(port.getSystemPortName());
        }
    }

    private void connectPort() {
        String selectedPort = (String) portComboBox.getSelectedItem();
        if (selectedPort == null) {
            JOptionPane.showMessageDialog(this, "Please select a port first", "Error", JOptionPane.ERROR_MESSAGE);
            return;
        }

        SerialPort port = SerialPort.getCommPort(selectedPort);
        port.setBaudRate(115200);
        // port.setComPortTimeouts(SerialPort.TIMEOUT_READ_SEMI_BLOCKING, 3000, 0);

        if (port.openPort()) {
            app.setSerialPort(port);
            connectButton.setText("Disconnect");
            JOptionPane.showMessageDialog(this, "Connected to " + selectedPort, "Success",
                    JOptionPane.INFORMATION_MESSAGE);

        } else {
            JOptionPane.showMessageDialog(this, "Failed to connect to " + selectedPort, "Error",
                    JOptionPane.ERROR_MESSAGE);
        }
    }

    private void disconnectPort() {
        SerialPort port = app.getSerialPort();
        if (port != null && port.closePort()) {
            app.setSerialPort(null);
            connectButton.setText("Connect");
            JOptionPane.showMessageDialog(this, "Serial connection closed", "Disconnected",
                    JOptionPane.INFORMATION_MESSAGE);
        } else {
            JOptionPane.showMessageDialog(this, "Failed to close port", "Error", JOptionPane.ERROR_MESSAGE);
        }
    }

    private void sendCommand() {
        SerialPort port = app.getSerialPort();
        if (port == null) {
            JOptionPane.showMessageDialog(this, "Not connected to any port", "Error", JOptionPane.ERROR_MESSAGE);
            return;
        }

        String command = commandField.getText();
        if (command.isEmpty()) {
            JOptionPane.showMessageDialog(this, "Please enter a command", "Error", JOptionPane.ERROR_MESSAGE);
            return;
        }

        try {

            // flush input buffer
            port.flushIOBuffers();
            port.writeBytes(command.getBytes(), command.length());

            String response = readStringFromUart();
            outputArea.append("Sent: " + command + "\nReceived: " + response + "\n");

        } catch (Exception e) {
            JOptionPane.showMessageDialog(this, "Failed to send command: " + e.getMessage(), "Error",
                    JOptionPane.ERROR_MESSAGE);
        }
    }

    private synchronized String readStringFromUart() {
        SerialPort port = app.getSerialPort();
        byte[] readBuffer = new byte[1024];
        StringBuilder responseBuilder = new StringBuilder();

        if (port == null) {
            return "Not connected to any port";
        }

        port.setComPortTimeouts(SerialPort.TIMEOUT_READ_SEMI_BLOCKING, 100, 0);

        Boolean bReadStarted = false;
        int numRead;
        long startTime = System.currentTimeMillis();
        while (true) {

            numRead = port.readBytes(readBuffer, readBuffer.length);
            if (numRead > 0) {

                bReadStarted = true;
                String partResponse = new String(readBuffer, 0, numRead);
                responseBuilder.append(partResponse);
                System.out.println("Received: " + numRead + " bytes , " + partResponse + "\n");
            } else {
                System.out.println("Received: 0 bytes\n");
            }

            if (bReadStarted == false) {

                // 2초 동안 대기 후 루프 종료
                if (System.currentTimeMillis() - startTime > 2000) {
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

}
