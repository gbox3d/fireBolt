
- uart 보내기 예제
var msg = JSON.stringify({cmd:'uart_bin',data:'hello'});


- uart_bin 보내기 예제
var msg = JSON.stringify({cmd:'uart_bin',data:[48,49]});

nodejs 용 버퍼 보내기 예>
-------------
var packet = buff.toJSON(); //json형식으로 바꾸기
packet.cmd = 'uart_bin';
packet = JSON.stringify(packet); //직렬화
udp_socket.send(
    packet, 0, packet.length,
    1471,'192.168.42.22' );
-------------