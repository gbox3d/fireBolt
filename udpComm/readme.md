# UDP Serial Communication Project for ESP8266

이 프로젝트는 ESP8266을 사용하여 시리얼 통신 내용을 UDP를 통해 네트워크로 전송하는 시스템을 구현합니다.

## 프로젝트 개요

- 목적: 시리얼 통신 데이터를 UDP를 통해 원격으로 전송 및 모니터링
- 대상 하드웨어: ESP8266
- 개발 환경: Arduino IDE (또는 PlatformIO)

## 주요 기능

1. WiFi 연결 및 관리
2. 시리얼 명령어 파싱 및 실행
3. UDP 통신을 통한 데이터 전송
4. 설정 관리 (EEPROM을 통한 저장 및 로드)
5. LED를 통한 시스템 상태 표시

## 사용된 라이브러리

- ESP8266WiFi
- TaskScheduler
- ArduinoJson
- ESPAsyncUDP

## 설정 방법

1. `config.hpp` 파일에서 WiFi SSID와 비밀번호 설정
2. UDP 포트 번호 설정 (기본값: 7204)

## 주요 명령어

- `about`: 시스템 정보 표시
- `reboot`: 시스템 재부팅
- `config`: 설정 관리 (load, save, dump, clear, set, get)

## 데이터 전송 형식

시리얼로 입력된 데이터 중 `!`로 시작하는 라인은 UDP를 통해 전송됩니다.

Example:
```plaintext
!Hello, World!
```

## LED 상태 표시

- 빠른 점멸: WiFi 연결 시도 중
- 켜짐: WiFi 연결 완료

## 향후 개선 사항

- Web 인터페이스 추가
- 보안 강화 (데이터 암호화)
- 다중 클라이언트 지원

## 기여 방법

프로젝트에 기여하고 싶으시다면, 풀 리퀘스트를 보내주시기 바랍니다. 버그 리포트나 기능 제안은 이슈 트래커를 이용해 주세요.

## 라이선스

이 프로젝트는 [라이선스 이름] 하에 배포됩니다. 자세한 내용은 LICENSE 파일을 참조하세요.