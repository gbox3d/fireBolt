package jatoichi;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;

public class App extends Application {

    @Override
    public void start(Stage primaryStage) throws Exception {
        Parent root = FXMLLoader.load(getClass().getResource("/Main.fxml"));

        // Scene을 root의 크기에 맞춰 설정
        primaryStage.setTitle("Jatoichi App");
        primaryStage.setScene(new Scene(root));
        
        
        // primaryStage.setScene(new Scene(root,320,240)); //크기 직접 지정

        primaryStage.show();
    }

    public static void main(String[] args) {
        launch(args);
    }

    public Object getApplicationVersion() {
        return "1.0.0";
    }

    
}
