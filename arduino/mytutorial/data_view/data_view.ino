#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); // RX,TX

typedef struct {
  byte header; //1
  int a; //2
  float b; //4 
  byte temp[1]; //1
  byte extra;  //1
  byte footer; //1
} S_TEST;

S_TEST data = {0x80,1024,3.14,0x00,0x00,0x81};

//항상 크기는 10바이트 
void convertGPKV1(byte *pData,int packet_size)
{
  byte *temp = pData;

  int data_number = packet_size-2;
  int extra_byte_index = data_number;

  //Serial.println( "data number :" + String(data_number) );
  
  for(int i=1;i< data_number ;i++) {

    if(bitRead(temp[i],7)) {
      int setbit = 7-i;
      bitSet(temp[extra_byte_index],setbit);
      bitClear(temp[i],7);
      //Serial.println( "set bit :" + String(7-i));  
    }
    else {
      
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  delay(500);
  //Serial.println( "data size" + String(sizeof(data)));

  convertGPKV1((byte *)&data,sizeof(data));
  //for(int i=0;i<sizeof(data);i++) {    
    //Serial.println( String(temp[i]) + "," + String(bitRead(temp[i],7)) );
    //bitSet(temp[7],bitRead(temp[i],7) );
  //}

  Serial.write((byte *)&data,sizeof(data));
  
}

void loop() {
  // put your main code here, to run repeatedly:
  
  //Serial.write((byte *)&data,sizeof(data));
  
  delay(500);

}
