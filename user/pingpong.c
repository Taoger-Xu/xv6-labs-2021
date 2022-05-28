#include "kernel/types.h"
#include "user/user.h"
# define RD 0 // pipe的读端
# define WR 1 // pipe的写端
int main(int argc, char *argv[])
{
    char buf[1]; // 用于传输的字节
    int fd_c2p[2]; // 子进程 to 父进程
    int fd_p2c[2]; // 父进程 to 子进程

    pipe(fd_c2p);
    pipe(fd_p2c);

    int pid = fork();
    int exit_status = 0;

    if (pid < 0){
        fprintf(2, "fork() error!\n");
        close(fd_c2p[RD]); 
        close(fd_c2p[WR]);
        close(fd_p2c[RD]);
        close(fd_p2c[WR]);
        exit(1);
    }else if (pid == 0) {
        // 子进程
        close(fd_c2p[RD]);
        close(fd_p2c[WR]);
        // 从p2c管道中读取
        if (read(fd_p2c[RD], buf, sizeof(char)) != sizeof(char)){
            fprintf(2, "child read error");
            exit_status = 1;
        }else {
            fprintf(1, "%d: received ping\n", getpid());
        }
        // 向c2p管道写入
        if (write(fd_c2p[WR], buf, sizeof(char)) != sizeof(char)){
            fprintf(2, "child write error");
            exit_status = 1;
        }
        close(fd_c2p[WR]);
        close(fd_p2c[RD]);
        exit(exit_status);
    }
    else {
        // 父进程
        close(fd_c2p[WR]);
        close(fd_p2c[RD]);

        // 向p2c管道写入
        if (write(fd_p2c[WR], buf, sizeof(char)) != sizeof(char)){
            fprintf(2, "parent write error");
            exit_status = 1;
        }

        // 从c2p管道中读取
        if (read(fd_c2p[RD], buf, sizeof(char)) != sizeof(char)){
            fprintf(2, "parent read error");
            exit_status = 1;
        }else {
             fprintf(1, "%d: received pong\n", getpid());
        }
        
        close(fd_c2p[RD]);
        close(fd_p2c[WR]);
        exit(exit_status);
    }
}