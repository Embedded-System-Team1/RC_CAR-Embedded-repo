#ifndef HORN_H
#define HORN_H
#include <wiringPi.h>
#include <softTone.h>
#include <stdio.h>

// GPIO 핀 매핑
#define HORN_PIN 18 // 경적에 사용할 핀 (GPIO 18)

// 초기 설정 함수
void HornSetup() 
{
    wiringPiSetupGpio();
    if (softToneCreate(HORN_PIN) != 0) { // SoftTone 초기화
        fprintf(stderr, "[Horn] Failed to initialize softTone on pin %d\n", HORN_PIN);
        return;
    }
    printf("[Horn] SoftTone initialized on pin %d\n", HORN_PIN);
}

// 주파수를 설정하고 경적을 울리는 함수
void HornOn(int freq) 
{
    if (freq <= 0) {
        fprintf(stderr, "[Horn] Invalid frequency: %d\n", freq);
        return;
    }

    softToneWrite(HORN_PIN, freq); // 지정된 주파수로 톤 생성
    printf("[Horn] Horn ON with frequency: %d Hz\n", freq);
}

// 경적을 멈추는 함수
void HornOff() 
{
    softToneWrite(HORN_PIN, 0); // 0 Hz를 설정하여 소리를 멈춤
    printf("[Horn] Horn OFF\n");
}

// 지정된 시간 동안 경적을 울리는 함수
void HornBeep(int freq, int duration_ms) 
{
    if (freq <= 0 || duration_ms <= 0) {
        fprintf(stderr, "[Horn] Invalid frequency or duration: freq=%d, duration=%d\n", freq, duration_ms);
        return;
    }

    HornOn(freq);               // 경적 켜기
    delay(duration_ms);         // 지정된 시간 대기
    HornOff();                  // 경적 끄기
}

#endif