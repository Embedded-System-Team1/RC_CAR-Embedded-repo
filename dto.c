#include <stdio.h>
#include <unistd.h>
#include <mqueue.h>
#include <string.h>
#include <jansson.h> // JSON 처리용 라이브러리
#include <stdlib.h>
#include <unistd.h>
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


// 유효한 JSON 문자열만 남기고 나머지 불필요한 문자는 제거하는 함수
void remove_invalid_json_characters(char* buffer) {
    int start_idx = -1, end_idx = -1;
    int i, len = strlen(buffer);
    
    // 시작 괄호 '{' 찾기
    for (i = 0; i < len; i++) {
        if (buffer[i] == '{') {
            start_idx = i;
            break;
        }
    }

    // 끝 괄호 '}' 찾기
    for (i = len - 1; i >= 0; i--) {
        if (buffer[i] == '}') {
            end_idx = i;
            break;
        }
    }

    // 유효한 JSON 데이터를 찾은 경우
    if (start_idx != -1 && end_idx != -1 && end_idx > start_idx) {
        // 시작과 끝 인덱스 사이의 문자열을 복사
        memmove(buffer, buffer + start_idx, end_idx - start_idx + 1);
        buffer[end_idx - start_idx + 1] = '\0';  // 문자열 끝 추가
    } else {
        // 유효한 JSON 문자열이 없는 경우 빈 문자열 처리
        buffer[0] = '\0';
    }
}
