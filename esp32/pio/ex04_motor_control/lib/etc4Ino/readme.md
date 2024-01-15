# 아두이노 프로그래밍을 위한 도구모듈 모음

## processSerialCmder

함수원형 : 
```cpp
bool processSerialCmder(void (*_doProcess)(String));
```

설명: 

processSerialCmder 모듈은 아두이노에서 시리얼 통신을 사용하여 명령어를 입력받고 처리하는 기능을 제공합니다. 이 모듈은 사용자가 시리얼 모니터(또는 다른 시리얼 통신 방법)를 통해 텍스트 명령어를 전송할 때 활용됩니다. 주요 특징은 다음과 같습니다:

Non-blocking 동작: 이 모듈은 non-blocking 방식으로 동작하여, 시리얼 데이터를 기다리는 동안 다른 작업을 수행할 수 있도록 합니다. 이는 프로그램의 효율성을 높이며, 리소스를 절약하는 데 도움이 됩니다.

종료 문자에 의한 입력 종료: 사용자가 입력하는 문자열의 끝은 '\n' 문자로 표시됩니다. 이 문자를 받으면 모듈은 현재까지 받은 문자열을 완전한 명령어로 간주하고 처리를 시작합니다.

라인별 처리: 사용자가 입력한 각 라인은 _doProcess 함수를 통해 처리됩니다. 이 함수는 사용자가 별도로 정의해야 하며, 각 라인에 대한 사용자 지정 처리 로직을 포함합니다.


사용예제 : 
```cpp
void myCommandProcessor(String command) {
    // 여기에 명령어 처리 로직을 작성합니다.
    Serial.println("Received command: " + command);
}

void setup() {
    Serial.begin(9600);
}

void loop() {
    processSerialCmder(myCommandProcessor);
}
```