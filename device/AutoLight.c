#ifndef AUTOLIGHT_H
#define AUTOLIGHT_H

#include <stdio.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <pthread.h>

// I2C 설정
#define SLAVE_ADDR_01 0x48                 // 조도센서 I2C 주소
static const char *I2C_DEV = "/dev/i2c-1"; // I2C 장치 파일

// GPIO 핀 설정
#define LED_PIN_1 19 // LED 1
#define LED_PIN_2 24 // LED 2

// 밝기 임계값
#define LIGHT_THRESHOLD 235

// 상태 변수
int autoLightEnabled = 0;           // 0: OFF, 1: ON
static pthread_t autoLightThread;   // AutoLight 스레드
static int stopAutoLightThread = 0; // AutoLight 스레드 중단 플래그
int i2c_fd = -1;                    // I2C 파일 디스크립터 (전역 변수)

// 초기화 함수
void AutoLightSetup()
{
    // GPIO 핀 설정
    pinMode(LED_PIN_1, OUTPUT);
    pinMode(LED_PIN_2, OUTPUT);

    // LED 초기 상태 OFF
    digitalWrite(LED_PIN_1, LOW);
    digitalWrite(LED_PIN_2, LOW);

    // I2C 초기화
    i2c_fd = wiringPiI2CSetup(SLAVE_ADDR_01);
    if (i2c_fd < 0)
    {
        fprintf(stderr, "[AutoLight] Failed to initialize I2C\n");
    }
    else
    {
        printf("[AutoLight] I2C initialized with fd: %d\n", i2c_fd);
    }

    printf("[AutoLight] AutoLight initialized\n");
}

// 주변 밝기 감지 및 LED 제어
void AutoLightControl()
{
    int adcChannel = 0; // 조도 센서 채널 (AIN0)
    int curVal = 0;     // 현재 밝기 값

    while (!stopAutoLightThread)
    {
        if (!autoLightEnabled)
        {
            // AutoLight가 비활성화된 상태에서는 LED를 끔
            digitalWrite(LED_PIN_1, LOW);
            digitalWrite(LED_PIN_2, LOW);
            usleep(500 * 1000); // 500ms 대기
            continue;
        }

        if (i2c_fd < 0)
        {
            fprintf(stderr, "[AutoLight] Invalid I2C descriptor\n");
            usleep(500 * 1000); // 500ms 대기
            continue;
        }

        // 조도 센서 값 읽기
        wiringPiI2CWrite(i2c_fd, 0x40 | adcChannel); // ADC 채널 설정
        curVal = wiringPiI2CRead(i2c_fd);            // 밝기 값 읽기 (0~255)

        // 밝기 임계값 비교
        if (curVal < LIGHT_THRESHOLD)
        {
            // 밝을 때 LED OFF
            digitalWrite(LED_PIN_1, LOW);
            digitalWrite(LED_PIN_2, LOW);
        }
        else
        {
            // 어두울 때 LED ON
            digitalWrite(LED_PIN_1, HIGH);
            digitalWrite(LED_PIN_2, HIGH);
        }

        usleep(100000); // 100ms 대기
    }

    printf("[AutoLight] AutoLight thread exiting...\n");
}

// AutoLight 스레드 함수
void *AutoLightThreadFunc(void *arg)
{
    AutoLightControl();
    pthread_exit(NULL);
}

// AutoLight 활성화/비활성화 처리
void HandleAutoLightMode(int mode)
{
    if (mode == 1 && !autoLightEnabled)
    {
        autoLightEnabled = 1; // AutoLight 활성화
        stopAutoLightThread = 0;

        // AutoLight 스레드 생성
        if (pthread_create(&autoLightThread, NULL, AutoLightThreadFunc, NULL) != 0)
        {
            fprintf(stderr, "[AutoLight] Failed to create thread\n");
            autoLightEnabled = 0;
            return;
        }

        printf("[AutoLight] AutoLight mode enabled\n");
    }
    else if (mode == 0 && autoLightEnabled)
    {
        autoLightEnabled = 0; // AutoLight 비활성화
        stopAutoLightThread = 1;

        // 스레드 종료 대기
        if (pthread_join(autoLightThread, NULL) != 0)
        {
            fprintf(stderr, "[AutoLight] Failed to join thread\n");
        }

        // LED 끄기
        digitalWrite(LED_PIN_1, LOW);
        digitalWrite(LED_PIN_2, LOW);

        printf("[AutoLight] AutoLight mode disabled\n");
    }
}

#endif
