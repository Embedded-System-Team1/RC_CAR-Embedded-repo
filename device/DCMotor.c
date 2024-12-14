#ifndef DCMOTOR_H
#define DCMOTOR_H
#include <wiringPi.h>
#include <softPwm.h>

// GPIO 핀 매핑
#define M_IN1 13 // IN3 ------- GPIO 13
#define M_IN2 12 // IN4 ------- GPIO 12
#define M_IN3 4  // IN1 ------- GPIO 4
#define M_IN4 5  // IN2 ------- GPIO 5

#define DCMOTOR_SPWM_RANGE 1024

#define SPEED_DIFFERENCE 600

void DCMotorSetup()
{
    pinMode(M_IN1, OUTPUT);
    softPwmCreate(M_IN1, 0, DCMOTOR_SPWM_RANGE);

    pinMode(M_IN2, OUTPUT);
    softPwmCreate(M_IN2, 0, DCMOTOR_SPWM_RANGE);

    pinMode(M_IN3, OUTPUT);
    softPwmCreate(M_IN3, 0, DCMOTOR_SPWM_RANGE);

    pinMode(M_IN4, OUTPUT);
    softPwmCreate(M_IN4, 0, DCMOTOR_SPWM_RANGE);
}

void controlMotor(int motor_dir, int Aspeed, int Bspeed)
{
    if (motor_dir) {
        // 모터 A 제어
        softPwmWrite(M_IN1, Aspeed);
        softPwmWrite(M_IN2, 0);
        
        // 모터 B 제어
        softPwmWrite(M_IN3, Bspeed);
        softPwmWrite(M_IN4, 0);
    } else {
        // 모터 A 제어
        softPwmWrite(M_IN1, 0);
        softPwmWrite(M_IN2, Aspeed);
        
        // 모터 B 제어
        softPwmWrite(M_IN3, 0);
        softPwmWrite(M_IN4, Bspeed);
    }
}

// turn(왼쪽:-1, 직진:0, 오른쪽: 1), forward(전진: 0, 후진: 1)
void controlCar(int speed, int turn, int forward) 
{
    speed = speed > 1024 ? 1024 : speed;
    speed = speed < 0 ? 0 : speed;

    if (turn < 0) {
        controlMotor(forward, speed-SPEED_DIFFERENCE > 50 ? speed-SPEED_DIFFERENCE : 50, speed);
    } else if (turn > 0) {
        controlMotor(forward, speed, speed-SPEED_DIFFERENCE > 50 ? speed-SPEED_DIFFERENCE : 50);
    } else {
        controlMotor(forward, speed, speed);
    }
    delay(50);
}

#endif