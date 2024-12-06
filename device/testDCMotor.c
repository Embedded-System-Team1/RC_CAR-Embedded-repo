#include <stdio.h>
#include "DCmotor.c"

int main()
{
    // WiringPi 초기화
    wiringPiSetupGpio(); // GPIO 핀 번호 사용
    DCMotorSetup(); // 초기화

    int dir, sec;
    while (1)
    {
        printf("방향(전진:1, 후진:0), 작동시간(초): ");
        scanf("%d %d", &dir, &sec);
        if (dir) {
            moveFront(sec);
        }
        else {
            moveBack(sec);
        }
    }

    return 0;
}