//#include <enc28j60.h>
#include <EtherCard.h>
//#include <net.h>

// ethernet interface mac address, must be unique on the LAN
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };
static byte myip[] = { 192,168,0,203 };
 
byte Ethernet::buffer[500];
BufferFiller bfill;
 
void setup () {
  
  Serial.begin(9600);
  
  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0) {
    Serial.println( "Failed to access Ethernet controller");
  }
  else {
    Serial.println("success to access Ethernet controller");
  }
  
  ether.hisport = 8080;
  
  ether.staticSetup(myip);
}
 
static word homePage() {
  /*
  long t = millis() / 1000;
  word h = t / 3600;
  byte m = (t / 60) % 60;
  byte s = t % 60;
  bfill = ether.tcpOffset();
  bfill.emit_p(PSTR(
    "HTTP/1.0 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Pragma: no-cache\r\n"
    "\r\n"
    //"<meta http-equiv='refresh' content='1'/>"
    "<title>hello</title>"
    "<h1>$D$D:$D$D:$D$D</h1>"),
      h/10, h%10, m/10, m%10, s/10, s%10);
      */
      bfill = ether.tcpOffset();
      bfill.emit_p(PSTR(
  
  "HTTP/1.0 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Pragma: no-cache\r\n"
    "\r\n"
    //"<meta http-equiv='refresh' content='1'/>"
    "<html>"
    "<header><title>hello</title></header>"
    "<body><h1>Hello enc28j80</h1></body>"
    "</html>"
      )

    );
    
    return bfill.position();
}
 
void loop () {
  word len = ether.packetReceive();
  word pos = ether.packetLoop(len);
  
 
  if (pos)  // check if valid tcp data is received
  {
    Serial.println("--------------------");
    Serial.println((char *)(Ethernet::buffer+pos));
    Serial.println("--------------------");
    
    //ether.packetLoop(ether.packetReceive());
    
    ether.httpServerReply(homePage()); // send web page data
    
    //flush 
    while( ether.packetLoop(ether.packetReceive()) == 0);
  }
    
}
