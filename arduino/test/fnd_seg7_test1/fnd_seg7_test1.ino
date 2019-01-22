#include <Arduino.h>

int _7segFont[] = {
  0b00000011, //0
  0b10011111, //1
  0b00100101, //2
  0b00001101, //3
  0b10011001, //4
  0b01001001, //5
  0b01000001, //6
  0b00011111, //7
  0b00000001, //8
  0b00001001,  //9
  0b11111111 //space , ':'
};

#define DIGIT_NUM 4

int pinMapLed[8] = {9,8,7,6,5,4,3,2};
int pinMapSelector[DIGIT_NUM] = {10,11,12,13};


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  for(int i=0;i<7;i++) {
    pinMode(pinMapLed[i],OUTPUT);
    digitalWrite(pinMapLed[i],HIGH);    
  }

  for(int i=0;i<DIGIT_NUM;i++) {
    pinMode(pinMapSelector[i],OUTPUT);
    digitalWrite(pinMapSelector[i],LOW);
  }

  //digitalWrite(pinMapSelector[0],HIGH);
  //putDigit(1);

  digitalWrite(pinMapSelector[2],HIGH);
  putDigit(1);  
  //delay(1);
  //digitalWrite(pinMapSelector[2],LOW);
  

}

void loop() {
  // put your main code here, to run repeatedly:
  //testBasic(0 );

  /*
  digitalWrite(pinMapSelector[1],HIGH);
  putDigit(2);
  delay(1);
  digitalWrite(pinMapSelector[1],LOW);
  
  digitalWrite(pinMapSelector[0],HIGH);
  putDigit(3);
  delay(1);
  digitalWrite(pinMapSelector[0],LOW);
  */

}

void putDigit( int num  )
{
  
  byte val = _7segFont[num];
  for(int i=0;i<7;i++) {
    if( !(val & 0b10000000) ) {
      digitalWrite(pinMapLed[i],LOW);
    }
    else {
      digitalWrite(pinMapLed[i],HIGH);
    }
    val = val << 1;
  }
}



void testBasic(int nSel)
{
  
  digitalWrite(pinMapSelector[nSel],HIGH);

  for(int i=0;i<8;i++) {    
    digitalWrite(pinMapLed[i],LOW);    
  }
  delay(500);
  for(int i=0;i<8;i++) {    
    digitalWrite(pinMapLed[i],HIGH);    
  }
  delay(500);
  
  for(int i=0;i<8;i++) {
    
    digitalWrite(pinMapLed[i],LOW);
    delay(300);
    digitalWrite(pinMapLed[i],HIGH);    
    delay(300);
  }
  
  digitalWrite(pinMapSelector[nSel],LOW);
}

