/* 
 * simple event demultiplexor implementation using epoll
 * single-threaded, no lock
 */
#include <sys/time.h>
#include "rbtree.h"
#include "list.h"

#define WBA_EVT_READ     0x01 
#define WBA_EVT_WRITE    0x02
#define WBA_EVT_TIMEOUT  0x04
#define WBA_EVT_CLOSED   0x08
#define WBA_EVT_PERSIST  0x10
#define WBA_EVT_ALL      0xff 

#define WBA_EVTLIST_INIT      0x01
#define WBA_EVTLIST_REG       0x02
#define WBA_EVTLIST_ACTIVE    0x04
#define WBA_EVTLIST_TIMEOUT   0x08  /* in time tree */

typedef int s32;
typedef unsigned int u32;
typedef short s16;

typedef void (*wba_evtcb_fn)(s32, s16, void *);

typedef struct wba_evtmap {
    struct list_head *evt_list;
    u32 size;
    u32 sizemask;
    u32 used;
} wba_evtmap;

typedef struct wba_evtbase {
    s32 epfd;
    s32 nevts;
    struct epoll_event *events;
    wba_evtmap evt_map;
    struct rb_root evt_time_root;
    struct list_head evt_active_list;
    /* total events added to this base, not exact, just show system load */
    u32 event_count; 
    u32 event_count_active;
} wba_evtbase_t;

struct wba_evtcb {
    void (*fn)(s32, s16, void *);
    void *arg;
};

typedef struct wba_evt {
    struct list_head evt_next;
    struct rb_node evt_time_node;
    struct list_head evt_active_next; /* active list */
    struct wba_evtcb cb;
    s32 fd;
    struct wba_evtbase *base;
    s16 flags;
    s16 events; /* registered events */
    s16 res;    /* activated events */
    struct timeval timeout;          /* timeout set by user */
    struct timeval evt_timeout;      /* timeout used for cmp */
} wba_evt_t;

wba_evtbase_t *wba_evtbase_new();

void *wba_event_selfptr();

wba_evt_t *wba_event_new(wba_evtbase_t *base, s32 fd, s16 events, wba_evtcb_fn, void *);

s32 wba_event_assign(wba_evt_t *, wba_evtbase_t *, s32 fd, s16 events, wba_evtcb_fn, void *);

s32 wba_event_add(wba_evt_t *, const struct timeval *tv);

s32 wba_event_del(wba_evt_t *);

void wba_event_free(wba_evt_t *);

void wba_evtbase_free(wba_evtbase_t *base);

s32 wba_event_loop(wba_evtbase_t *base);

