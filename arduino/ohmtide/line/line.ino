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
#include <Servo.h>
#include <EEPROM.h>

/*

duino gbox3d version
date 13-12.7

- write result 
start ! end . 
00 success 
01 error

-read result
start # end .



*/

bool debug = true;

int index = 0;

char messageBuffer[12];
char cmd[3];
char pin[3];
char val[4];
char aux[4];

Servo servo;

/*
 * Deal with a full message and determine function to call
 */
String process() {
  index = 0;

  strncpy(cmd, messageBuffer, 2);
  cmd[2] = '\0';
  strncpy(pin, messageBuffer + 2, 2);
  pin[2] = '\0';

  if (atoi(cmd) > 90) {
    strncpy(val, messageBuffer + 4, 2);
    val[2] = '\0';
    strncpy(aux, messageBuffer + 6, 3);
    aux[3] = '\0';
  } else {
    strncpy(val, messageBuffer + 4, 3);
    val[3] = '\0';
    strncpy(aux, messageBuffer + 7, 3);
    aux[3] = '\0';
  }

  if (debug) {
    Serial.println(messageBuffer);
  }
  int cmdid = atoi(cmd);

  Serial.println(cmd);
  Serial.println(pin);
  Serial.println(val);
  Serial.println(aux);
  
  String strResult = "none";

  switch(cmdid) {
    case 0:  strResult = sm(pin,val);              break;
    case 1:  strResult = dw(pin,val);              break;
    case 2:  strResult = dr(pin,val);              break;
    case 3:  strResult = aw(pin,val);              break;
    case 4:  strResult = ar(pin,val);              break;
    case 97: handlePing(pin,val,aux);  break;
    case 98: handleServo(pin,val,aux); break;
    case 99: toggleDebug(val);         break;
    default:                           break;
  }
  
  return strResult;
  
}

/*
 * Toggle debug mode
 */
void toggleDebug(char *val) {
  if (atoi(val) == 0) {
    debug = false;
    Serial.println("goodbye");
  } else {
    debug = true;
    Serial.println("hello");
  }
}

/*
 * Set pin mode
 */
String sm(char *pin, char *val) {
  if (debug) Serial.println("sm");
  int p = getPin(pin);
  if(p == -1) { if(debug) Serial.println("badpin"); 
  //error
    Serial.println("!01."); 
    return String("!01.");
  }
  if (atoi(val) == 0) {
    pinMode(p, OUTPUT);
  } else {
    pinMode(p, INPUT);
  }
  
  //success
  Serial.println("!00."); 
  return String("!00.");
  
}

/*
 * Digital write
 */
String dw(char *pin, char *val) {
  if (debug) Serial.println("dw");
  int p = getPin(pin);
  if(p == -1) { 
    
   if(debug) Serial.println("badpin");
   Serial.println("!01."); 
    return String("!01.");
    //return; 
  }
  pinMode(p, OUTPUT);
  if (atoi(val) == 0) {
    digitalWrite(p, LOW);
  } else {
    digitalWrite(p, HIGH);
  }
  
  //success
  Serial.println("!00."); 
  return String("!00.");
  
  
}

/*
 * Digital read
 */
String dr(char *pin, char *val) {
  if (debug) Serial.println("dr");
  int p = getPin(pin);
  if(p == -1) { if(debug) Serial.println("badpin"); return String("!01."); }
  pinMode(p, INPUT);
  int oraw = digitalRead(p);
  char m[7];
  sprintf(m, "!00%02d.",oraw);
  Serial.println(m);
  return String(m);
}

/*
 * Analog read
 */
String ar(char *pin, char *val) {
  if(debug) Serial.println("ar");
  
  int p = getPin(pin);
  
  if(p == -1) { if(debug) Serial.println("badpin"); return String("!01."); }
  pinMode(p, INPUT); // don't want to sw
  
  int rval = analogRead(p);
  
  char m[12];
  sprintf(m, "!00%04d.", rval);
  Serial.println(m);
  return String(m);
  
  //return String("test");
}

String aw(char *pin, char *val) {
  if(debug) Serial.println("aw");
  int p = getPin(pin);
  pinMode(p, OUTPUT);
  if(p == -1) { if(debug) Serial.println("badpin"); Serial.println("!01."); return String("!01."); }
  analogWrite(p,atoi(val));
  Serial.println("!00.");
  
  return String("!00.");
  
}

int getPin(char *pin) { //Converts to A0-A5, and returns -1 on error
  int ret = -1;
  if(pin[0] == 'A' || pin[0] == 'a') {
    switch(pin[1]) {
      case '0':  ret = A0; break;
      case '1':  ret = A1; break;
      case '2':  ret = A2; break;
      case '3':  ret = A3; break;
      case '4':  ret = A4; break;
      case '5':  ret = A5; break;
      default:             break;
    }
  } else {
    ret = atoi(pin);
    if(ret == 0 && (pin[0] != '0' || pin[1] != '0')) {
      ret = -1;
    }
  }
  return ret;
}

/*
 * Handle Ping commands
 * fire, read
 */
void handlePing(char *pin, char *val, char *aux) {
  if (debug) Serial.println("ss");
  int p = getPin(pin);

  if(p == -1) { if(debug) Serial.println("badpin"); return; }
  Serial.println("got signal");

  // 01(1) Fire and Read
  if (atoi(val) == 1) {
    char m[16];

    pinMode(p, OUTPUT);
    digitalWrite(p, LOW);
    delayMicroseconds(2);
    digitalWrite(p, HIGH);
    delayMicroseconds(5);
    digitalWrite(p, LOW);

    Serial.println("ping fired");

    pinMode(p, INPUT);
    sprintf(m, "%s::read::%08d", pin, pulseIn(p, HIGH));
    Serial.println(m);

    delay(50);
  }
}

/*
 * Handle Servo commands
 * attach, detach, write, read, writeMicroseconds, attached
 */
void handleServo(char *pin, char *val, char *aux) {
  if (debug) Serial.println("ss");
  int p = getPin(pin);
  if(p == -1) { if(debug) Serial.println("badpin"); return; }
  //Serial.println("signal: servo");

  // 00(0) Detach
  if (atoi(val) == 0) {
    servo.detach();
    //char m[12];
    //sprintf(m, "!%s::detached.", pin);
    //Serial.println(m);
    Serial.print("!00."); 

  // 01(1) Attach
  } else if (atoi(val) == 1) {
    // servo.attach(p, 750, 2250);
    servo.attach(p);
    //char m[12];
    //sprintf(m, "!%s::attached.", pin);
    //Serial.println(m);
    Serial.print("!00."); 

  // 02(2) Write
  } else if (atoi(val) == 2) {
    //Serial.println("writing to servo");
    //Serial.println(atoi(aux));
    // Write to servo
    servo.write(atoi(aux));
    
    //success
    Serial.print("!00."); 

    // TODO: Experiment with microsecond pulses
    // digitalWrite(pin, HIGH);   // start the pulse
    // delayMicroseconds(pulseWidth);  // pulse width
    // digitalWrite(pin, LOW);    // stop the pulse

  // 03(3) Read
  } else if (atoi(val) == 3) {
    //Serial.println("reading servo");
    int sval = servo.read();
    char m[13];
    sprintf(m, "#%03d.", sval);
    Serial.print(m);
  }
}

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = {  0x90, 0xA2, 0xDA, 0x0D, 0xCC, 0xE2 };
byte ip_address[4]; 
//40-5E-1D-47-57-70
//http://1.214.222.70:8051/check_job
//http://1.214.222.77/
//http://203.249.115.50:58090/

const int EEPROMBufSize = 64;

int server_port = 9305;
String deviceID = "A0001";

//ohmtide,0x90,0xA2,0xDA,0x0D,0xCC,0xE2,1,214,222,77,9305,A0001

//char serverName[] = "m.naver.com";
//IPAddress server_ip(203,249,115,50);
//const int server_port = 58090;

//
EthernetClient g_client;

String g_strCurrentStatus;
String g_Current_CommandCode;
String g_Current_NotifyID;

//unsigned long g_RequestCount;

void request_Server(EthernetClient *client,String &strReq) {
  
  IPAddress server_ip(ip_address[0],ip_address[1],ip_address[2],ip_address[3]);  
  
  if (client->connect(server_ip, server_port)) {
    
    Serial.println("connection success");
    
    client->println(strReq);
    client->println("Host: www.goorumefactory.co.kr");
    client->println("User-Agent: arduino-ethernet-ohmtide");
    client->println("Connection: close");
    client->println();
    
  } 
  else {
    // kf you didn't get a connection to the server:
    Serial.println("connection failed");
    g_strCurrentStatus = "connection_failed";
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




//implement console shell interface 
void processShell()
{
  char buf[EEPROMBufSize];
  byte read_size;
  
  if(Serial.available() > 0) {
    
    
    read_size = Serial.readBytesUntil('\r',buf,EEPROMBufSize);
    
    if(read_size <= 0 || read_size > EEPROMBufSize) {
      Serial.println(String("pasre error : cmd size ") + read_size);
      return;
    }
    else {
      buf[read_size] = 0x00;
      if(buf[read_size-1] == '\n')
        buf[read_size-1] = 0x00;
        
    }
    
    String strCmd = buf;
    
    if(strCmd == "info") {
//      Serial.println(String("status :") + g_strCurrentStatus);
      Serial.print("mac : ");
      for(int i=0;i<6;i++) {
        
        Serial.print(mac[i],HEX);
        Serial.print(",");
        
      }
      Serial.println("");
      
      Serial.print("ip address : ");
      for(int i=0;i<4;i++) {
        
        Serial.print(ip_address[i],DEC);
        Serial.print(",");
        
      }
      Serial.println("");
      
      Serial.println(String("port :") + server_port);
      Serial.println(String("id :") + deviceID);
      //Serial.println(String("network request count :") + g_RequestCount);
    }
    else if(strCmd == "cmd") {
      Serial.println("");
      Serial.println("***");
      g_strCurrentStatus = "process_menu";
      
    }
    else if(strCmd == "run") {
      Serial.println("");
      Serial.println("resume running..");
      g_strCurrentStatus = "ready";
    }
    else if(strCmd == "reset") {
      for (int i = 0; i < 512; i++) {
        EEPROM.write(i, 0);
      }
      
      Serial.println("eeprom clear ok");
        
    }
    else if( strCmd.startsWith("ohmtide") ) {
      
      //char buf[EEPROMBufSize];
      
      //strCmd.toCharArray(buf,strCmd.length()+1);
      int i =0;
      for(i=0;i<read_size;i++) {
        Serial.println(buf[i]);
        
        EEPROM.write(i, buf[i]);
      }
      EEPROM.write(i, 0);
      
    }
    else {
      
      Serial.println(strCmd + " is syntex error");
    }
  }
  
}

//-----------------------------------------------------------

void setup() {
  
  
  //g_strCurrentStatus = "setup";
  
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
  //Serial.println(strtol("0xff",NULL,16),DEC);


//read setup infomation form EEPROM 
  {
    char buf[EEPROMBufSize];
    
    for(int i=0;i<EEPROMBufSize;i++) {
      
      buf[i] = EEPROM.read(i);
    }
    
    
    String data = String(buf);
    
    
    //Serial.println(data);
    
    int startIndex = 0;
    int lastIndex = data.indexOf(',');
    String strToken =  data.substring(startIndex,lastIndex);
    //Serial.println(strToken);
    
    //eeprom have data
    if(strToken == "ohmtide") {
      
      //get mac address
      for(int i=0;i<6;i++) {
        startIndex = lastIndex+1;
        lastIndex = data.indexOf(',',startIndex);
        strToken =  data.substring(startIndex,lastIndex);
        
        char tempBuf[5];
        strToken.toCharArray(tempBuf,5);
        mac[i] = strtol(tempBuf,NULL,16);
      }
      
      //get ip address
      for(int i=0;i<4;i++) {
        startIndex = lastIndex+1;
        lastIndex = data.indexOf(',',startIndex);
        strToken =  data.substring(startIndex,lastIndex);
        ip_address[i] = strToken.toInt();
      }
      
      //get port
      startIndex = lastIndex+1;
      lastIndex = data.indexOf(',',startIndex);
      strToken =  data.substring(startIndex,lastIndex);
      server_port = strToken.toInt();
      
      //get device_id
      startIndex = lastIndex+1;
      lastIndex = data.indexOf(',',startIndex);
      strToken =  data.substring(startIndex,lastIndex);
      deviceID = strToken;
      
      
      Serial.println("start m-os v0.3");
    // start the Ethernet connection:
      if (Ethernet.begin(mac) == 0) {
        Serial.println("Failed to configure Ethernet using DHCP");
        // no point in carrying on, so do nothing forevermore:
        while(true);
      }
      
      g_strCurrentStatus = "ready";
      Serial.println("setup ok");
      
    }
    else {
      g_strCurrentStatus = "process_menu";
      Serial.println("please setup");
    }
    
  }  
}


void loop()
{
  
  processShell();
  
  if(g_strCurrentStatus == "process_menu") {
    
  }
  else {
    delay(1000);
  }
  
  if(g_strCurrentStatus == "process_pop") {
    
    //////////
    if (!g_client.connected()) {
      
      g_client.stop();
    
      String strReq = "GET /";
      strReq += "pop?device_id=";
      strReq += deviceID;
      strReq += " HTTP/1.1";
      Serial.print("request server :");
      Serial.println( strReq);
      request_Server(&g_client,strReq);    
    }
    else {
      //parseCmd(&g_client);
      Serial.println("wait respond server....");

      
      EthernetClient *client = &g_client;
      
      //wait for repspond before 3sec 
      client->setTimeout(3000);
      
      String strResult = ADJson_getData(client,"result");
      
      Serial.print("server respond :");    
      
      if(strResult == "ok") {
          
          //String ID = ADJson_getData(client,"device_id");
          String commandCode = ADJson_getData(client,"command_code");
          String notifyID = ADJson_getData(client,"notify_id");
          //Serial.println(ID);
          Serial.println(commandCode);
          Serial.println(notifyID);
          
          g_Current_CommandCode = commandCode;
          g_Current_NotifyID = notifyID;
          
          g_strCurrentStatus = "process_push";
        }
        else {
          //g_strCurrentStatus = "peek";
          Serial.println(strResult);
        }
      
      client->flush();
    }
  }
  else if(g_strCurrentStatus == "process_push") {
    
    if (!g_client.connected()) {
      
      g_client.stop();
      
      g_Current_CommandCode = g_Current_CommandCode.substring(g_Current_CommandCode.indexOf('!')+1,g_Current_CommandCode.indexOf('.'));
      Serial.println(g_Current_CommandCode);
      g_Current_CommandCode.toCharArray(messageBuffer,12);
        
      String strNotify =  process();
      
      String strReq = "GET /";
      strReq += "push_notify?";
      //strReq += deviceID;
      strReq += "notify_id=";
      strReq += g_Current_NotifyID;
      strReq += "&notify=";
      strReq += strNotify;
      strReq += " HTTP/1.1";
      
      Serial.print("request server :");
      Serial.println( strReq);
      
      request_Server(&g_client,strReq);  
    }
    else {
      
      //reset status
      
      Serial.println("wait respond server....");
      EthernetClient *client = &g_client;
      
      //wait for repspond before 3sec 
      client->setTimeout(3000);
      
      String strResult = ADJson_getData(client,"result");
      
      Serial.print("server respond :");    
      
      Serial.println(strResult);
      
      client->flush();
      g_strCurrentStatus = "ready";
      
    }
    
  }
  else if(g_strCurrentStatus == "ready") {
    Serial.println("ready...");
    g_strCurrentStatus = "process_pop";
  }
  else if(g_strCurrentStatus == "connection_failed") {
    Serial.println("connection failed retry...");
    g_strCurrentStatus = "ready";
  }
  else if(g_strCurrentStatus == "process_menu") {

  }
  else {
    
    Serial.println(g_strCurrentStatus);
    Serial.println("nop");
  }
  
}



