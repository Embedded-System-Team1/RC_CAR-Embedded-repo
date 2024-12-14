#include <libwebsockets.h>
#include <string.h>
#include <stdio.h>
#include <mqueue.h>

#include "../handler_mutex.c"

#define MESSAGE_QUEUE_NAME "/rc_car_queue"

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

void mq_push(char* buffer) {
  mqd_t mq;

  mq = mq_open(MESSAGE_QUEUE_NAME, O_WRONLY);
  if (mq == (mqd_t)-1) {
      perror("[Handler] Failed to open message queue");
      return;
  }

   // 메시지 큐 오픈
  mq = mq_open(MESSAGE_QUEUE_NAME, O_WRONLY);

  if(strlen(buffer) > 256) {
      printf("[Handler] data's size is too big\n");
      memset(buffer, 0, sizeof(buffer)); // buf 초기화
      return;
  }

  remove_invalid_json_characters(buffer);
  // 메시지 큐로 전송
  if (mq_send(mq, buffer, strlen(buffer), 0) == -1) {
      perror("[Handler] Failed to send message");
  } else {
      printf("[Handler] sent: %s\n", buffer);
  }
  memset(buffer, 0, sizeof(buffer));
}

int speed = 0;
int forward = 1;

// 클라이언트 메시지 처리 콜백
static int callback_server(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
    switch (reason) {
        // 클라이언트가 연결되었을 때
        case LWS_CALLBACK_ESTABLISHED:
            printf("TCP 소켓 연결됨\n");
            pthread_mutex_lock(&mid);
            break;
        // 클라이언트 연결이 끊겼을 때
        case LWS_CALLBACK_CLOSED:
            printf("TCP 소켓 연결 해제됨\n");
            pthread_mutex_unlock(&mid);
            // 연결 종료 시 필요한 작업을 여기에 추가
            break;
        // 메시지를 받았을 때
        case LWS_CALLBACK_RECEIVE:
            printf("Received: %.*s\n", (int)len, (char *)in);

            if (strncmp((char *)in, "FORWARD", len) == 0) {
              char buffer[256];
              if (forward == 1) {
                speed++;
                speed = speed > 1024 ? 1024 : speed;
                forward = 1;
              } else {
                speed = 0;
                forward = 1;
              }
              sprintf(buffer, "{\"id\": 1, \"speed\": %d \"direction\": 1}\n", speed);
              mq_push(buffer);
            } else if (strncmp((char *)in, "FORWARD_LEFT", len) == 0) {
              char buffer[256] = "{\"id\": 2, \"speed\": 51, \"direction\": 1}\n";
              mq_push(buffer);
            } else if (strncmp((char *)in, "FORWARD_RIGHT", len) == 0) {
              char buffer[256] = "{\"id\": 3, \"speed\": 51, \"direction\": 1}\n";
              mq_push(buffer);
            } else if (strncmp((char *)in, "BACKWARD", len) == 0) {
              char buffer[256];
              if (forward == 0) {
                speed++;
                speed = speed > 1024 ? 1024 : speed;
                forward = 0;
              } else {
                speed = 0;
                forward = 0;
              }
              sprintf(buffer, "{\"id\": 1, \"speed\": %d \"direction\": 1}\n", speed);
              mq_push(buffer);
            } else if (strncmp((char *)in, "BACKWARD_LEFT", len) == 0) {
              char buffer[256] = "{\"id\": 5, \"speed\": 51, \"direction\": 1}\n";
              mq_push(buffer);
            } else if (strncmp((char *)in, "BACKWARD_RIGHT", len) == 0) {
              char buffer[256] = "{\"id\": 6, \"speed\": 51, \"direction\": 1}\n";
              mq_push(buffer);
            } else if (strncmp((char *)in, "LEFT", len) == 0) {
              char buffer[256] = "{\"id\": 7, \"speed\": 51, \"direction\": 1}\n";
              mq_push(buffer);
            } else if (strncmp((char *)in, "RIGHT", len) == 0) {
              char buffer[256] = "{\"id\": 8, \"speed\": 51, \"direction\": 1}\n";
              mq_push(buffer);
            } else if (strncmp((char *)in, "HORN", len) == 0) {
              char buffer[256] = "{\"id\": 9, \"speed\": 51, \"direction\": 1}\n";
              mq_push(buffer);
            } else if (strncmp((char *)in, "TOGGLE_CEILING", len) == 0) {
              char buffer[256] = "{\"id\": 10, \"speed\": 51, \"direction\": 1}\n";
              mq_push(buffer);
            } else if (strncmp((char *)in, "STOP", len) == 0) {
              forward = 2;
              char buffer[256] = "{\"id\": 11, \"speed\": 0, \"direction\": 1}\n";
              mq_push(buffer);
            }
            break;

        default:
            break;
    }
    return 0;
}

// 웹소켓 프로토콜 설정
static struct lws_protocols protocols[] = {
    {
        "default-protocol", // 프로토콜 이름
        callback_server,    // 콜백 함수
        0,                  // 사용자 데이터 크기
        0,                  // 버퍼 크기
    },
    { NULL, NULL, 0, 0 } // 끝을 표시
};

int main(void) {
    struct lws_context_creation_info info;
    struct lws_context *context;

    memset(&info, 0, sizeof(info));
    info.port = 9000; // 서버 포트
    info.protocols = protocols;

    // 웹소켓 컨텍스트 생성
    context = lws_create_context(&info);
    if (!context) {
        fprintf(stderr, "웹소켓 컨텍스트 생성 실패\n");
        return -1;
    }

    printf("WebSocket 서버가 포트 9000에서 실행 중입니다.\n");

    // 서버 실행 루프
    while (1) {
        lws_service(context, 0);
    }

    // 웹소켓 컨텍스트 종료
    lws_context_destroy(context);
    return 0;
}
