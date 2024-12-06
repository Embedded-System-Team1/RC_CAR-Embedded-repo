#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>

#define PWM1 19

#define RANGE 2000
#define CLOCK ((19200000/(RANGE))/50)
#define ANGLE_TO_MS(angle) (((angle)+90)/90+0.5)
#define MS_TO_DUTY(x) ((x) * (RANGE)/20)

void pwmInit() 
{
    pinMode(PWM1, PWM_OUTPUT);
    pwmSetMode(PWM_MODE_MS);
    pwmSetRange(RANGE);
    pwmSetClock(CLOCK);
}

void rotate_Servo(float angle)
{
    if (angle > 90) {
        angle = 90;
    } else if (angle < -90) {
        angle = -90;
    }

    float ms = ANGLE_TO_MS(angle);
    printf("%fms\n", ms);
    pwmWrite(PWM1, MS_TO_DUTY(ms));
}

int main()
{
    wiringPiSetupGpio();
    pwmInit();

    float angle;
    
    while (1)
    {
        printf("각도: ");
        scanf("%f", &angle);
        rotate_Servo(angle);
        delay(10);
    }
    
    return 0;
}