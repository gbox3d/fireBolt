/*
 * This file was generated by the Gradle 'init' task.
 *
 * This generated file contains a sample Java application project to get you started.
 * For more details take a look at the 'Building Java & JVM projects' chapter in the Gradle
 * User Manual available at https://docs.gradle.org/8.0.2/userguide/building_java_projects.html
 */

plugins {
    // Apply the application plugin to add support for building a CLI application in Java.
    application
}

repositories {
    // Use Maven Central for resolving dependencies.
    mavenCentral()
}

dependencies {
    // Use JUnit Jupiter for testing.
    testImplementation("org.junit.jupiter:junit-jupiter:5.9.1")

    // This dependency is used by the application.
    implementation("com.google.guava:guava:31.1-jre")

    // jSerialComm 라이브러리 추가
    implementation("com.fazecast:jSerialComm:2.11.0") // 최신 버전을 사용하세요.
    implementation("org.json:json:20240303") // 최신 버전을 사용하세요.
}

application {
    // Define the main class for the application.
    mainClass.set("jatoichi.App")
}

tasks.named<Test>("test") {
    // Use JUnit Platform for unit tests.
    useJUnitPlatform()
}

// 디버깅을 위한 설정
tasks.withType<JavaExec> {
    jvmArgs = listOf("-agentlib:jdwp=transport=dt_socket,server=y,suspend=n,address=*:5005")
}


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