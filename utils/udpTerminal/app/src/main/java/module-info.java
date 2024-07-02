module com.bsq.udpterminal {
    requires javafx.controls;
    requires javafx.fxml;
    requires javafx.graphics;
    requires org.json;

    opens com.bsq.udpterminal to javafx.fxml;
    exports com.bsq.udpterminal;
}
