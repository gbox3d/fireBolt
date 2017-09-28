#include <Wire.h>

void setup()
{
  Wire.begin(8); // 자신의 주소를 4번으로 설정하고 i2c 버스에 참여
  Wire.onReceive(receiveEvent); // 수신 이벤트 함수 등록  
  Wire.onRequest(requestEvent);
  Serial.begin(115200);
  Serial.println("wait for...");
  
}

void loop()
{
  delay(100);
}

// 다음 함수는 마스터가 데이터를 전송해 올 때마다 호출됨
void receiveEvent(int howMany)
{
  while(0 < Wire.available()) 
  {
    
    char c = Wire.read(); 
    Serial.print(c);
  }
  //int x = Wire.read(); // byte를 읽어 int로 변환
  Serial.println(" --- end read");
  //delay(100);
  //
}

void requestEvent() {
  Wire.write("hello "); // respond with message of 6 bytes
}
