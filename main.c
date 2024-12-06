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

    char *r_argv[] = {"sudo","./handler", NULL};
    char *c_argv[] = {"sudo","./controller", NULL};
    
    posix_spawn(&pid_r, "/home/pi/rc_car_project/handler", NULL, NULL, r_argv, environ);
    posix_spawn(&pid_c, "/home/pi/rc_car_project/controller", NULL, NULL, c_argv, environ);
    if((r_pid = waitpid(pid_r, &status_r, 0)) < 0){ // 자식 프로세스의 종료 대기
        printf("handler waitpid() error\n");
    }
    if((c_pid = waitpid(pid_c, &status_c, 0)) < 0){ // 자식 프로세스의 종료 대기
    printf("controller waitpid() error\n");
    }
    printf("Child pid: %d, status: %d\n", r_pid, status_r);
    printf("Child pid: %d, status: %d\n", r_pid, status_c);
    return 0;
}