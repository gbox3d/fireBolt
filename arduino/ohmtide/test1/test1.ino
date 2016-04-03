/*
  DNS and DHCP-based Web client
 
 This sketch connects to a website (http://www.google.com)
 using an Arduino Wiznet Ethernet shield. 
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 
 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe, based on work by Adrian McEwen
 
 */

#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = {  0x90, 0xA2, 0xDA, 0x0D, 0xCC, 0xE2 };
//40-5E-1D-47-57-70
//http://1.214.222.70:8051/check_job
//http://1.214.222.77/
//http://203.249.115.50:58090/


IPAddress server_ip(1,214,222,77);
const int server_port = 9304;
String userID = "A0001";
//char serverName[] = "m.naver.com";

//IPAddress server_ip(203,249,115,50);
//const int server_port = 58090;

//
EthernetClient g_client;

String g_strCurrentStatus;
unsigned long g_RequestCount;


void request_Server(EthernetClient *client) {
  
  if (client->connect(server_ip, server_port)) {
    
    String strReq = "GET /sparkDeviceCommandCheck?id=";
    
    strReq += userID;
    
 //   String strReq = "GET /check_job?id=144:162:218:13:204:226";
    //String strReq = "GET /check_job";
    //String strReq = "GET /";
    
    
    strReq += " HTTP/1.1";
    
    Serial.println("connection success");
    Serial.println(strReq);
    
    client->println(strReq);
    client->println("Host: www.arduino.cc");
    client->println("User-Agent: arduino-ethernet");
    client->println("Connection: close");
    client->println();
    
  } 
  else {
    // kf you didn't get a connection to the server:
    Serial.println("connection failed");
    g_strCurrentStatus = "failed";
  }

}

////////tinny json parser

String ADJson_getData(Stream *stream,char *name)
{
  if(!stream->find(name)) {
    Serial.print(name);
    Serial.println(" not found pasre error");
  }
  else {
    
    if(!stream->find(":")) {
      Serial.println(" syntex error : not founded");
      
    }
    else {
      if(!stream->find("\"")) {
        Serial.println(" syntex error \" not founded");
      }
      else {
        char buf[16];
        byte read_size;
        read_size = stream->readBytesUntil('\"',buf,16);
        
        if(read_size <= 0 || read_size > 16) {
          Serial.println(String("pasre error : cmd size ") + read_size);
        }
        else {
          buf[read_size] = 0x00;
          return String(buf);
        }
      }
    }
  }
  
  return String("error");
}


////////

//control main 
void parseCmd(EthernetClient *client) {
  
  
  client->setTimeout(3000);
  
  
    //Serial.println("begine msg");
  /*
  while (client->available()) {
    char c = client->read();
    Serial.print(c);
  }
  */
  
  //Serial.println("end msg");
  String strResult = ADJson_getData(client,"result");
  Serial.println(strResult);
  
  if(strResult == "LoadSuccess") {
    
    String ID = ADJson_getData(client,"ID");
    String commandCode = ADJson_getData(client,"commandCode");
    String commandID = ADJson_getData(client,"commandID");
    
    Serial.println(ID);
    Serial.println(commandCode);
    Serial.println(commandID);
  
  }
  
  
  //clear stream buf 
  client->flush();
    
}

//implement console shell interface 
void processShell()
{
  char buf[16];
  
  if(Serial.available() > 0) {
    
    byte read_size;
    read_size = Serial.readBytesUntil('\r',buf,16);
    
    if(read_size <= 0 || read_size > 16) {
      Serial.println(String("pasre error : cmd size ") + read_size);
      return;
    }
    else {
      buf[read_size] = 0x00;
      if(buf[read_size-1] == '\n')
        buf[read_size-1] = 0x00;
        
    }
    
    String strCmd = buf;
    
    if(strCmd == "status") {
      Serial.println(String("status :") + g_strCurrentStatus);
      Serial.println(String("network request count :") + g_RequestCount);
    }
    else {
      Serial.println(strCmd + " is syntex error");
    }
    
  }
  
}

//----------------------------------------------------

void setup() {
  
  g_strCurrentStatus = "setup";
  g_RequestCount = 0;
  
  /*
  
  pinMode(g_pin_door,OUTPUT);
  pinMode(g_pin_light,OUTPUT);
  pinMode(g_network_connection,OUTPUT);
  pinMode(g_network_status,OUTPUT);
  
  digitalWrite(g_pin_door,LOW);
  digitalWrite(g_pin_light,LOW);
  digitalWrite(g_network_connection,LOW);
  digitalWrite(g_network_status,LOW);
  
  */
  
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  Serial.println("start m-os v1.0");
  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    while(true);
  }
  
 // Ethernet.setTimeout(5000);
 
  Serial.println("setup ok");
  
  g_strCurrentStatus = "begin";
  
  //request_Server();
  
}



void loop()
{
  delay(1000);
  /*
  if(g_strCurrentStatus == "failed") {
    digitalWrite(g_network_status,HIGH);
  }
  else {
    digitalWrite(g_network_status,LOW);
  }
  */
 
  processShell();
  parseCmd(&g_client);

  // if the server's disconnected, stop the client:
  if (!g_client.connected()) {
    //Serial.println("disconnecting.");
    //Serial.println();
    g_client.stop();
    
    //while(1);
    request_Server(&g_client);    

  }
}
