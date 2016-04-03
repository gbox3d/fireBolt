#include <Servo.h>

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

const String App_Version = "0.1.0";
bool debug = false;

int index = 0;

char messageBuffer[12];
char cmd[3];
char pin[3];
char val[4];
char aux[4];

////////////
#define MAX_SERVO 32
Servo *servo[MAX_SERVO];

void setup() {
  //Serial.begin(115200);
  Serial.begin(9600);
  
  for(int i=0;i<MAX_SERVO;i++)
  {
    servo[i] = NULL;
  }
  
  Serial.println("start :" + App_Version);
  
  
}

void loop() {
  while(Serial.available() > 0) {
    char x = Serial.read();
    if (x == '!') index = 0;      // start
    else if (x == '.') process(); // end
    else messageBuffer[index++] = x;
  }
}
/*

get pin number 'AXX code' for annalog

*/
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
 * Deal with a full message and determine function to call
 */
void process() {
  index = 0;

  strncpy(cmd, messageBuffer, 2);
  cmd[2] = '\0';
  strncpy(pin, messageBuffer + 2, 2);
  pin[2] = '\0';

  int cmdid = atoi(cmd);

  if (debug) {
    Serial.println(messageBuffer);
  }
  
  strncpy(val, messageBuffer + 4, 2);
  val[2] = '\0';
      

  switch(cmdid) {
    case 0:  
      {
        //set mode
        strncpy(val, messageBuffer + 4, 1);
        val[1] = '\0';
        sm(pin,val);              
      }
      break;
    case 1:  
    {
      //digital write
      strncpy(val, messageBuffer + 4, 3);
      val[3] = '\0';
      dw(pin,val);              
    }
      
      break;
    case 2:  
    {
      //digital read
        dr(pin);              
    }
      break;
    case 3:  
    {
      //analog write
      strncpy(val, messageBuffer + 4, 3);
      val[3] = '\0';
      aw(pin,val);              
    }
      break;
    case 4:  
    {
      ar(pin);              
    }
      break;
    case 97: 
    {
      //ping
      strncpy(val, messageBuffer + 4, 3);
        val[3] = '\0';
        strncpy(aux, messageBuffer + 7, 3);
        aux[3] = '\0';
      handlePing(pin,val,aux);  
    }
      break;
    case 98: 
    {
      //servo cmd value
      strncpy(val, messageBuffer + 4, 1);
      val[1] = '\0';
      
      int cmd_val = atoi(val);
      int aux_val = 0;
      
      switch(cmd_val) {
        case 2:
          strncpy(aux, messageBuffer + 5, 3);
          aux[3] = '\0';
          aux_val = atoi(aux);
        break;
        default:
          

        break;
      }
    
      handleServo(getPin(pin),cmd_val,aux_val); 
    }
    
    
    break;
    case 99: toggleDebug();         break;
    default:                           break;
  }
}

/*
 * Toggle debug mode
 */
void toggleDebug() {
  if (debug == true) {
    debug = false;
    Serial.println("dbg end");
  } else {
    debug = true;
    Serial.print("dbg start");
  }
}

/*
 * Set pin mode
 */
void sm(char *pin, char *val) {
  if (debug) Serial.println("sm");
  int p = getPin(pin);
  if(p == -1) { if(debug) Serial.println("badpin"); 
  //error
    Serial.println("!01."); 
    return; 
  }
  if (atoi(val) == 0) {
    pinMode(p, OUTPUT);
  } else {
    pinMode(p, INPUT);
  }
  
  //success
  Serial.print("!00."); 
  
}

/*
 * Digital write
 */
void dw(char *pin, char *val) {
  if (debug) Serial.println("dw");
  int p = getPin(pin);
  if(p == -1) { 
    
   if(debug) Serial.println("badpin");
   Serial.print("!01."); 
    
    return; 
  }
  pinMode(p, OUTPUT);
  if (atoi(val) == 0) {
    digitalWrite(p, LOW);
  } else {
    digitalWrite(p, HIGH);
  }
  
  //success
  //delay(10);
  Serial.print("!00."); 
  
  
}

/*
 * Digital read
 */
void dr(char *pin) {
  if (debug) Serial.println("dr");
  int p = getPin(pin);
  if(p == -1) { if(debug) Serial.println("badpin"); return; }
  pinMode(p, INPUT);
  int oraw = digitalRead(p);
  char m[7];
  sprintf(m, "#%02d.",oraw);
  Serial.print(m);
}

/*
 * Analog read
 */
void ar(char *pin) {
  if(debug) Serial.println("ar");
  int p = getPin(pin);
  if(p == -1) { if(debug) Serial.println("badpin"); return; }
  pinMode(p, INPUT); // don't want to sw
  int rval = analogRead(p);
  char m[8];
  sprintf(m, "#%03d.", pin, rval);
  Serial.print(m);
}

void aw(char *pin, char *val) {
  if(debug) Serial.println("aw");
  int p = getPin(pin);
  pinMode(p, OUTPUT);
  if(p == -1) { if(debug) Serial.println("badpin"); Serial.println("!01."); return; }
  analogWrite(p,atoi(val));
  Serial.print("!00.");
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
void handleServo(int pin, int val, int aux) {
  if (debug) Serial.println("servo command start");
  //int p = getPin(pin);
  int p = pin;
  if(p == -1) { if(debug) Serial.println("badpin"); return; }
  //Serial.println("signal: servo");

  // 00(0) Detach
  if (val == 0) {
    
    servo[p]->detach();
    
    delete servo[p];
    servo[p] = NULL;
    
    Serial.print("!00."); 
    if(debug) {
      Serial.println("servo dettach");
    }

  // 01(1) Attach
  } else if (val == 1) {
    // servo.attach(p, 750, 2250);
    if(debug) {
      Serial.println("servo attach");
    }
    
    if(servo[p] == NULL) {
      servo[p] = new Servo();
      
      if(debug) {
        Serial.print("new servo :");
        Serial.println(p);
      }
      
    }
    
    servo[p]->attach(p);
    Serial.print("!00."); 

  // 02(2) Write
  } else if (val == 2) {
    servo[p]->write(aux);
    
    //success
    Serial.print("!00."); 
    
    if(debug) {
      Serial.println("servo write");
    }
    
    // TODO: Experiment with microsecond pulses
    // digitalWrite(pin, HIGH);   // start the pulse
    // delayMicroseconds(pulseWidth);  // pulse width
    // digitalWrite(pin, LOW);    // stop the pulse

  // 03(3) Read
  } else if (val == 3) {
    //Serial.println("reading servo");
    int sval = servo[p]->read();
    char m[13];
    sprintf(m, "#%03d.", sval);
    Serial.print(m);
  }
  
  if (debug) Serial.println("servo command end");
  
}
