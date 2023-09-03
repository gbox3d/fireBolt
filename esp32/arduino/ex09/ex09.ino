#include <WiFi.h>
#include <WiFiUdp.h>
#include <MPU9250_asukiaaa.h>

const char* ssid = "redstar_mc0815";
const char* password = "123456789u";
const char* udpAddress = "192.168.4.71";  // 실제 서버의 IP를 입력하세요
const int udpPort = 9250;  // 실제 서버의 포트를 입력하세요

WiFiUDP udp;
MPU9250_asukiaaa mySensor;

struct S_Udp_Header_Res_Packet
{
    uint32_t checkCode;
    uint8_t cmd;
    uint8_t parm[3];
};

struct S_Udp_IMU_Angle_Packet
{
    //header
    S_Udp_Header_Res_Packet header;

    //angle data
    float roll;
    float pitch;
    float yaw;
};

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  mySensor.beginAccel();
}

void loop() {
  if (mySensor.accelUpdate() == 0) {
    float aX = mySensor.accelX();
    float aY = mySensor.accelY();
    float aZ = mySensor.accelZ();

    // 각도 계산
    float roll = atan2(aY, aZ) * 180 / PI;
    float pitch = atan2(aX, aZ) * 180 / PI;
    float yaw = 0;  // yaw는 이 예제에서는 계산하지 않습니다.

    // 패킷 생성 및 전송
    S_Udp_IMU_Angle_Packet packet;
    packet.header.checkCode = 0x12345678;  // 예시 체크 코드
    packet.header.cmd = 0x01;  // 예시 명령어
    packet.header.parm[0] = 0;
    packet.header.parm[1] = 0;
    packet.header.parm[2] = 0;
    packet.roll = roll;
    packet.pitch = pitch;
    packet.yaw = yaw;

    sendDataToServer(packet);
  }

  delay(1000);
}

void sendDataToServer(S_Udp_IMU_Angle_Packet& packet) {
  udp.beginPacket(udpAddress, udpPort);
  udp.write((uint8_t*)&packet, sizeof(S_Udp_IMU_Angle_Packet));
  udp.endPacket();
}
