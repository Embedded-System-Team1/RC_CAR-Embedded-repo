#include <stdio.h>
#include <unistd.h>
#include <mqueue.h>
#include <string.h>
#include <pthread.h>
#include "device_adaptor.c" // 커스텀 장치 핸들러 등록 함수 포함

#define MESSAGE_QUEUE_NAME "/rc_car_queue"


// 모터 제어 스레드 구조체
typedef struct {
    pthread_t thread;
    char* command;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int is_active;
    int motor_id;
} MotorThread;


// 전역 변수: 모터 제어 스레드 배열
MotorThread motor_threads[MAX_MOTOR_TYPES];



// 모터 제어 스레드가 처리할 함수
void* motor_control_thread(void* arg) {
    MotorThread* motor_thread = (MotorThread*)arg;
    
    while (1) {
        pthread_mutex_lock(&motor_thread->mutex);
        
        // 명령이 있을 때까지 대기
        while (!motor_thread->is_active) {
            pthread_cond_wait(&motor_thread->cond, &motor_thread->mutex);
        }

        // 현재 쓰레드 ID 출력
        pthread_t thread_id = pthread_self();  // 현재 쓰레드 ID 얻기
        printf("Motor control thread ID: %lu\n", (unsigned long)thread_id);

        printf("Motor ID: %d, Command: %s\n", motor_thread->motor_id, motor_thread->command);

        handle_motor_command(motor_thread->motor_id, motor_thread->command); // 명령 처리

        // 명령 처리 후 다시 대기 상태로 돌아감
        motor_thread->is_active = 0;
        pthread_mutex_unlock(&motor_thread->mutex);
    }
    return NULL;
}

// 모터 제어 명령을 스레드에 전달하는 함수
void send_motor_command(int motor_id, const char* command) {
    if (motor_id < 0 || motor_id >= MAX_MOTOR_TYPES) {
        printf("Invalid motor ID\n");
        return;
    }

    MotorThread* motor_thread = &motor_threads[motor_id];

    pthread_mutex_lock(&motor_thread->mutex);

    // 새로운 명령을 전달하고 스레드를 활성화
    motor_thread->command = malloc(strlen(command) + 1);
    if (!motor_thread->command) {
        printf("[Error] Memory allocation failed\n");
        pthread_mutex_unlock(&motor_thread->mutex);
        return;
    }
    strcpy(motor_thread->command, command);

    motor_thread->is_active = 1;
    pthread_cond_signal(&motor_thread->cond);  // 명령이 도착했다고 알림

    pthread_mutex_unlock(&motor_thread->mutex);
}

int get_id_by_message(const char* message) {
    json_t* root;
    json_error_t error;

    // JSON 파싱
    root = json_loads(message, 0, &error);
    if (!root) {
        fprintf(stderr, "JSON parsing error: %s\n", error.text);
        return -1;
    }

    // ID 추출
    json_t* motor_id = json_object_get(root, "id");
    if (json_is_integer(motor_id)) {
        int id = json_integer_value(motor_id);
        json_decref(root);
        return id;
    } else {
        fprintf(stderr, "Missing or invalid 'id' field\n");
        json_decref(root);
        return -1;
    }
}




// main 함수는 그대로 두고, 모터 제어와 스레드 관리 부분을 분리하여 구현
int main(int argc, char **argv)
{
    printf("server init\n");
    
    mqd_t mq;
    struct mq_attr attr;
    char buf[BUFSIZ]; // 메시지를 저장할 공간
    int n; // 읽은 메시지의 길이
    int bQuit = 0; // 종료 여부를 저장할 변수

    
    // 메시지 큐 속성의 초기화
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = BUFSIZ;
    attr.mq_curmsgs = 0;
    
    // 메시지 큐 오픈
    mq = mq_open(MESSAGE_QUEUE_NAME, O_CREAT | O_RDONLY, 0644, &attr);
    if (mq == (mqd_t)-1) {
        perror("mq_open failed");
        return 1;
    }

    // 모터 제어 스레드를 초기화
    for (int i = 0; i < MAX_MOTOR_TYPES; i++) {
        motor_threads[i].motor_id = i;
        motor_threads[i].is_active = 0;
        pthread_mutex_init(&motor_threads[i].mutex, NULL);
        pthread_cond_init(&motor_threads[i].cond, NULL);
        pthread_create(&motor_threads[i].thread, NULL, motor_control_thread, &motor_threads[i]);
    }


    // 커스텀 장치 핸들러 등록
    register_custom_device_handler();
    
    while (!bQuit) {
        // 메시지 큐에서 데이터를 읽음
        memset(buf, 0, sizeof(buf)); // 메시지 큐로부터 읽은 후 버퍼 초기화
        n = mq_receive(mq, buf, sizeof(buf), NULL);
        if (n == -1) {
            perror("mq_receive failed");
            continue;
        }

        // 수신한 메시지가 'q'일 경우 종료
        if (buf[0] == 'q') {
            bQuit = 1;
        } else {
            int motor_id;
            // JSON 문자열을 구조체로 변환
            if ((motor_id = get_id_by_message(buf)) == -1) {
                fprintf(stderr, "JSON parsing error: %s\n", buf);
                continue; // 에러 발생 시 다음 메시지를 처리
            }

            // 변환된 데이터를 출력
            printf("[controller] Received Command: ID = %d, Message = %s\n", motor_id,buf);
            // 예시: motor_id가 0일 경우 첫 번째 모터에 명령 전달
            
            send_motor_command(motor_id, buf);
        }

        memset(buf, 0, sizeof(buf)); // buf 초기화
    }

    // 메시지 큐를 닫고 정리
    mq_close(mq);
    mq_unlink(MESSAGE_QUEUE_NAME);

    for (int i = 0; i < MAX_MOTOR_TYPES; i++) {
        pthread_cancel(motor_threads[i].thread);
        pthread_join(motor_threads[i].thread, NULL);
        pthread_mutex_destroy(&motor_threads[i].mutex);
        pthread_cond_destroy(&motor_threads[i].cond);
    }
    printf("server exit\n");
    return 0;
}  
