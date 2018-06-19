#include "wba_evloop.h"
#include <sys/epoll.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* Since Linux 2.6.8, the size argument is ignored, but must be greater than zero */
#define EVT_POOL_SIZE   1024
#define EVT_NUM_EVENTS  32
#define EVT_MAP_SIZE    32
#define EVT_DEF_WAIT_MS (60 * 1000)

#define event_debug(fmt, args...) do { \
        struct timeval tv;  \
        event_timeget(&tv); \
        printf("[%d.%d][%s]"fmt"\n", tv.tv_sec, tv.tv_usec, __func__, ##args); \
    } while (0)

static s32 event_timeget(struct timeval *tv);
static void *event_selfptr = NULL;

static char *event_2str(wba_evt_t *evt)
{
    static char buf[1024];
    snprintf(buf, sizeof(buf), 
            "event %p, fd %d, flags %s%s%s%s, events %s%s%s%s%s, call %p",
            evt, evt->fd,
            evt->flags & WBA_EVTLIST_INIT    ? "I" : "_",
            evt->flags & WBA_EVTLIST_REG     ? "R" : "_",
            evt->flags & WBA_EVTLIST_ACTIVE  ? "A" : "_",
            evt->flags & WBA_EVTLIST_TIMEOUT ? "T" : "_",
            evt->events & WBA_EVT_READ       ? "R" : "_",
            evt->events & WBA_EVT_WRITE      ? "W" : "_",
            evt->events & WBA_EVT_TIMEOUT    ? "T" : "_",
            evt->events & WBA_EVT_CLOSED     ? "C" : "_",
            evt->events & WBA_EVT_PERSIST    ? "P" : "_",
            evt->cb.fn);
    return buf;
}

static s32 event_register(wba_evt_t *evt)
{
    struct epoll_event ev;

    memset(&ev, 0, sizeof(struct epoll_event));
    ev.data.fd = evt->fd;
    if (evt->events & WBA_EVT_READ) {
        ev.events |= EPOLLIN;
    }
    if (evt->events & WBA_EVT_WRITE) {
        ev.events |= EPOLLOUT;
    }
    if (evt->events & WBA_EVT_CLOSED) {
        ev.events |= EPOLLRDHUP;
    }

    if (epoll_ctl(evt->base->epfd, EPOLL_CTL_ADD, evt->fd, &ev) == -1) {
        event_debug("fail to register, fd %d, events %d: %s\n", evt->fd, evt->events,
                strerror(errno));
        return 0;
    }
    return 1;
}

static s32 event_unregister(wba_evt_t *evt) 
{
    if (epoll_ctl(evt->base->epfd, EPOLL_CTL_DEL, evt->fd, NULL) == -1) {
        event_debug("fail to unregister, fd %d, events %d: %s\n", evt->fd, evt->events,
                strerror(errno));
        return 0;
    }
    return 1;
}

static s32 event_timeget(struct timeval *tv)
{
    struct timespec ts;

    if (clock_gettime(CLOCK_MONOTONIC, &ts) == -1) {
        return 0;
    }

    tv->tv_sec = ts.tv_sec;
    tv->tv_usec = ts.tv_nsec / 1000;
    return 1;
}

static void event_timeadd(const struct timeval *a, const struct timeval *b, struct timeval *tv)
{
    tv->tv_sec = a->tv_sec + b->tv_sec;
    tv->tv_usec = a->tv_usec + b->tv_usec;
    if (tv->tv_usec >= 1000000) {
        tv->tv_sec++;
        tv->tv_usec -= 1000000;
    }
}

static void event_timesub(const struct timeval *a, const struct timeval *b, struct timeval *tv)
{
    tv->tv_sec = a->tv_sec - b->tv_sec;
    tv->tv_usec = a->tv_usec - b->tv_usec;
    if (tv->tv_usec < 0) {
        tv->tv_sec--;
        tv->tv_usec += 1000000;
    }
}

static s32 event_timecmp(const struct timeval *a, const struct timeval *b)
{
    if (a->tv_sec != b->tv_sec) {
        if (a->tv_sec > b->tv_sec) {
            return 1;
        } else {
           return -1;
        }
    } else if (a->tv_usec != b->tv_usec) {
        if (a->tv_usec > b->tv_usec) {
            return 1;
        } else {
            return -1;
        }
    } else {
        return 0;
    }
}

static void event_add_active(wba_evt_t *evt, s16 res)
{
    wba_evtbase_t *base;

    base = evt->base;

    if (!(evt->flags & WBA_EVTLIST_ACTIVE)) {
        evt->res = res;
        list_add_tail(&evt->evt_active_next, &base->evt_active_list);
        evt->flags |= WBA_EVTLIST_ACTIVE;
        base->event_count_active++;
    }
}

static void event_remove_active(wba_evt_t *evt)
{
    if (evt->flags & WBA_EVTLIST_ACTIVE) {
        list_del_init(&evt->evt_active_next);
        evt->flags &= ~WBA_EVTLIST_ACTIVE;
        evt->base->event_count_active--;
    }
}

static struct list_head *event_map_get_slot(wba_evtbase_t *base, s32 fd)
{
    u32 index;

    if (fd < 0) {
        return NULL;
    }

    index = fd & base->evt_map.sizemask;
    return &base->evt_map.evt_list[index];
}

/*
 *  0 : nothing changed
 * -1 : fail
 *  1 : success
 */
static s32 event_map_add(wba_evt_t *evt)
{
    struct list_head *list;
    s32 res = 0;

    if (evt->fd < 0) {
        return 0;
    }

    if (evt->events & (WBA_EVT_READ | WBA_EVT_WRITE| WBA_EVT_CLOSED)) {
        if (!event_register(evt)) {
            event_debug("fail to register event\n");
            return -1;
        }
    } else {
        return 0;
    }
    list = event_map_get_slot(evt->base, evt->fd);
    list_add_tail(&evt->evt_next, list);
    evt->base->evt_map.used++;
    evt->base->event_count++;
    return 1;
}

/*
 *  0 : nothing changed
 * -1 : fail
 *  1 : success
 */
static s32 event_map_remove(wba_evt_t *evt)
{
    s32 res;

    if (evt->fd < 0) {
        return 0;
    }

    res = 1;
    if (evt->events & (WBA_EVT_READ | WBA_EVT_WRITE| WBA_EVT_CLOSED)) {
        if (!event_unregister(evt)) {
            /* closing fd will cause it to be removed from epoll set, so don't worry about it */
            event_debug("warning: fail to unregister event\n");
            res = -1;
        }
    } else {
        return 0;
    }
    list_del_init(&evt->evt_next);
    evt->base->evt_map.used--;
    evt->base->event_count--;
    return res;
}

static void event_map_active(wba_evtbase_t *base, s32 fd, s16 events)
{
    struct list_head *list;
    wba_evt_t *pos;

    list = event_map_get_slot(base, fd);
    if (!list) {
        return;
    }

    list_for_each_entry(pos, list, evt_next) {
        if (pos->events & events) {
            event_add_active(pos, pos->events & events);
            event_debug("active %s, res %x", event_2str(pos), pos->events & events);
        }
    }
}

static void event_add_timeout(wba_evt_t *new)
{ 
    struct rb_root *root = &new->base->evt_time_root;
    struct rb_node **link = &root->rb_node;
    struct rb_node *parent = NULL;
    wba_evt_t *evt;

    if (new->flags & WBA_EVTLIST_TIMEOUT) {
        return;
    }

    while (*link) {
        parent = *link;
        evt = rb_entry(parent, wba_evt_t, evt_time_node);
        s32 ret = event_timecmp(&new->evt_timeout, &evt->evt_timeout);
        if (ret < 0) {
            link = &(*link)->rb_left;
        } else {
            link = &(*link)->rb_right;
        }
    }

    rb_link_node(&new->evt_time_node, parent, link);
    rb_insert_color(&new->evt_time_node, root);

    new->flags |= WBA_EVTLIST_TIMEOUT;
    new->base->event_count++;
}

static void event_remove_timeout(wba_evt_t *evt)
{
    if (evt->flags & WBA_EVTLIST_TIMEOUT) {
        rb_erase(&evt->evt_time_node, &evt->base->evt_time_root);
        evt->flags &= ~WBA_EVTLIST_TIMEOUT;
        evt->base->event_count--;
    }
}

wba_evtbase_t *wba_evtbase_new()
{
    wba_evtbase_t *base;
    wba_evtmap map;
    s32 epfd;
    s32 i;

    epfd = epoll_create(EVT_POOL_SIZE);
    if (epfd == -1) {
        event_debug("fail to create epoll: %s", strerror(errno));
        return NULL;
    }

    base = (wba_evtbase_t *)malloc(sizeof(wba_evtbase_t));
    if (!base) {
        return NULL;
    }

    memset(base, 0, sizeof(wba_evtbase_t));
    base->epfd = epfd;
    base->nevts = EVT_NUM_EVENTS;
    base->events = (struct epoll_event*)malloc(sizeof(struct epoll_event) * base->nevts);
    if (!base->events) {
        free(base);
        return NULL;
    }

    base->evt_map.size = EVT_MAP_SIZE;
    base->evt_map.sizemask = EVT_MAP_SIZE - 1;
    base->evt_map.evt_list = 
        (struct list_head*)malloc(sizeof(struct list_head) * base->evt_map.size);
    if (!base->evt_map.evt_list) {
        free(base->events);
        free(base);
        return NULL;
    }
    for (i = 0; i < base->evt_map.size; i++) {
        INIT_LIST_HEAD(&base->evt_map.evt_list[i]);
    }

    base->evt_time_root = RB_ROOT;
    INIT_LIST_HEAD(&base->evt_active_list);

    return base;
}

void *wba_event_selfptr()
{
    return &event_selfptr;
}

/* if events is 0, this event can be triggered only by a timeout */
wba_evt_t *wba_event_new(wba_evtbase_t *base, s32 fd, s16 events, wba_evtcb_fn fn, void *arg)
{
    wba_evt_t *evt;

    if (!base || !fn) {
        return NULL;
    }

    if (fd < 0) {
        fd = -1;
    }

    evt = (wba_evt_t *)malloc(sizeof(wba_evt_t));
    if (!evt) {
        return NULL;
    }

    memset(evt, 0, sizeof(wba_evt_t));
    evt->base = base;
    evt->fd = fd;
    evt->cb.fn = fn;
    if (arg == &event_selfptr) {
        evt->cb.arg = evt;
    } else {
        evt->cb.arg = arg;
    }
    evt->events = events;
    evt->flags |= WBA_EVTLIST_INIT;

    return evt;
}

s32 wba_event_assign(wba_evt_t *evt, wba_evtbase_t *base, s32 fd, s16 events, 
        wba_evtcb_fn fn, void *arg)
{
    if (!evt || !base || !fn) {
        return 0;
    }

    if (fd < 0) {
        fd = -1;
    }

    memset(evt, 0, sizeof(wba_evt_t));
    evt->base = base;
    evt->fd = fd;
    evt->cb.fn = fn;
    evt->cb.arg = arg;
    evt->events = events;
    evt->flags |= WBA_EVTLIST_INIT;

    return 1;
}

/* readd an event:
 * tv is not NULL: reschedule it with the provided timeout 
 * tv is NULL    : no effect
 */ 
static s32 event_add(wba_evt_t *evt, const struct timeval *tv, s32 tv_is_absolute)
{
    s32 res = 0;
    struct timeval now;

    if (!evt || !evt->base) {
        return 0;
    }

    event_debug("%s, is_absolute %d", event_2str(evt), tv_is_absolute);

    if ((evt->flags & WBA_EVTLIST_REG) && !tv) {
        event_debug("event already pending, fd %d", evt->fd);
        return 1;
    }

    if (!(evt->flags & WBA_EVTLIST_REG)) {
        res = event_map_add(evt);
    }

    if (res != -1) {
        evt->flags |= WBA_EVTLIST_REG;
    }

    if (res != -1 && tv) {
        if (evt->flags & WBA_EVTLIST_TIMEOUT) {
            event_remove_timeout(evt);
        }

        if (tv_is_absolute) {
            evt->evt_timeout = *tv;
        } else {
            if (evt->events & WBA_EVT_PERSIST) {
                evt->timeout = *tv;
            }
            event_timeget(&now);
            event_timeadd(&now, tv, &evt->evt_timeout);
        } 
        event_debug("event %p timeout in %d seconds %d useconds",
                evt, evt->evt_timeout.tv_sec, evt->evt_timeout.tv_usec);
        event_add_timeout(evt);
        res = 1;
    }

    return (res > 0 ? res : 0);
}

s32 wba_event_add(wba_evt_t *evt, const struct timeval *tv)
{
    if (tv && (tv->tv_sec < 0 || tv->tv_usec < 0)) {
        return 0;
    }
    return event_add(evt, tv, 0);
}

static s32 event_del(wba_evt_t *evt)
{
    s32 res = 0;

    if (!evt || !evt->base) {
        return 0;
    }

    event_debug("%s", event_2str(evt));

    if (!(evt->flags & (WBA_EVTLIST_REG | WBA_EVTLIST_ACTIVE))) {
        event_debug("no pending or active event");
        return 1; 
    }

    if (evt->flags & WBA_EVTLIST_TIMEOUT) {
        event_remove_timeout(evt);
    }

    if (evt->flags & WBA_EVTLIST_ACTIVE) {
        event_remove_active(evt);
    }

    if (evt->flags & WBA_EVTLIST_REG) {
        res = event_map_remove(evt);
        evt->flags &= ~WBA_EVTLIST_REG;
    }

    return (res > 0 ? res : 0);
}

s32 wba_event_del(wba_evt_t *evt)
{
    return event_del(evt);
}

void wba_event_free(wba_evt_t *evt) 
{
    event_del(evt);
    if (evt) {
        event_debug("free event %p", evt);
        free(evt);
    }
}

/* does not free any events associated with this base */
void wba_evtbase_free(wba_evtbase_t *base)
{
    wba_evt_t *evt, *te;
    struct rb_node *node, *tn;
    s32 i, cnt = 0;

    if (!base) {
        return;
    }

    for (i = 0; i < base->evt_map.size; i++) {
        list_for_each_entry_safe(evt, te, &base->evt_map.evt_list[i], evt_next) {
            event_del(evt);
            cnt++;
        }
    }
    free(base->evt_map.evt_list);

    node = rb_first(&base->evt_time_root);
    while (node) {
        tn = node;
        node = rb_next(node); /* need to be safe against removal */
        evt = rb_entry(tn, wba_evt_t, evt_time_node);
        event_del(evt);
        cnt++;
    }

    event_debug("%d events were still set in this base", cnt);

    free(base->events);
    close(base->epfd);
    free(base);
}

/* timeout in milliseconds */
static s32 event_next_timeout(wba_evtbase_t *base) 
{
    struct rb_node *p;
    wba_evt_t *evt;
    struct timeval now;
    struct timeval tv;
    s32 timeout = EVT_DEF_WAIT_MS;

    p = rb_first(&base->evt_time_root);
    if (!p || !event_timeget(&now)) {
        return timeout;
    }

    evt = rb_entry(p, wba_evt_t, evt_time_node);
    if (event_timecmp(&evt->evt_timeout, &now) <= 0) {
        return 0; /* epoll_wait will return immediately */
    }

    event_timesub(&evt->evt_timeout, &now, &tv);
    /* round up to the nearest millisecond no matter what the microsecond part is. 
     * make sure that the waiting time is enough */ 
    timeout = tv.tv_sec * 1000 + tv.tv_usec / 1000 + 1;

    return timeout; 
}

static s32 event_process_timeout(wba_evtbase_t *base)
{
    s32 cnt = 0;
    struct timeval now;
    struct rb_node *node;
    struct rb_node *p;
    wba_evt_t *evt;

    if (!event_timeget(&now)) {
        return 0;
    }

    node = rb_first(&base->evt_time_root);
    while (node) {
        p = node;
        node = rb_next(node); /* need to be safe against removal */

        evt = rb_entry(p, wba_evt_t, evt_time_node);
        if (event_timecmp(&evt->evt_timeout, &now) > 0) {
            break;
        }
        event_debug("event %p is timeout", evt);
        /* timed out event need to be removed and rescheduled(if it is persist) */
        event_del(evt); 
        event_add_active(evt, WBA_EVT_TIMEOUT);
        cnt++;
    }
    return cnt;
}

static s32 event_process_active(wba_evtbase_t *base)
{
    s32 cnt = 0;
    wba_evt_t *evt, *p;
    struct timeval last, now, next;

    list_for_each_entry_safe(evt, p, &base->evt_active_list, evt_active_next) {
        if (evt->events & WBA_EVT_PERSIST) {
            event_remove_active(evt);
            /* reshedule persisitent event */
            if (evt->timeout.tv_sec || evt->timeout.tv_usec) {
                event_timeget(&now);
                if (evt->res & WBA_EVT_TIMEOUT) {
                    /* run at a interval of evt->timeout if there was a timeout */
                    last = evt->evt_timeout;
                } else {
                    last = now;
                }
                event_timeadd(&last, &evt->timeout, &next);
                if (event_timecmp(&next, &now) < 0) {
                    event_timeadd(&now, &evt->timeout, &next);
                }
                event_add(evt, &next, 1);
            }
        } else {
            event_del(evt);
        }

        evt->cb.fn(evt->fd, evt->res, evt->cb.arg);
        cnt++;
    }

    return cnt;
}

s32 wba_event_loop(wba_evtbase_t *base)
{
    s32 done = 0;
    s32 timeout;
    s32 res = 1;
    s32 i;
    s32 nto, ncb;

    if (!base) {
        return 0;
    }

    while (!done) {
        if (!base->event_count && !base->event_count_active) {
            event_debug("empty base");
            break;
        }

        event_debug(">>> event_loop: event_count %u, active_count %u, map_used %u", 
                base->event_count, base->event_count_active, base->evt_map.used);
        timeout = event_next_timeout(base);
        event_debug("next timeout from now in millisecond: %d", timeout);
        res = epoll_wait(base->epfd, base->events, base->nevts, timeout);
        if (res == -1) {
            event_debug("epoll error: %s", strerror(errno));
            res = 0;
            break;
        }

        for (i = 0; i < res; i++) {
            u32 what = base->events[i].events;
            s16 events = 0;

            if (what & (EPOLLHUP|EPOLLERR)) {
                events = WBA_EVT_READ | WBA_EVT_WRITE;
            } else {
                if (what & EPOLLIN)
                    events |= WBA_EVT_READ;
                if (what & EPOLLOUT)
                    events |= WBA_EVT_WRITE;
                if (what & EPOLLRDHUP)
                    events |= WBA_EVT_CLOSED;
            }

            if (!events) {
                continue;
            }
            
            event_map_active(base, base->events[i].data.fd, events);
        }

        nto = event_process_timeout(base);
        ncb = event_process_active(base);
        event_debug("<<< event_loop: timeouts %d, callbacks %d", nto, ncb);
        event_debug("----------");
    }

    return res;
}


