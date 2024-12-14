#ifndef HORN_H
#define HORN_H
#include <wiringPi.h>
#include <stdio.h>

// GPIO 핀 매핑
#define PWM_PIN 18 // PWM 핀 (GPIO 18)

// 초기 설정 함수
void HornSetup() 
{
    pinMode(PWM_PIN, PWM_OUTPUT); // PWM 핀을 출력 모드로 설정
    pwmSetMode(PWM_MODE_MS);      // PWM 모드 설정 (마크-스페이스 모드)
    pwmSetRange(1000);            // Duty Cycle 범위 설정
    pwmWrite(PWM_PIN, 0);         // 초기 Duty Cycle은 0으로 설정
}

// 주파수를 설정하고 경적을 울리는 함수
void HornOn(int freq) 
{
    if (freq <= 0) {
        fprintf(stderr, "[Horn] Invalid frequency: %d\n", freq);
        return;
    }

    int divisor = 19200 / freq; // 주파수에 따른 분주값 계산
    pwmSetClock(divisor);       // PWM 주파수 설정
    pwmWrite(PWM_PIN, 500);     // Duty Cycle 설정 (50%)
    printf("[Horn] Horn ON with frequency: %d Hz\n", freq);
}

// 경적을 멈추는 함수
void HornOff() 
{
    pwmWrite(PWM_PIN, 0); // Duty Cycle을 0으로 설정하여 경적 멈춤
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
