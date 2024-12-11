#include <libwebsockets.h>
#include <string.h>
#include <stdio.h>

// 클라이언트 메시지 처리 콜백
static int callback_server(struct lws *wsi, enum lws_callback_reasons reason,
                           void *user, void *in, size_t len) {
    switch (reason) {
        case LWS_CALLBACK_RECEIVE:
            // 메시지를 받았을 때
            printf("Received: %.*s\n", (int)len, (char *)in);

            if (strncmp((char *)in, "FORWARD_LEFT", len) == 0) {
              printf("{ id: 1, speed: 50, direction: 1 }\n");
            } else if (strncmp((char *)in, "FORWARD_RIGHT", len) == 0) {
              printf("{ id: 1, speed: 50, direction: 1 }\n");
            } else if (strncmp((char *)in, "FORWARD", len) == 0) {
              printf("{ id: 1, speed: 50, direction: 1 }\n");
            } else if (strncmp((char *)in, "BACKWARD", len) == 0) {
              printf("{ id: 1, speed: 50, direction: 0 }\n");
            } else if (strncmp((char *)in, "LEFT", len) == 0) {
              printf("{ id: 2, speed: 50, direction: 1 }\n");
            } else if (strncmp((char *)in, "RIGHT", len) == 0) {
              printf("{ id: 2, speed: 50, direction: 1 }\n");
            } else if (strncmp((char *)in, "STOP", len) == 0) {
              printf("{ id: 1, speed: 0, direction: 0 }\n");
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
