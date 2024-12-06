#include <wiringPi.h>

// GPIO 핀 매핑
#define M_IN1 4  // IN1 ------- GPIO 4
#define M_IN2 5  // IN2 ------- GPIO 5
#define M_IN3 13 // IN3 ------- GPIO 13
#define M_IN4 12 // IN4 ------- GPIO 12

int motor_vector = 1; // 모터 회전 방향 (1: 정회전, 0: 역회전)

void setup()
{
    // WiringPi 초기화
    wiringPiSetupGpio(); // GPIO 핀 번호 사용

    // GPIO 핀을 출력으로 설정
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

void loop()
{
    while (1)
    {
        // 모터 정회전
        motorControl(motor_vector, 100); // 양쪽 바퀴 동일 방향, 속도 50%
        // delay(5000);                    // 5초 대기

        // // 모터 정지
        // motorControl(0, 0); // 양쪽 바퀴 정지
        // delay(5000);        // 5초 대기

        // // 모터 역회전
        // motorControl(!motor_vector, 50); // 양쪽 바퀴 동일 역방향, 속도 50%
        // delay(5000);                     // 5초 대기
    }
}

int main()
{
    setup(); // 초기화
    loop();  // 무한 루프 실행
    return 0;
}
