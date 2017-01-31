snqbW
===
snqbW 는 nodemcu 가지고 만든 network bootloader 입니다.<br>
최소한의 기본 기능만을 제공하고 나머지는 동적으로 확장 가능 하도록 했습니다.

1999번 포트로 기본 정보(ip주소)를 브로드 캐스팅을 합니다.<br>
동시에 같은 포트로 데이터를 송수신합니다.

config.lua 에는 기본 설정 정보가 들어가며 수정할 수 없습니다.<br>
대신에, config.json에 현재설정 정보를 저장합니다. 그리고 이것은 수정가능합니다.

기본으로 제공되는 패킷은 "eval", "stat" 입니다.

형식은 다음과 같습니다.

{cmd:"eval",code:" 루아코드 스트링 "}

"eval" 은 원격으로 루아 코드를 실행시켜줍니다.<br>
 이것으로 많은 확장을 할 수 있습니다.
 
새로운 패킷에 대한 처리는 packet_dic 에 함수를 추가하여 처리합니다.

```
packet_dic["new packet name"] = function(packet) .. 처리내용 ..  end 
```


boot_status는 status.json에 저장됩니다.<br>
boot_status.process 는 부트 초기에는 "startup" 입니다.<br>
성공적으로 부트가 완료되면 APOK,STOK 둘중 하나가 되고 status.json에 저장합니다.<br>
boot_status.mode 는 normal 이 됩니다.

현재의 부트상태를 저장하고 싶다면 다음과 같이 합니다.

```
boot_status.process = "현재상태"
saveStatus()
```

잘못된 코드가 반복적으로 호출되지 않도록 하기 위하여 boot_status.process값에 따라서 부팅과정을 조절합니다.<br>
루아펌로딩후 boot_status.process 가 startup 상태이면 부팅을 멈추고 nook로 상태를 바꾸어 저장합니다.<br>
check_stub( boot_status.mode 는 check) 이면 ext_main 함수(ext.lua) 콜을 유보합니다.(네트웍 기능은 정상작동)<br>

STOK,APOK 이면 정상 동작을 수행합니다.

현재상태가 정상동작 상태(normal)인지 체크부트(check) 상태인지 알아보기 위해서는 다음과 같은 코드로 확인합니다.

```
rt ={type="rs",id=0,run_mode=boot_status.mode} 
udp_safe_sender(cjson.encode(rt),2012,"192.168.10.107") 
```

ext.lua 는 확장을 위한 루아 파일입니다.<br>
지속적으로 추가를 원하는 기능은 여기에 코드를 써줍니다

### 유용한 전역변수

last_nt_tick : 마지막 네트웍 응답시간을 얻는다.

rt : 리턴값만들기용 전역 변수

네트웍응답 지연시간을 다음과 같이 얻을수있다.
```
tmr.now()-last_nt_tick
```


예제 >

0번 gpio 끄기

```
{
cmd :"eval",
code : "gpio.mode(0,1)gpio.write(0,0)"
}
```
0번 gpio 켜기
```
{
cmd :"eval",
code : "gpio.mode(0,1)gpio.write(0,1)"
}
```
브로드 캐스팅 끄기
```
{
cmd : "eval",
code : "stopUdpCast()"
}
```
브로드 캐스팅 켜기
```
{
cmd : "eval",
code : "startUdpCast()"
}
```
echo 패킷추가
```
{
cmd:"eval",
code : "packet_dic[\"echo\"] = function(packet) local rt = {type=\"rs\",result=\"echo\",msg=packet.msg} udp_server:send(cjson.encode(rt))  end"
}

```
