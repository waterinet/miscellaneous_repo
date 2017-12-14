#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <arpa/inet.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>
#include <signal.h>

/*
 * 测试什么情况下server read会挂住
 * 1) read过程中close，不会挂住
 * 2) 两个线程同时read，未读到数据的线程将会等待，此时server执行close不会唤醒，
 *    close之后fd被重新分配出去(server开启SO_REUSEADDR)也不会唤醒，只有对端close才能唤醒。
 *    对端如果是无线终端，在这种情况下解关联后server仍然不会被唤醒。
 * 3) client发送完消息后立即执行close，server端两个线程延迟read，其中一个线程读取到消息，另
 *    一个返回0，都不会挂住。
 */

#define BUF_SIZE 1024
#define FD_AGE   20
#define MAX_FD   1024
typedef void (*timer_handler_t)(sigval_t);

typedef struct thread_s {
    pthread_t t;
    int idx;
} thread_t;

typedef struct fd_s {
    long tm;
} fd_t;

int timer_init(timer_t* tid, timer_handler_t handler, int val);
int timer_set(timer_t tid, long interval_seconds);
void timer_handler(sigval_t data);
void* read_task(void* arg);

int con_fd;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
fd_t fds[MAX_FD + 1];

int main()
{
    int listen_fd, optval;
    struct sockaddr_in sockaddr;
    timer_t timer;
    thread_t thread[2];
    srand(time(NULL));
    signal(SIGPIPE, SIG_IGN);

    memset(&sockaddr, 0, sizeof(sockaddr));
    thread[0].idx = 0;
    thread[1].idx = 1;

    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    sockaddr.sin_port = htons(10010);

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd == -1) {
        printf("fail to create socket: %s\n", strerror(errno));
        exit(1);
    }
    
    /*
     * Allow a listener to be awakened only when data arrives on the socket. Takes an integer value (seconds), 
     * this can bound the maximum number of attempts TCP will make to complete the connection.  
     * This option should not be used in code intended to be portable.
     */
    optval = 3;     
    if (setsockopt(listen_fd, IPPROTO_TCP, TCP_DEFER_ACCEPT, &optval, sizeof(optval)) == -1) {
        printf("fail to setsockopt TCP_DEFER_ACCEPT: %s\n", strerror(errno));
    }
    /* 
     * This socket option tells the kernel that even if this port is busy (in the TIME_WAIT state), 
     * go ahead and reuse it anyway.  If it is busy, but with another state, you will still get 
     * an address already in use error.
     */
    optval = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
        printf("fail to setsockopt SO_REUSEADDR: %s\n", strerror(errno));
    }
    if (bind(listen_fd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) == -1) {
        printf("fail to bind: %s\n",  strerror(errno));
        close(listen_fd);
        exit(1);
    }
    if (listen(listen_fd, 1024) == -1) {
        printf("fail to listen: %s\n",  strerror(errno));
        close(listen_fd);
        exit(1);
    }

    timer_init(&timer, timer_handler, 0);
    timer_set(timer, 1000 * FD_AGE);
    pthread_create(&thread[0].t, NULL, read_task, &thread[0]);
    pthread_create(&thread[1].t, NULL, read_task, &thread[1]);
    while (1) {
        if ((con_fd = accept(listen_fd, NULL, NULL)) == -1) {
            printf("fail to accept socket: %s\n", strerror(errno));
            continue;
        }
        if (con_fd > MAX_FD) {
            close(con_fd);
            continue;
        }
        pthread_mutex_lock(&mtx);
        fds[con_fd].tm = time(NULL);
        printf("Accept socket %d(%ld)\n", con_fd, fds[con_fd].tm);
        pthread_mutex_unlock(&mtx);
    }

    close(listen_fd); /* not expected to execute */
}

void* read_task(void* arg)
{
    int n;
    char buf[BUF_SIZE];
    thread_t* p = (thread_t*)arg;

    printf("[%d]start to read...\n", p->idx);
    while (1) {
        if (con_fd <= 0) {
            sleep(1);
            continue;
        }
        sleep(rand() % 3 + 1);
        if ((n = read(con_fd, buf, BUF_SIZE - 1)) < 0) {
            printf("[%d]fail to read msg: %s\n", p->idx, strerror(errno));
            continue;
        }
        buf[n] = '\0';
        printf("[%d]read msg from client: %s\n", p->idx, buf);
    }
}

int timer_init(timer_t* tid, timer_handler_t handler, int val) {
    sigevent_t se;
    memset(&se, 0, sizeof(se));

    se.sigev_value.sival_int = val;
    se.sigev_notify = SIGEV_THREAD;
    se.sigev_notify_function = handler;
    if (timer_create(CLOCK_MONOTONIC, &se, tid) == -1) {
        return 0;
    }
    return 1;
}

int timer_set(timer_t tid, long milliseconds) {
    struct itimerspec its;
    memset(&its, 0, sizeof(its));
    its.it_value.tv_sec = 1;
    its.it_interval.tv_sec = milliseconds / 1000;
    its.it_interval.tv_nsec = milliseconds % 1000 * 1000000L;
    if (timer_settime(tid, 0, &its, NULL) == -1) {
        return 0;
    }
    return 1;
}

void timer_handler(sigval_t data)
{
    long now = time(NULL);
    int i;

    pthread_mutex_lock(&mtx);
    for (i = 0; i <= MAX_FD; i++) {
        if (fds[i].tm > 0 && fds[i].tm + FD_AGE < now) {
            if (close(i) == -1) {
                printf(">>>fail to close %d: %s\n", i, strerror(errno));
            } else {
                printf("!!!close %d(%ld,%ld)!!!\n", i, fds[i].tm, now);
                fds[i].tm = 0;
            }
        }
    }
    pthread_mutex_unlock(&mtx);
}

