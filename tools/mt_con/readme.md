
## 패킷 보내기 예
JsonData jsonObj = new JsonData();
jsonObj["cmd"] = "eval";
jsonObj["code"] = strCode;
sendString(jsonObj.ToJson());
