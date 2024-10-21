## 주요 설정 값 정리

```c++
    String ssid = g_config.get<String>("ssid", "redstar0427");
    String password = g_config.get<String>("password", "123456789a");

    g_ApiUrl = g_config.get<String>("api_url", "http://gears001.iptime.org:21037");
    g_restCallTerm = g_config.get<int>("rest_call_term", 10000);
    g_LogID = g_config.get<String>("log_id", "67136f184d4d7e4e6371a943");

    Serial.println("try to connect WiFi.....");
    Serial.printf("ssid: %s\n", ssid.c_str());
    Serial.printf("password: %s\n", password.c_str());

    Serial.printf("API URL: %s\n", g_ApiUrl.c_str());
    Serial.printf("REST Call Term: %d\n", g_restCallTerm);
    Serial.printf("Log ID: %s\n", g_LogID.c_str());

```


위의 소스코드에서 나온것처럼 설정값을 읽어오는 부분이다.   

ssid, password, api_url, rest_call_term, log_id 등을 설정값으로 사용하고 있다.  

ssid : 와이파이 ssid  
password : 와이파이 비밀번호  
api_url : 서버 주소<br>
rest_call_term : 서버에 데이터를 보내는 주기<br>
log_id : 할당된 로그 데이터 유닛 아이디<br>
