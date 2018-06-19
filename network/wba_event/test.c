#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>
#include "wba_evloop.h"

void timeout_test(wba_evtbase_t *base, long sec, long usec, s16 events, void *arg);
void io_accept_cb(s32 fd, s16 events, void *arg);

void timeout_cb(s32 fd, s16 events, void *arg)
{
    printf("timeout: fd %d, events %x\n", fd, events);
}
void timeout_cb2(s32 fd, s16 events, void *arg)
{
    wba_evt_t *evt = (wba_evt_t *)arg;
    printf("timeout: fd %d, events %x\n", fd, events);
    struct timeval tv = { 3, 100 };
    wba_event_add(evt, &tv);
}
void timeout_cb3(s32 fd, s16 events, void *arg)
{
    wba_evt_t *evt = (wba_evt_t *)arg;
    printf("timeout: fd %d, events %x\n", fd, events);
    wba_event_del(evt);
}
void timeout_cb4(s32 fd, s16 events, void *arg)
{
    if (arg) {
        wba_evt_t *evt = (wba_evt_t *)arg;
        wba_evtbase_t *base = evt->base;
        printf("timeout: fd %d, events %x\n", fd, events);
        wba_event_free(evt);
        evt = wba_event_new(base, -1, WBA_EVT_TIMEOUT | WBA_EVT_PERSIST, timeout_cb4, wba_event_selfptr());
        struct timeval tv = { rand() % 10 + 1, 10086 };
        wba_event_add(evt, &tv);

    }
}

void evtnew_test(wba_evtbase_t *base)
{
    //wba_evt_t *evt = wba_event_new(base, -1, 0, timeout_cb, NULL);
    //wba_evt_t *evt = wba_event_new(base, -1, WBA_EVT_PERSIST, timeout_cb, NULL);
    wba_evt_t *evt = wba_event_new(base, 0, WBA_EVT_WRITE, timeout_cb, NULL);
    if (!evt) {
        printf("wba_event_new error\n");
        return;
    }
    struct timeval tv = { 1, 1000 * 500 };
    wba_event_add(evt, &tv);
}

void timeout_test(wba_evtbase_t *base, long sec, long usec, s16 events, void *arg)
{
    wba_evt_t *evt = wba_event_new(base, -1, events, timeout_cb, arg);
    struct timeval tv = { sec, usec };
    wba_event_add(evt, &tv);
}
void timeout_test01(wba_evtbase_t *base)
{
    timeout_test(base, 2, 500, WBA_EVT_TIMEOUT, NULL);
    timeout_test(base, 4, 0, WBA_EVT_TIMEOUT, NULL);
    timeout_test(base, 8, 1000, WBA_EVT_TIMEOUT, NULL);
    timeout_test(base, 5, 1000, WBA_EVT_TIMEOUT | WBA_EVT_PERSIST, NULL);
    timeout_test(base, 15, 1000, WBA_EVT_PERSIST, NULL);
}

void timeout_test02(wba_evtbase_t *base)
{
    wba_evt_t *evt = wba_event_new(base, -1, WBA_EVT_TIMEOUT, timeout_cb2, wba_event_selfptr());
    struct timeval tv = { 3, 100 };
    wba_event_add(evt, &tv);

    timeout_test(base, 4, 500, WBA_EVT_TIMEOUT | WBA_EVT_PERSIST, NULL);
}

void timeout_test03(wba_evtbase_t *base)
{
    wba_evt_t *evt = wba_event_new(base, -1, WBA_EVT_TIMEOUT | WBA_EVT_PERSIST, timeout_cb3, wba_event_selfptr());
    struct timeval tv = { 3, 100 };
    wba_event_add(evt, &tv);

    timeout_test(base, 4, 500, WBA_EVT_TIMEOUT | WBA_EVT_PERSIST, NULL);
}

void timeout_test04(wba_evtbase_t *base)
{
    srand(time(0));
    //timeout_test(base, 5, 500, WBA_EVT_TIMEOUT | WBA_EVT_PERSIST, NULL);
    wba_evt_t *evt = wba_event_new(base, -1, WBA_EVT_TIMEOUT | WBA_EVT_PERSIST, timeout_cb4, wba_event_selfptr());
    struct timeval tv = { rand() % 10 + 1, 10086 };
    wba_event_add(evt, &tv);
}

void io_start_listen(wba_evtbase_t *base)
{
    s32 fd;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("fail to create sock\n");
        exit(1);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(5004);
    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("fail to bind\n");
        exit(1);
    }

    if (listen(fd, 10) == -1) {
        perror("fail to listen\n");
        exit(1);
    }
    
    wba_evt_t *evt = wba_event_new(base, fd, WBA_EVT_READ | WBA_EVT_PERSIST, io_accept_cb, wba_event_selfptr());
    wba_event_add(evt, NULL);
}

s16 io_events = 0;
void (*io_rw_cb)(s32 fd, s16 events, void *arg) = NULL;
void io_accept_cb(s32 fd, s16 events, void *arg)
{
    if (arg) {
        wba_evt_t *evt = (wba_evt_t*)arg;
        if (events & WBA_EVT_READ) {
            s32 new_fd = accept(fd, NULL, NULL);
            if (new_fd == -1) {
                printf("fail to accept from %d: %s\n", fd, strerror(errno));
                return;
            }
            printf("accept fd %d\n", new_fd);
            wba_evt_t *new = wba_event_new(evt->base, new_fd, io_events, io_rw_cb, wba_event_selfptr());
            struct timeval tv = { 60, 10086 };
            wba_event_add(new, &tv); 
        }
    }
}

void io_read_cb01(s32 fd, s16 events, void *arg)
{
    char buf[1024] = { 0 };

    wba_evt_t *evt = (wba_evt_t*)arg;
    if (events & WBA_EVT_TIMEOUT) {
        printf("event %p timeout!\n", evt);
        wba_event_free(evt);
    } else if (events & WBA_EVT_READ) {
        ssize_t ret = read(fd, buf, sizeof(buf));
        if (ret == 0) {
            printf("EOF from %d\n", fd);
            wba_event_free(evt);
        } else if (ret == -1) {
            printf("fail to read from %d: %s\n", fd, strerror(errno));
            wba_event_free(evt);
        } else {
            printf("read from fd: %s\n", buf);
            struct timeval tv = { 60, 10086 };
            wba_event_add(evt, &tv);
        }
    }
}

void io_read_cb02(s32 fd, s16 events, void *arg)
{
    wba_evt_t *evt = (wba_evt_t*)arg;
    if (events & WBA_EVT_CLOSED) {
        printf("peer closed, fd: %d\n", fd);
        wba_event_free(evt);
    }
}

void io_read_cb03(s32 fd, s16 events, void *arg)
{
    wba_evt_t *evt = (wba_evt_t*)arg;
    static char buf[1024];

    if (events & WBA_EVT_READ) {
        memset(buf, 0, sizeof(buf));
        ssize_t ret = read(fd, buf, sizeof(buf));
        if (ret <= 0) {
            printf("fail to read from %d: %s\n", fd, strerror(errno));
            wba_event_free(evt);
        } else {
            printf("read from fd: %s\n", buf);
            struct timeval tv = { 60, 10086 };
            wba_event_assign(evt, evt->base, fd, WBA_EVT_WRITE, io_read_cb03, evt); 
            wba_event_add(evt, &tv);
        }
    } else if (events & WBA_EVT_WRITE) {
        ssize_t ret = write(fd, buf, strlen(buf));
        if (ret <= 0) {
            printf("fail to write to %d: %s\n", fd, strerror(errno));
            wba_event_free(evt);
        } else {
            printf("write to fd: %s\n", buf);
            struct timeval tv = { 60, 10086 };
            wba_event_assign(evt, evt->base, fd, WBA_EVT_READ, io_read_cb03, evt);
            wba_event_add(evt, &tv);
        }
    }
}

void io_read_cb04(s32 fd, s16 events, void *arg)
{
    char buf[1024] = { 0 };

    wba_evt_t *evt = (wba_evt_t*)arg;
    if (events & WBA_EVT_TIMEOUT) {
        printf("event %p timeout!\n", evt);
        wba_event_free(evt);
    } else if (events & WBA_EVT_READ) {
        ssize_t ret = read(fd, buf, sizeof(buf));
        if (ret <= 0) {
            printf("fail to read from %d: %s\n", fd, strerror(errno));
            wba_event_free(evt);
        } else {
            printf("read from fd: %s\n", buf);
        }
    }
}

void io_read_cb05(s32 fd, s16 events, void *arg)
{
    char buf[1024] = { 0 };
    wba_evt_t *evt = (wba_evt_t*)arg;

    if (events & WBA_EVT_READ) {
        ssize_t ret = read(fd, buf, sizeof(buf));
        if (ret <= 0) {
            printf("fail to read from %d: %s\n", fd, strerror(errno));
            wba_event_free(evt);
        } else {
            printf("read from fd: %s\n", buf);
        }
    }

    if (events & WBA_EVT_WRITE) {
        printf("%d ready to write\n", fd);
        if (strlen(buf) > 0) {
            ssize_t ret = write(fd, buf, strlen(buf));
            if (ret <= 0) {
                printf("fail to write to %d: %s\n", fd, strerror(errno));
                wba_event_free(evt);
            } else {
                printf("write to fd: %s\n", buf);
            }
        }
        sleep(2);
    }
}

void evtbase_test()
{
   wba_evtbase_t *base;
   wba_evt_t evt[2];
   s32 i;

   // /proc/sys/fs/epoll/max_user_watches
   while (1) {
       base = wba_evtbase_new();
       wba_event_assign(&evt[0], base, 0, WBA_EVT_READ, timeout_cb, NULL);
       wba_event_assign(&evt[1], base, -1, WBA_EVT_TIMEOUT, timeout_cb, NULL);
       struct timeval tv = { 0, 100000 };
       wba_event_add(&evt[0], &tv);
       wba_event_add(&evt[1], &tv);
       //wba_event_loop(base);
       wba_evtbase_free(base);
   }

}

int main() 
{
    struct timeval tv;
    wba_evtbase_t *base = wba_evtbase_new();
    if (!base) {
        perror("wba_evtbase_new\n");
        exit(1);
    }


    //evtnew_test(base);

    /* timeout */
    //timeout_test01(base);
    //timeout_test02(base);
    //timeout_test03(base);
    //timeout_test04(base);

    /* io */
    //io_events = WBA_EVT_READ;
    //io_rw_cb = io_read_cb01;
    //io_start_listen(base);

    //io_events = WBA_EVT_CLOSED;
    //io_rw_cb = io_read_cb02;
    //io_start_listen(base);

    //io_events = WBA_EVT_READ;
    //io_rw_cb = io_read_cb03; // echo
    //io_start_listen(base);

    //io_events = WBA_EVT_READ | WBA_EVT_PERSIST;
    //io_rw_cb = io_read_cb04; // persist read
    //io_start_listen(base);

    //io_events = WBA_EVT_READ | WBA_EVT_PERSIST | WBA_EVT_WRITE;
    //io_rw_cb = io_read_cb05;
    //io_start_listen(base); // read & write without ET

    //wba_event_loop(base);

    evtbase_test();
}

