#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <libgen.h>

int main(int argc, char *argv[])
{
    int fd0 = 0, fd1 = 0;
    char rbuf[100];

    if (argc != 3){
        printf("Usage : %s <path> <path>\n", basename(argv[0]));
        return -1;
    }

    fd0 = open(argv[1], O_RDWR);
    if(fd0 < 0){
        printf("open failed!\n");
        return -1;
    }

    fd1 = open(argv[2], O_RDWR);
    if(fd1 < 0){
        printf("open failed!\n");
        return -1;
    }

    printf("open successed fd0 = %d\n", fd0);
    printf("open successed fd1 = %d\n", fd1);
    write(fd0, "Hello fd0!\n", 100);
    write(fd1, "World fd1!\n", 100);

    bzero(rbuf, 100);
    read(fd0, rbuf, 100);
    printf("fd0 read user space from kernel: %s\n", rbuf);

    bzero(rbuf, 100);
    read(fd1, rbuf, 100);
    printf("fd1 read user space from kernel: %s\n", rbuf);

    close(fd0);
    close(fd1);
    return 0;
}
