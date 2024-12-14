#include <stdio.h>
#include "DCMotor.c"

void moveFront(int sec) 
{
    controlMotor(0, DCMOTOR_SPWM_RANGE/2, DCMOTOR_SPWM_RANGE/2);
    delay(1000 * sec);
    motorStop();
}

void moveBack(int sec) 
{
    controlMotor(1, DCMOTOR_SPWM_RANGE/2, DCMOTOR_SPWM_RANGE/2);
    delay(1000 * sec);
    motorStop();
}

void turn(int dir) {
    controlMotor(0, DCMOTOR_SPWM_RANGE/2, DCMOTOR_SPWM_RANGE/2);
    delay(500);
    if (dir) {
        controlMotor(0, DCMOTOR_SPWM_RANGE/8, DCMOTOR_SPWM_RANGE);
    } else {
        controlMotor(0, DCMOTOR_SPWM_RANGE, DCMOTOR_SPWM_RANGE/8);
    }
    delay(1000);
    motorStop();
}

int main()
{
    // WiringPi 초기화
    wiringPiSetupGpio(); // GPIO 핀 번호 사용
    DCMotorSetup(); // 초기화

    int dir, sec;
    while (1)
    {
        printf("방향(전:0, 후:1, 회: 2), 시간(초) or 방향(오:0, 왼:1): ");
        scanf("%d %d", &dir, &sec);
        if (dir == 0) {
            moveFront(sec);
        }
        else if (dir == 1) {
            moveBack(sec);
        }
        else {
            turn(sec);
        }
    }

    return 0;
}