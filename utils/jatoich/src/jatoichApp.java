import javax.swing.SwingUtilities;
import com.fazecast.jSerialComm.SerialPort;

public class jatoichApp {

    public String applicationName = "jatoich";
    private SerialPort serialPort;

    public jatoichApp() {
        // 생성자
    }

    public static void main(String[] args) throws Exception {
        System.out.println("Hello, World!");

        // App 객체 생성
        jatoichApp app = new jatoichApp();

        // Event Dispatch Thread에서 GUI 초기화
        SwingUtilities.invokeLater(new Runnable() {
            @Override
            public void run() {
                MainFrame frame = new MainFrame(app);
                frame.setVisible(true);
            }
        });
    }

    public synchronized void setSerialPort(SerialPort port) {
        this.serialPort = port;
    }

    public synchronized SerialPort getSerialPort() {
        return serialPort;
    }

    
}
