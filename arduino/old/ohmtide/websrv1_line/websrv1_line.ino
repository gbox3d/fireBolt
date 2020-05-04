/*
  Web Server
 
 A simple web server that shows the value of the analog input pins.
 using an Arduino Wiznet Ethernet shield. 
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog inputs attached to pins A0 through A5 (optional)
 
 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe
 
 */

#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 
  //0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED 
 0x90, 0xA2, 0xDA, 0x0D, 0xCC, 0xE2
};
IPAddress ip(192,168,9,80);

const int onoff_Pin = 9;

// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):
EthernetServer server(12080);

void setup() {
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
  
  pinMode(onoff_Pin,OUTPUT);
  digitalWrite(onoff_Pin,LOW);


  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}



void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    String strUrl;
    String strPath;
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        
        //Serial.write(c);
        strUrl += c;
 
   
        if(strUrl.endsWith(String("HTTP/1.1")) == true) {
          
          strPath = strUrl.substring(4,strUrl.length() - 9);
          
          /*
          Serial.println(path);
          strUrl = "";
          
          if(path.equals("/on")) {
          }
          else if(path.equals("/off")) {
          }
          */
          
        }
        
        
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          
          Serial.println(strPath);
          
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/plain");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          //client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          
          
          client.print("{");
          client.print("\"result\":\"ok\"");
          if(strPath.equals("/on")) {
            digitalWrite(onoff_Pin,HIGH);
            client.print(",\"cmd\":\"on\"");
          }
          else if(strPath.equals("/off")) {
            digitalWrite(onoff_Pin,LOW);
            client.print(",\"cmd\":\"off\"");
          }
          
          client.println("}");
          
          /*
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<div>hello arduino</div>");
          
          
          // output the value of each analog input pin
          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            int sensorReading = analogRead(analogChannel);
            client.print("analog input ");
            client.print(analogChannel);
            client.print(" is ");
            client.print(sensorReading);
            client.println("<br />");       
          }
          
          client.println("</html>");
          */
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    
    //Serial.println(strUrl);
    
    Serial.println("client disconnected");
  }
}
