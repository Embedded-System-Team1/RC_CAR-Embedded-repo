#ifndef DCMOTOR_H
#define DCMOTOR_H
#include <wiringPi.h>

// GPIO 핀 매핑
#define M_IN1 4  // IN1 ------- GPIO 4
#define M_IN2 5  // IN2 ------- GPIO 5
#define M_IN3 13 // IN3 ------- GPIO 13
#define M_IN4 12 // IN4 ------- GPIO 12

void DCMotorSetup()
{
    pinMode(M_IN1, OUTPUT);
    pinMode(M_IN2, OUTPUT);
    pinMode(M_IN3, OUTPUT);
    pinMode(M_IN4, OUTPUT);
}

void motorControl(int motor_dir, int speed)
{
    // 모터 A 제어
    digitalWrite(M_IN1, !motor_dir);
    digitalWrite(M_IN2, motor_dir);

    // 모터 B 제어
    digitalWrite(M_IN3, !motor_dir);
    digitalWrite(M_IN4, motor_dir);
}

void motorStop() 
{
    // 모터 A 제어
    digitalWrite(M_IN1, 0);
    digitalWrite(M_IN2, 0);

    // 모터 B 제어
    digitalWrite(M_IN3, 0);
    digitalWrite(M_IN4, 0);
}

void moveFront(int sec) 
{
    motorControl(1, 100);
    delay(1000 * sec);
    motorStop();
}

void moveBack(int sec) 
{
    motorControl(0, 100);
    delay(1000 * sec);
    motorStop();
}

#endif