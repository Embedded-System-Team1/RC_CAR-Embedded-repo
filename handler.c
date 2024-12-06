#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringSerial.h>

#include <string.h>
#include <mqueue.h>
#define BAUD_RATE 115200
#define MESSAGE_QUEUE_NAME "/rc_car_queue"
static const char* UART1_DEV = "/dev/ttyAMA1";


int connection_flug = 0;

pthread_mutex_t mid;

unsigned char serialRead(const int fd) //1Byte 데이터를 수신하는 함수
{
	unsigned char x;
	if(read (fd, &x, 1) != 1) //read 함수를 통해 1바이트 읽어옴
	return -1;
	
	return x; //읽어온 데이터 반환
}


void* thread_bluetooth_connection(void* arg){
    int fd_serial = *((int*)arg);
    mqd_t mq;
    char buffer[256];

    // 메시지 큐 오픈
    mq = mq_open(MESSAGE_QUEUE_NAME, O_WRONLY);
    if (mq == (mqd_t)-1) {
        perror("[Handler] Failed to open message queue");
        return NULL;
    }

    while (1) {
        pthread_mutex_lock(&mid);

        // 블루투스 데이터 수신 (JSON 형태 가정)
        if (serialDataAvail(fd_serial)) {
            int idx = 0;
            while (idx < sizeof(buffer) - 1 && buffer[idx-1] != '}') {
                buffer[idx++] = serialRead(fd_serial);
            }
            if(idx >= sizeof(buffer) - 1) { // 버퍼 크기 초과에 따른 예외 처리
                printf("[Handler] data's size is too big\n");
                memset(buffer, 0, sizeof(buffer)); // buf 초기화
                pthread_mutex_unlock(&mid);
                continue;
            }

            buffer[idx] = '\0'; // 문자열 끝 추가
            printf("[Handler] bluetooth received: %s\n", buffer);

            // 메시지 큐로 전송
            if (mq_send(mq, buffer, strlen(buffer), 0) == -1) {
                perror("[Handler] Failed to send message");
            } else {
                printf("[Handler] sent: %s\n", buffer);
            }
        }
        memset(buffer, 0, sizeof(buffer)); // buf 초기화
        pthread_mutex_unlock(&mid);
    }

    mq_close(mq);
    return NULL;
}



void* thread_test_connection(void* arg){
    int num = 0;
    while(1){
        scanf("%d",&num);
        pthread_mutex_lock(&mid); // 뮤텍스 잠금
        while(num==1){
            printf("--\n");
            scanf("%d",&num);
            if(num==0) break;
        }

        pthread_mutex_unlock(&mid); // 뮤텍스 잠금 해제
    }
}


int init(int* fd){
    int fd_serial ;
    if (wiringPiSetupGpio () < 0) return 0;

    if ((fd_serial = serialOpen (UART1_DEV, BAUD_RATE)) < 0)
    {
        printf ("[Handler] Unable to open serial device.\n") ;
        return 0;
    }
    *fd = fd_serial;
}


int fork_handler()
{



    int fd_serial;
    init(&fd_serial);

    pthread_t thread_blue,thread_test;
    pthread_mutex_init(&mid, NULL); // 뮤텍스 초기화
    pthread_create(&thread_blue,NULL,thread_bluetooth_connection,(void*)&fd_serial);
    pthread_create(&thread_test,NULL,thread_test_connection,NULL);

	pthread_join(thread_blue, NULL); //생산자 쓰레드 종료 대기
	pthread_join(thread_test, NULL); //소비자 쓰레드 종료 대기
	pthread_mutex_destroy(&mid); // 뮤텍스 제거


}

int main(){
    printf("Handler init\n");
    fork_handler();
    printf("Handler exit\n");
}
