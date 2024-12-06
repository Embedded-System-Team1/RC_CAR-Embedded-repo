#include <stdio.h>
#include <unistd.h>
#include <mqueue.h>
#include <string.h>
#include <jansson.h> // JSON 처리용 라이브러리

// 데이터 구조체 정의
typedef struct {
    int id;
    char message[100];
} Command;

// JSON 문자열을 구조체로 변환
int parse_json_to_struct(const char* json_str, Command* cmd) {
    json_t* root;
    json_error_t error;

    root = json_loads(json_str, 0, &error);
    if (!root) {
        fprintf(stderr, "JSON parsing error: %s\n", error.text);
        return -1;
    }

    json_t* id = json_object_get(root, "id");
    json_t* message = json_object_get(root, "message");

    if (json_is_integer(id)) {
        cmd->id = json_integer_value(id);
    }

    if (json_is_string(message)) {
        strncpy(cmd->message, json_string_value(message), sizeof(cmd->message) - 1);
        cmd->message[sizeof(cmd->message) - 1] = '\0'; // Null terminator 추가
    }

    json_decref(root); // JSON 메모리 해제
}
