1> 데이터 출력전용포트
5638 포트로 데이터를 전송한다.

2> 설정입출력 & 데이터입력 포트
1471 포트로 데이터를입력받는다.

-입력상세 설명

json 포멧사용함
json 형식의 패킷구조는 다음과 같음.
{
    id : 패킷아이디,
    cmd : [ping|save|reboot|uart|uart_bin]
}

- cmd

1) save
이름과파일내용을넘겨주어 플레쉬 메모리에 파일로 저장하도록 한다.
save 명령어의 따라오는 인자값들은 파일이름 name, 파일내용 content 임

2)uart
data :데이터스트링(0x7f까지의 문자만 가능)
예>"hello"

3)uart_bin
data : 데이터 배열, 0x7f 이상의 바이트 데이터도 가능
예>[23,88,9f,f7]

4)to-ip
데이터를 전송할 아이피 변경
ip : 데이터를 받을 아이피


3>기본 AP
GUNPOWER_PI_1

자체 AP모드
espudp/12345678
