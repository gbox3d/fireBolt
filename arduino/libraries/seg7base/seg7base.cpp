#include <Arduino.h>
#include "seg7base.h"

/*
 * 인펀트리 FND 제어 모듈 
 */
#define DIGIT_NUM 3

//int pinMapLed[8] = {13,12,11,10,9,8,7,6};
//int pinMapSelector[DIGIT_NUM] = {3,4,5};

// int pinMapLed[8] = {7,8,12,13,4,5,6,8};
// int pinMapSelector[DIGIT_NUM] = {9,10,3};


//318-A
int pinMapLed[8] = {7,8,12,13,14,5,6,8};
int pinMapSelector[DIGIT_NUM] = {9,10,15};


int _7segFont[] = {
  0b00000011,//0
  0b10011111,//1
  0b00100101, //2
  0b00001101, //3
  0b10011001, //4
  0b01001001, //5
  0b01000001, //6
  0b00011111, //7
  0b00000001, //8
  0b00001001  //9
};

char g_renderDigits[DIGIT_NUM] = {'0','0','0'};

void setupFND()
{
  // put your setup code here, to run once:
  for(int i=0;i<8;i++) {
    pinMode(pinMapLed[i],OUTPUT);
    digitalWrite(pinMapLed[i],HIGH);    
  }

  for(int i=0;i<DIGIT_NUM;i++) {
    pinMode(pinMapSelector[i],OUTPUT);
    digitalWrite(pinMapSelector[i],LOW);
  }
//아날로그 핀 제어 
  //analogWrite(A0,0);
  //analogWrite(A1,255);
  //Serial.println(A0);
}

void setupFND(int pinMap[])
{
  // put your setup code here, to run once:
  for(int i=0;i<8;i++) {
    pinMapLed[i] = pinMap[i];
  }

  for(int i=0;i<DIGIT_NUM;i++) {
    pinMapSelector[i] = pinMap[i+8];
    
  }
  setupFND();
}


void setRenderDigit_Number(int number)
{
  String digit = String(number,DEC);
  
  for(int i=digit.length();i<DIGIT_NUM;i++) {
    digit = '0' + digit;
  }

  for(int i=0;i<digit.length();i++) {
    g_renderDigits[ (DIGIT_NUM-1)-i] = digit[i];
  }
}

void putDigit(int nDigit,int num  )
{  
  byte val = _7segFont[num];
  for(int i=0;i<7;i++) {
    byte pinVal = pinMapLed[i];
    if( !(val & 0b10000000) ) {
      // if(pinVal >= 14) {
      //   digitalWrite(pinVal,0);
      // }
      // else {
      //   digitalWrite(pinVal,LOW);
      // }
      digitalWrite(pinVal,LOW);
      
    }
    else {
      // if(pinVal >= 14) {
      //   digitalWrite(pinVal,255);
      // }
      // else digitalWrite(pinVal,HIGH);

      digitalWrite(pinVal,HIGH);
    }
    val = val << 1;
  }

  if(pinMapSelector[nDigit] >= 100) {
    analogWrite(A1,255);
  }
  else digitalWrite(pinMapSelector[nDigit],HIGH);
  //delayMicroseconds(300); 
  delay(1);

  for(int i=0;i<DIGIT_NUM;i++) {
    if(pinMapSelector[nDigit] >= 100) {
      analogWrite(A1,0);
    }
    else {
      digitalWrite(pinMapSelector[i],LOW);
    }    
    
  }

  
}

void renderSegment()
{
  for(int i=0;i<DIGIT_NUM;i++) {
    putDigit(i, g_renderDigits[i] - '0' );
  }
  //Serial.println(digit);
  
}

//--------------테스트 함수 

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


void testDigit()
{ 

  putDigit(0,1);
  delay(10);
  putDigit(1,2);
  delay(10);
  
}
