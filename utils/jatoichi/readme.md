# Jatoichi (Java TTL Configuration Helper Instrument)

JATOICHI is short word for JAva Ttl cOnfIguration Helper Instrument.   


## Dependencies

serial 통신과 json 을 위하여 다음과 같은 라이브러리를 사용한다.  
[jSerialComm](https://fazecast.github.io/jSerialComm/)  
[org.json](https://github.com/stleary/JSON-java?tab=readme-ov-file)

build.gradle.kts 파일에 다음과 같이 추가한다.  


```kotlin
dependencies {
    
    //기존 의존성 
    // ....


    // jSerialComm 라이브러리 추가
    implementation("com.fazecast:jSerialComm:2.11.0") // 최신 버전을 사용하세요.
    implementation("org.json:json:20240303") // 최신 버전을 사용하세요.
}
```

## build environment

jatoichi is built with gradle. To build the project, you need to have gradle installed on your system.  

upgrade gradle to the latest version.  

```shell
# sample for upgrading gradle to 8.8
gradle wrapper --gradle-version 8.8
```

build the project.  

```shell

# build the project
./gradlew build

# clean the project
./gradlew clean

# bulid clean
./gradlew clean build

```

## Development Environment(Debugging)

app/build.gradle.kts 파일에 다음과 같이 설정을 추가한다.    

```kotlin
// 5005 포트로 디버깅을 할 수 있도록 설정한다.  
tasks.withType<JavaExec> {
    jvmArgs = listOf("-agentlib:jdwp=transport=dt_socket,server=y,suspend=n,address=*:5005")
}

```

launch.json 파일을 생성하고 다음과 같이 설정한다.  

```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "type": "java",
            "name": "Debug (Attach)",
            "request": "attach",
            "hostName": "localhost",
            "port": 5005
        }
    ]
}
```

## Packaging

build.gradle.kts 파일에 다음과 같이 설정을 추가한다.  

```kotlin

//... 기존 설정
//...

// Distribution 설정
distributions {
    main {
        contents {
            from("src/main/resources") {
                into("resources")
            }
            from(tasks.named<Jar>("jar")) {
                into("lib")
            }
            from(configurations.runtimeClasspath) {
                into("lib")
            }
        }
    }
}

// Copy runtime dependencies to lib directory
tasks.register<Copy>("copyLibs") {
    from(configurations.runtimeClasspath)
    into("$buildDir/libs")
}

tasks.withType<Tar> {
    duplicatesStrategy = DuplicatesStrategy.EXCLUDE
    dependsOn("copyLibs")
    from("$buildDir/libs") {
        into("lib")
    }
}

tasks.withType<Zip> {
    duplicatesStrategy = DuplicatesStrategy.EXCLUDE
    dependsOn("copyLibs")
    from("$buildDir/libs") {
        into("lib")
    }
}
```

패키지를 생성한다.  

```shell
# create a distribution
./gradlew distTar
./gradlew distZip
```


