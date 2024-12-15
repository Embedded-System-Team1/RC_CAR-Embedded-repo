#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <softPwm.h>

#define SERVO 2

#define SERVO_RANGE 100
#define ANGLE_TO_MS(angle) (((angle)+90)/90+0.5)
#define MS_TO_DUTY(x) ((x) * (SERVO_RANGE) / 20)

void pwmInit()
{
    if (softPwmCreate(SERVO, 0, SERVO_RANGE) != 0) {
        fprintf(stderr, "softPwm 초기화 실패\n");
        exit(EXIT_FAILURE);
    }
}

void rotate_Servo(float angle)
{
    if (angle > 90) {
        angle = 90;
    } else if (angle < -90) {
        angle = -90;
    }

    float ms = ANGLE_TO_MS(angle); // 각도를 ms로 변환
    printf("%fms\n", ms);

    int dutyCycle = MS_TO_DUTY(ms); // ms를 듀티 사이클로 변환
    softPwmWrite(SERVO, dutyCycle); // 소프트웨어 PWM으로 듀티 사이클 설정
}

void stop_Servo()
{
    softPwmWrite(SERVO, 0); // 듀티 사이클을 0으로 설정하여 전력을 끔
}

void open_ceiling() {
    rotate_Servo(90);
    stop_Servo();
}

void close_ceiling() {
    rotate_Servo(-10);
    stop_Servo();
}
