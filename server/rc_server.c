#include <libwebsockets.h>
#include <string.h>
#include <stdio.h>
#include <mqueue.h>
#include <set>
#include <string>

#include "../handler_mutex.c"

std::set<std::string> connected_clients; // 연결된 클라이언트 관리
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER; // 스레드 안전성 보장

#define MESSAGE_QUEUE_NAME "/rc_car_queue"
#define CLIENT_TIMEOUT_SECONDS 30 // 30초 (초 단위)

void remove_invalid_json_characters(char* buffer);

void mq_push(char* buffer) {
  mqd_t mq;

  mq = mq_open(MESSAGE_QUEUE_NAME, O_WRONLY);
  if (mq == (mqd_t)-1) {
      perror("[Handler] Failed to open message queue");
      return;
  }

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

  mq_close(mq); // 메시지 큐 종료
}

int speed = 400;
int forward = 1;

void updateForwardSpeed() {
  if (forward == 1) {
    speed += 10;
    speed = speed > 1024 ? 1024 : speed;
    forward = 1;
  } else {
    speed = 400;
    forward = 1;
  }
}

void updateBackwardSpeed() {
  if (forward == 0) {
    speed += 10;
    speed = speed > 1024 ? 1024 : speed;
    forward = 0;
  } else {
    speed = 400;
    forward = 0;
  }
}

// 클라이언트 메시지 처리 콜백
static int callback_server(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
    switch (reason) {
        // 클라이언트가 연결되었을 때
        case LWS_CALLBACK_ESTABLISHED:
            printf("TCP 소켓 연결됨\n");
            // 클라이언트 IP 가져오기
            lws_get_peer_simple(wsi, client_ip, sizeof(client_ip));
            printf("클라이언트 연결 시도: %s\n", client_ip);

            pthread_mutex_lock(&mid);

            // 이미 연결된 클라이언트인지 확인
            if (connected_clients.find(client_ip) != connected_clients.end()) {
                printf("이미 연결된 클라이언트입니다: %s\n", client_ip);
                lws_close_reason(wsi, LWS_CLOSE_STATUS_NORMAL, (unsigned char *)"Duplicate connection", 20);
                pthread_mutex_unlock(&mid);
                return -1; // 연결 차단
            }

            // 새 클라이언트 추가
            connected_clients.insert(client_ip);
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
              updateForwardSpeed();
              sprintf(buffer, "{\"id\": 0, \"speed\": %d, \"directionX\": 0, \"directionY\": 0}\n", speed);
              mq_push(buffer);
            } else if (strncmp((char *)in, "FORWARD_LEFT", len) == 0) {
              char buffer[256];
              updateForwardSpeed();
              sprintf(buffer, "{\"id\": 0, \"speed\": %d, \"directionX\": -1, \"directionY\": 0}\n", speed);
              mq_push(buffer);
            } else if (strncmp((char *)in, "FORWARD_RIGHT", len) == 0) {
              char buffer[256];
              updateForwardSpeed();
              sprintf(buffer, "{\"id\": 0, \"speed\": %d, \"directionX\": 1, \"directionY\": 0}\n", speed);
              mq_push(buffer);
            } else if (strncmp((char *)in, "BACKWARD", len) == 0) {
              char buffer[256];
              updateBackwardSpeed();
              sprintf(buffer, "{\"id\": 0, \"speed\": %d, \"directionX\": 0, \"directionY\": 1}\n", speed);
              mq_push(buffer);
            } else if (strncmp((char *)in, "BACKWARD_LEFT", len) == 0) {
              char buffer[256];
              updateBackwardSpeed();
              sprintf(buffer, "{\"id\": 0, \"speed\": %d, \"directionX\": -1, \"directionY\": 1}\n", speed);
              mq_push(buffer);
            } else if (strncmp((char *)in, "BACKWARD_RIGHT", len) == 0) {
              char buffer[256];
              updateBackwardSpeed();
              sprintf(buffer, "{\"id\": 0, \"speed\": %d, \"directionX\": 1, \"directionY\": 1}\n", speed);
              mq_push(buffer);
            } else if (strncmp((char *)in, "HORN", len) == 0) {
              char buffer[256] = "{\"id\": 1, \"hornState\": 1}\n";
              mq_push(buffer);
            } else if (strncmp((char *)in, "END_HORN", len) == 0) {
              char buffer[256] = "{\"id\": 1, \"hornState\": 0}\n";
              mq_push(buffer);
            } else if (strncmp((char *)in, "ON_AUTO_LIGHT", len) == 0) {
              char buffer[256] = "{\"id\": 2, \"mode\": 1}\n";
              mq_push(buffer);
            } else if (strncmp((char *)in, "OFF_AUTO_LIGHT", len) == 0) {
              char buffer[256] = "{\"id\": 2, \"mode\": 0}\n";
              mq_push(buffer);
            } else if (strncmp((char *)in, "OPEN_CEILING", len) == 0) {
              char buffer[256] = "{\"id\": 3, \"ceilingStatus\": 1}\n";
              mq_push(buffer);
            } else if (strncmp((char *)in, "CLOSE_CEILING", len) == 0) {
              char buffer[256] = "{\"id\": 3, \"ceilingStatus\": 0}\n";
              mq_push(buffer);
            } else if (strncmp((char *)in, "STOP", len) == 0) {
              forward = 2;
              char buffer[256] = "{\"id\": 0, \"speed\": 0, \"directionX\": 0, \"directionY\": 0}\n";
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

void start_server(void) {
    struct lws_context_creation_info info;
    struct lws_context *context;

    memset(&info, 0, sizeof(info));
    info.port = 9000; // 서버 포트
    info.protocols = protocols;

    // 웹소켓 컨텍스트 생성
    context = lws_create_context(&info);
    if (!context) {
        fprintf(stderr, "웹소켓 컨텍스트 생성 실패\n");
        return;
    }

    printf("WebSocket 서버가 포트 9000에서 실행 중입니다.\n");

    // 서버 실행 루프
    while (1) {
        lws_service(context, 0);
    }

    // 웹소켓 컨텍스트 종료
    lws_context_destroy(context);
    return;
}
