#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jansson.h> // JSON 처리용 라이브러리

// 경적 명령 구조체
typedef struct {
    int id;         // 필수 값
    int hornState;  // 0: OFF, 1: ON
} HornCommand;

// JSON 문자열을 경적 명령 구조체로 변환
void* parse_horn_message(const char* json_str) {
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
    HornCommand* command = malloc(sizeof(HornCommand));
    if (!command) {
        fprintf(stderr, "[Parser] Memory allocation failed\n");
        json_decref(root);
        return NULL;
    }

    // JSON 데이터 추출
    json_t* id = json_object_get(root, "id");
    json_t* hornState = json_object_get(root, "hornState");

    if (json_is_integer(id)) {
        command->id = json_integer_value(id);
    } else {
        fprintf(stderr, "[Parser] Missing or invalid 'id' field\n");
        free(command);
        json_decref(root);
        return NULL;
    }

    if (json_is_integer(hornState)) {
        command->hornState = json_integer_value(hornState);
    } else {
        fprintf(stderr, "[Parser] Missing or invalid 'hornState' field\n");
        free(command);
        json_decref(root);
        return NULL;
    }

    json_decref(root); // JSON 메모리 해제
    return command;
}

// 경적 명령 처리 함수
void handle_horn_command(void* command) {
    HornCommand* horn_command = (HornCommand*)command;
    if (horn_command->hornState == 1) {
        printf("Horn ON (ID: %d)\n", horn_command->id);
    } else {
        printf("Horn OFF (ID: %d)\n", horn_command->id);
    }
}
