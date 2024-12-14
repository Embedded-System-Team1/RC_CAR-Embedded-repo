#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jansson.h> // JSON 처리용 라이브러리
#include "dc_motor.c" // DC 모터 명령 처리 함수 포함
#include "horn.c"     // Horn 명령 처리 함수 포함

// -- DC 모터 장치 추가 및 기능 구현 커스텀 예시 --
// 핸들러 등록 테이블
#define MAX_MOTOR_TYPES 10

// 모든 모터가 구현해야 하는 기본 인터페이스
typedef struct {
    void (*handle_command)(void* command); // 명령 처리 함수
    void* (*parse_message)(const char* message); // 메시지 -> 구조체 변환 함수
    void (*pin_mapping)();
} MotorHandler;

MotorHandler motor_handlers[MAX_MOTOR_TYPES] = {0};

// 핸들러 등록 함수
void register_motor_handler(int motor_id, MotorHandler handler) {
    if (motor_id < 0 || motor_id >= MAX_MOTOR_TYPES) {
        fprintf(stderr, "Invalid motor ID for registration\n");
        return;
    }
    motor_handlers[motor_id] = handler;
}

// 명령 처리 함수
void handle_motor_command(int motor_id, const char* message) {
    if (motor_id < 0 || motor_id >= MAX_MOTOR_TYPES || motor_handlers[motor_id].handle_command == NULL) {
        fprintf(stderr, "Invalid or unregistered motor ID: %d\n", motor_id);
        return;
    }

    // 메시지를 해당 모터의 구조체로 변환
    void* command_struct = motor_handlers[motor_id].parse_message(message);
    if (!command_struct) {
        fprintf(stderr, "Failed to parse message for motor ID: %d\n", motor_id);
        return;
    }

    // 명령 처리
    motor_handlers[motor_id].handle_command(command_struct);

    // 필요한 경우 메모리 해제
    free(command_struct);
}

// 핸들러 등록 함수
void register_custom_device_handler() {
    wiringPiSetup();

    // DC 모터 핸들러 생성
    MotorHandler dc_motor_handler = {
        .handle_command = handle_dc_motor_command,
        .parse_message = parse_dc_motor_message,
    };
    register_motor_handler(0, dc_motor_handler); // 모터 ID 0에 등록
    DCMotorSetup();                              // 모터 핀 셋업

    // Horn 핸들러 생성
    MotorHandler horn_handler = {
        .handle_command = handle_horn_command,
        .parse_message = parse_horn_message,
    };
    register_motor_handler(1, horn_handler); // 모터 ID 1에 등록


}
