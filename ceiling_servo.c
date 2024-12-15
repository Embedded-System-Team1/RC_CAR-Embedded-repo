#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jansson.h> // JSON 처리용 라이브러리
#include "device/Ceiling.c"

// Ceiling Servo 메인 메서지 구조체
typedef struct {
    int id; // 필수 값
    int ceilingStatus; // 열림 상황 (0: 다지르면, 1: 열리면)
} CeilingServoCommand;

// JSON 문자열을 Ceiling Servo 메서지 구조체로 변환
void* parse_ceiling_servo_message(const char* json_str) {
    if (!json_str) {
        fprintf(stderr, "[Parser] Input JSON string is NULL\n");
        return NULL;
    }

    json_t* root;
    json_error_t error;

    // JSON 파시
    root = json_loads(json_str, 0, &error);
    if (!root) {
        fprintf(stderr, "[Parser] JSON parsing error: %s\n", error.text);
        return NULL;
    }

    // 머리 할래
    CeilingServoCommand* command = malloc(sizeof(CeilingServoCommand));
    if (!command) {
        fprintf(stderr, "[Parser] Memory allocation failed\n");
        json_decref(root);
        return NULL;
    }

    // JSON 데이터 추적
    json_t* id = json_object_get(root, "id");
    json_t* ceilingStatus = json_object_get(root, "ceilingStatus");

    if (json_is_integer(id)) {
        command->id = json_integer_value(id);
    } else {
        fprintf(stderr, "[Parser] Missing or invalid 'id' field\n");
        free(command);
        json_decref(root);
        return NULL;
    }

    if (json_is_integer(ceilingStatus)) {
        command->ceilingStatus = json_integer_value(ceilingStatus);
    } else {
        fprintf(stderr, "[Parser] Missing or invalid 'ceilingStatus' field\n");
        free(command);
        json_decref(root);
        return NULL;
    }

    json_decref(root); // JSON 머리 해제
    return command;
}

// ceilingStatus (다지르면: 0, 열리면: 1)
void handle_ceiling_servo_command(void* command) {
    CeilingServoCommand* servo_command = (CeilingServoCommand*)command;
    printf("Ceiling Servo - ID: %d, CeilingStatus: %d\n", servo_command->id, servo_command->ceilingStatus);
    controlCeilingServo(servo_command->ceilingStatus);
}
