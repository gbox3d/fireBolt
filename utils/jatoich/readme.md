# Com port Communication Utility

## how to Compile

javaFX -> run  

## how to Build Release

javaFX -> JLink 를 이용하여 빌드  

결과물릉 target/image/ 로 출력된다.


실행을 위한 run.bash 는 다음과 같다.

```bash
#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
$DIR/bin/java -m com.bsq.jatoich/com.bsq.jatoich.App

```

