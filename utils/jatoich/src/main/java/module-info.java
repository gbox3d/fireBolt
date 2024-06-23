module com.bsq.jatoich {
    requires javafx.controls;
    requires javafx.fxml;
    requires javafx.graphics;
    requires org.json;
    requires com.fazecast.jSerialComm;


    opens com.bsq.jatoich to javafx.fxml;
    exports com.bsq.jatoich;
}
