#include <stdio.h>
#include <sys/wait.h>
#include <spawn.h>
extern char **environ;

int main()
{
    pid_t pid_r;
    pid_t pid_c;

    int status_r;
    int status_c;

    pid_t r_pid;
    pid_t c_pid;

    // 상대 경로로 수정
    char *r_argv[] = {"sudo", "./handler", NULL};
    char *c_argv[] = {"sudo", "./controller", NULL};
    
    // 상대 경로를 사용하여 posix_spawn 호출
    if (posix_spawn(&pid_r, "./handler", NULL, NULL, r_argv, environ) != 0) {
        perror("posix_spawn handler failed");
        return 1;
    }
    
    if (posix_spawn(&pid_c, "./controller", NULL, NULL, c_argv, environ) != 0) {
        perror("posix_spawn controller failed");
        return 1;
    }

    // 자식 프로세스 종료 대기
    if((r_pid = waitpid(pid_r, &status_r, 0)) < 0){ 
        printf("handler waitpid() error\n");
    }
    if((c_pid = waitpid(pid_c, &status_c, 0)) < 0){ 
        printf("controller waitpid() error\n");
    }

    // 자식 프로세스 상태 출력
    printf("Handler Child pid: %d, status: %d\n", r_pid, status_r);
    printf("Controller Child pid: %d, status: %d\n", c_pid, status_c);

    return 0;
}
