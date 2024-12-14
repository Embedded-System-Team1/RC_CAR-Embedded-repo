#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jansson.h> // JSON 처리용 라이브러리
#include "device/AutoLight.c"

// 오토라이트 명령 구조체
typedef struct
{
    int id;   // 필수 값
    int mode; // 0: OFF, 1: ON
} AutoLightCommand;

void *parse_auto_light_message(const char *json_str) {
    if (!json_str) {
        fprintf(stderr, "[Parser] Input JSON string is NULL\n");
        return NULL;
    }

    json_t *root;
    json_error_t error;

    root = json_loads(json_str, 0, &error);
    if (!root) {
        fprintf(stderr, "[Parser] JSON parsing error: %s\n", error.text);
        return NULL;
    }

    AutoLightCommand *command = malloc(sizeof(AutoLightCommand));
    if (!command) {
        fprintf(stderr, "[Parser] Memory allocation failed\n");
        json_decref(root); // Ensure JSON memory is released
        return NULL;
    }

    json_t *id = json_object_get(root, "id");
    json_t *mode = json_object_get(root, "mode");

    if (json_is_integer(id)) {
        command->id = json_integer_value(id);
    } else {
        fprintf(stderr, "[Parser] Missing or invalid 'id' field\n");
        free(command);
        json_decref(root); // Ensure JSON memory is released
        return NULL;
    }

    if (json_is_integer(mode)) {
        command->mode = json_integer_value(mode);
    } else {
        fprintf(stderr, "[Parser] Missing or invalid 'mode' field\n");
        free(command);
        json_decref(root); // Ensure JSON memory is released
        return NULL;
    }

    json_decref(root); // Always release JSON memory
    return command;
}

void handle_auto_light_command(void *command) {
    if (!command) {
        fprintf(stderr, "[AutoLight] Null command received\n");
        return;
    }

    AutoLightCommand *auto_command = (AutoLightCommand *)command;
    printf("[AutoLight] Received mode: %d\n", auto_command->mode);

    HandleAutoLightMode(auto_command->mode); // Ensure this function is safe
}
