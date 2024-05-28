/*
 * This file was generated by the Gradle 'init' task.
 *
 * This generated file contains a sample Java application project to get you started.
 * For more details take a look at the 'Building Java & JVM projects' chapter in the Gradle
 * User Manual available at https://docs.gradle.org/8.1.1/userguide/building_java_projects.html
 */

import org.gradle.api.tasks.testing.logging.TestExceptionFormat
import org.gradle.api.tasks.testing.logging.TestLogEvent

plugins {
    // Apply the application plugin to add support for building a CLI application in Java.
    antlr
    java
    application
}

repositories {
    mavenLocal()

    // Use Maven Central for resolving dependencies.
    mavenCentral()

    // CUBRID JDBC
    var jdbc_path = project.properties["cubridJdbcPath"]
    if (jdbc_path != null) {
        // find cubrid-jdbc in local submodule path
        flatDir {
            dirs("$jdbc_path")
        }
    } else {
        // find in cubrid's artifactory repository
        maven {
            url = uri("http://maven.cubrid.org/")
            isAllowInsecureProtocol = true
        }
    }
}

dependencies {
    // Use JUnit Jupiter for testing.
    testImplementation("org.junit.jupiter:junit-jupiter:5.9.1")

    // Unix Domain Socket
    implementation("com.kohlschutter.junixsocket:junixsocket-core:2.8.3")
    implementation("com.kohlschutter.junixsocket:junixsocket-server:2.8.3")

    // ANTLR
    antlr("org.antlr:antlr4:4.9.3")

    implementation("org.apache.commons:commons-text:1.10.0")
    implementation("org.apache.commons:commons-collections4:4.4")
    implementation("org.apache.commons:commons-lang3:3.13.0")
    implementation("org.antlr:antlr4-runtime:4.9.3")

    // CUBRID JDBC
    implementation("cubrid:cubrid-jdbc:latest.integration")

    // netty
    implementation("io.netty:netty-buffer:4.1.95.Final")
}

// Antlr
tasks.generateGrammarSource {
    // outputDirectory = file("${project.buildDir}/generated-src/antlr")
    arguments = arguments + listOf("-listener","-visitor")
}

// Apply a specific Java toolchain to ease working on different environments.
java {
    toolchain {
        languageVersion.set(JavaLanguageVersion.of(8))
    }
}

application {
    // Define the main class for the application.
    mainClass.set("com.cubrid.jsp.Server")
}

tasks.test {
    // Use JUnit Platform for unit tests.
    useJUnitPlatform()

    testLogging {
        lifecycle {
            events = mutableSetOf(TestLogEvent.FAILED, TestLogEvent.PASSED, TestLogEvent
                .SKIPPED)
            exceptionFormat = TestExceptionFormat.FULL

            showExceptions = true
            showCauses = true
            showStackTraces = true
            showStandardStreams = true
        }
        info.events = lifecycle.events
        info.exceptionFormat = lifecycle.exceptionFormat
    }

    val failedTests = mutableListOf<TestDescriptor>()
    val skippedTests = mutableListOf<TestDescriptor>()

    addTestListener(object : TestListener {
        override fun beforeSuite(suite: TestDescriptor) {}

        override fun beforeTest(testDescriptor: TestDescriptor) {}

        override fun afterTest(testDescriptor: TestDescriptor, result: TestResult) {
            when (result.resultType) {
                TestResult.ResultType.FAILURE -> failedTests.add(testDescriptor)
                TestResult.ResultType.SKIPPED -> skippedTests.add(testDescriptor)
                else -> Unit
            }
        }

        override fun afterSuite(suite: TestDescriptor, result: TestResult) {
            if (suite.parent == null) {
                logger.lifecycle("\n################ Summary::Start ################")
                logger.lifecycle("Test result: ${result.resultType}")
                logger.lifecycle(
                    "Test summary: ${result.testCount} tests, " +
                            "${result.successfulTestCount} succeeded, " +
                            "${result.failedTestCount} failed, " +
                            "${result.skippedTestCount} skipped")
                failedTests.takeIf { it.isNotEmpty() }?.prefixedSummary("\tFailed Tests")
                skippedTests.takeIf { it.isNotEmpty() }?.prefixedSummary("\tSkipped Tests:")
                logger.lifecycle("################ Summary::End ##################")
            }
        }

        private infix fun List<TestDescriptor>.prefixedSummary(subject: String) {
            logger.lifecycle(subject)
            forEach { test -> logger.lifecycle("\t\t${test.displayName()}") }
        }

        private fun TestDescriptor.displayName() = parent?.let { "${it.name} - $name" } ?: "$name"

    })
}

tasks.jar {
    // To make fat Jar
    val dependencies = configurations
        .runtimeClasspath
        .get()
        .map(::zipTree) // OR .map { zipTree(it) }
    from(dependencies)
    duplicatesStrategy = DuplicatesStrategy.EXCLUDE
}