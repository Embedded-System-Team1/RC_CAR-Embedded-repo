#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jansson.h> // JSON 처리용 라이브러리

// DC 모터 명령 구조체
typedef struct {
    int id; // 필수 값
    int speed;
    int direction;
} DCMotorCommand;

// JSON 문자열을 DC 모터 명령 구조체로 변환
void* parse_dc_motor_message(const char* json_str) {
    if (!json_str) {
        fprintf(stderr, "[Parser] Input JSON string is NULL\n");
        return NULL;
    }

    json_t* root;
    json_error_t error;

    // JSON 파싱
    root = json_loads(json_str, 0, &error);
    if (!root) {
        fprintf(stderr, "[Parser] JSON parsing error: %s\n", error.text);
        return NULL;
    }

    // 메모리 할당
    DCMotorCommand* command = malloc(sizeof(DCMotorCommand));
    if (!command) {
        fprintf(stderr, "[Parser] Memory allocation failed\n");
        json_decref(root);
        return NULL;
    }

    // JSON 데이터 추출
    json_t* id = json_object_get(root, "id");
    json_t* speed = json_object_get(root, "speed");
    json_t* direction = json_object_get(root, "direction");


    if (json_is_integer(id)) {
        command->id = json_integer_value(id);
    } else {
        fprintf(stderr, "[Parser] Missing or invalid 'id' field\n");
        free(command);
        json_decref(root);
        return NULL;
    }
    if (json_is_integer(speed)) {
        command->speed = json_integer_value(speed);
    } else {
        fprintf(stderr, "[Parser] Missing or invalid 'speed' field\n");
        free(command);
        json_decref(root);
        return NULL;
    }

    if (json_is_integer(direction)) {
        command->direction = json_integer_value(direction);
    } else {
        fprintf(stderr, "[Parser] Missing or invalid 'direction' field\n");
        free(command);
        json_decref(root);
        return NULL;
    }

    json_decref(root); // JSON 메모리 해제
    return command;
}

void handle_dc_motor_command(void* command) {
    DCMotorCommand* dc_command = (DCMotorCommand*)command;
    printf("DC Motor - Speed: %d, Direction: %d\n", dc_command->speed, dc_command->direction);
}

