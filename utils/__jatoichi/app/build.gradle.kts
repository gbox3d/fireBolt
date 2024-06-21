plugins {
    // Apply the application plugin to add support for building a CLI application in Java.
    application

    // Apply the JavaFX plugin
    id("org.openjfx.javafxplugin") version "0.1.0"
}

repositories {
    // Use Maven Central for resolving dependencies.
    mavenCentral()
}

java {
    toolchain {
        languageVersion.set(JavaLanguageVersion.of(21))
    }
}

tasks.withType<JavaCompile> {
    options.release.set(21)
}

tasks.withType<JavaCompile> {
    sourceCompatibility = "21"
    targetCompatibility = "21"
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

javafx {
    version = "22.0.1"
    modules = listOf("javafx.controls", "javafx.fxml")
}

application {
    // Define the main class for the application.
    mainClass.set("jatoichi.App")
}

tasks.withType<JavaCompile> {
    options.encoding = "UTF-8"
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
    into(layout.buildDirectory.dir("libs"))
}

tasks.withType<Tar> {
    duplicatesStrategy = DuplicatesStrategy.EXCLUDE
    dependsOn("copyLibs")
    from(layout.buildDirectory.dir("libs")) {
        into("lib")
    }
}

tasks.withType<Zip> {
    duplicatesStrategy = DuplicatesStrategy.EXCLUDE
    dependsOn("copyLibs")
    from(layout.buildDirectory.dir("libs")) {
        into("lib")
    }
}

tasks.withType<Jar> {
    duplicatesStrategy = DuplicatesStrategy.EXCLUDE
}




