#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>
#include <time.h>

#define MSG_TO_SVR "hey! wifidog"

int main(int argc, char* argv[]) {
    int sock_fd;
    const char* svr_addr = "192.168.197.207";
    char buf[32];
    struct sockaddr_in sockaddr;
    int n, num, i;

    srand(time(NULL));
    memset(buf, 0, sizeof(buf));
    if (argc > 1) {
        snprintf(buf, sizeof(buf), "%s (%s)", MSG_TO_SVR, argv[1]);
    } else {
        strcpy(buf, MSG_TO_SVR);
    }
restart:
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&sockaddr, 0, sizeof(sockaddr));
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(10010);
    inet_pton(AF_INET, svr_addr, &sockaddr.sin_addr);

    if ((connect(sock_fd, (struct sockaddr*)&sockaddr, sizeof(sockaddr))) < 0) {
        printf("failt to connect to server: %s\n", strerror(errno));
        sleep(1);
        goto restart;
    }

    sleep(rand() % 10 + 5);

    if (write(sock_fd, buf, strlen(buf)) < 0) {
        printf("send msg error: %s\n", strerror(errno));
    }

    sleep(rand() % 2 + 1);
    printf("close %d\n", sock_fd);
    close(sock_fd);
    goto restart;
}
