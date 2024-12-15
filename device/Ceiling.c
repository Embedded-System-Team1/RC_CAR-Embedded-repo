#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>

#define SERVO 17

#define SERVO_RANGE 100
#define ANGLE_TO_MS(angle) (((angle)+90)/90+0.5)
#define MS_TO_DUTY(x) ((x) * (SERVO_RANGE) / 20)

void CeilingServoSetup()
{
    pinMode(SERVO, OUTPUT);
    if (softPwmCreate(SERVO, 0, SERVO_RANGE) != 0) {
        fprintf(stderr, "softPwm 초기화 실패\n");
        exit(EXIT_FAILURE);
    }
}

void rotateServo(float angle)
{
    if (angle > 90) {
        angle = 90;
    } else if (angle < -90) {
        angle = -90;
    }

    float ms = ANGLE_TO_MS(angle); // 각도를 ms로 변환
    int dutyCycle = MS_TO_DUTY(ms); // ms를 듀티 사이클로 변환
    softPwmWrite(SERVO, dutyCycle); // 소프트웨어 PWM으로 듀티 사이클 설정
}

void stopServo()
{
    softPwmWrite(SERVO, 0); // 듀티 사이클을 0으로 설정하여 전력을 끔
}

// open: 1, close: 0
void controlCeilingServo(int openStatus) {
    if (openStatus) {
        rotateServo(90);
    } else {
        rotateServo(-11);
    }
}