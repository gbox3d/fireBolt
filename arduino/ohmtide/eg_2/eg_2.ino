// Sweep
// by BARRAGAN <http://barraganstudio.com> 
// This example code is in the public domain.
/*

example>
!0902000090.
09 servo
02 write
000 portnumber
090 angle
*/

#include <Servo.h> 
#include <SoftwareSerial.h>
 
#define MAX_SERVO 3

SoftwareSerial mySerial(2, 3); // RX, TX

Servo myservo[MAX_SERVO];
byte servo_port[MAX_SERVO] = {9,10,11}; //6,5,3

 
void setup() 
{ 
  for(int i=0;i<MAX_SERVO;i++) {
    
    myservo[i].attach(servo_port[i]);
    myservo[i].write(5);
  }
  
  mySerial.begin(9600);
  Serial.begin(9600);
  
  myservo[0].write( 5 );
  
} 
 
int index = 0;
char messageBuffer[12];
char dev[3]; //device
char cmd[3]; // command
char op1[4]; // option 1
char op2[4]; // option 2

//[dev][command][op1][op2]
//0902001090
 
void loop() 
{ 
  
  //Serial.print("hello");
  //while(mySerial.available() > 0) {
    //char x = mySerial.read();
    while(Serial.available() > 0) {
       char x = Serial.read();
    
    //Serial.print(x);
    
    if (x == '!') index = 0;      // start
    else if (x == '.') {
      index = 0;
      process(); // end
    }
    else messageBuffer[index++] = x;
  }
  
  
}


void process() {
  
  
  
  strncpy(dev, messageBuffer, 2);
  dev[2] = '\0';
  strncpy(cmd, messageBuffer + 2, 2);
  cmd[2] = '\0';
  strncpy(op1, messageBuffer + 4, 3);
  op1[3] = '\0';
  strncpy(op2, messageBuffer + 7, 3);
  op2[3] = '\0';
  
  int dev_id = atoi(dev);
  
  Serial.println(dev_id);
  Serial.println(cmd);
  Serial.println(op1);
  Serial.println(op2);
  
  if(dev_id == 9) {
    switch(atoi(cmd)) {
      case 1://attach
      break;
      case 2://write
      myservo[atoi(op1)].write( atoi(op2) );
      break;
      case 3://read
      //myservo[atoi(op1)].read( atoi(op2) );
      break;
      case 4://dettach:
      break;
    }
  }
  
}
  
  /*
  myservo.write(5);              // tell servo to go to position in variable 'pos' 
  myservo2.write(5);
  delay(1500);                       // waits 15ms for the servo to reach the position 
  
  myservo.write(180);              // tell servo to go to position in variable 'pos' 
  myservo2.write(180);
  delay(1500);                       // waits 15ms for the servo to reach the position 
  */
  


