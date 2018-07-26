#include "wba_str.h"
#include <stdlib.h>

#define WBA_STR_MAX_RESERVE_SIZE  (1024*1024)

/* If NULL is used for 'init' the string is initialized with zero bytes */
wba_str_t *wba_strnewlen(const void *init, size_t initlen)
{
    wba_str_t *ws = (wba_str_t *)malloc(sizeof(wba_str_t));
    if (!ws) {
        return NULL;
    }
    ws->buf = (s8 *)malloc(initlen + 1);
    if (!ws->buf) {
        free(ws);
        return NULL;
    }
    ws->len = initlen;
    ws->cap = initlen;
    if (!init) {
        memset(ws->buf, 0, initlen);
    } else {
        memcpy(ws->buf, init, initlen);
    }
    ws->buf[initlen] = '\0';
    return ws;
}

wba_str_t *wba_strnew(size_t cap)
{
    wba_str_t *ws = (wba_str_t *)malloc(sizeof(wba_str_t));
    if (!ws) {
        return NULL;
    }
    ws->buf = (s8 *)malloc(cap + 1);
    if (!ws->buf) {
        free(ws);
        return NULL;
    }
    ws->len = 0;
    ws->cap = cap;
    memset(ws->buf, 0, cap + 1);
    return ws;
}

wba_str_t *wba_strnew2(const s8 *str)
{
    size_t len = str ? strlen(str) : 0;
    return wba_strnewlen(str, len);
}

void wba_strfree(wba_str_t *ws)
{
    if (ws) {
        if (ws->buf) {
            free(ws->buf);
        }
        free(ws);
    }
}

void wba_strclr(wba_str_t* ws)
{
    if (ws) {
        ws->len = 0;
        ws->buf[0] = '\0';
    }
}

wba_str_t *wba_strgrow(wba_str_t* ws, size_t cap)
{
    s8* buf;

    if (!ws || cap <= ws->cap) {
        return ws;
    }

    buf = (s8 *)realloc(ws->buf, cap + 1);
    if (!buf) {
        return NULL;
    }
    ws->buf = buf;
    ws->cap = cap;
    return ws;
}

/*
 * reserve space for future use
 * cap: minimal capacity required by now
 */
static wba_str_t *wba_str_reserve(wba_str_t* ws, size_t cap)
{
    if (ws->cap >= cap) {
        return ws;
    }

    if (cap < WBA_STR_MAX_RESERVE_SIZE) {
        return wba_strgrow(ws, cap * 2);
    } else {
        return wba_strgrow(ws, cap + WBA_STR_MAX_RESERVE_SIZE);
    }
}

/* str is is binary safe string */
wba_str_t *wba_strncpy(wba_str_t *ws, const s8 *str, size_t len)
{
    if (!ws || !str || len == 0) {
        return ws;
    }

    if (!wba_str_reserve(ws, len)) {
        return NULL;
    }
    memcpy(ws->buf, str, len);
    ws->len = len;
    ws->buf[ws->len] = '\0';

    return ws;
}

/* str is null-terminated string */
wba_str_t *wba_strcpy(wba_str_t *ws, const s8 *str)
{
    if (!ws || !str) {
        return ws;
    }

    return wba_strncpy(ws, str, strlen(str));
}

/* str is is binary safe string */
wba_str_t *wba_strncat(wba_str_t *ws, const void *str, size_t len)
{
    if (!ws || !str || len == 0) {
        return ws;
    }

    if (!wba_str_reserve(ws, ws->len + len)) {
        return NULL;
    }
    memcpy(ws->buf + ws->len, str, len);
    ws->len += len;
    ws->buf[ws->len] = '\0';

    return ws;
}

/* str is null-terminated string */
wba_str_t *wba_strcat(wba_str_t *ws, const s8 *str)
{
    if (!ws || !str) {
        return ws;
    }

    return wba_strncat(ws, str, strlen(str));
}

wba_str_t *wba_strcat_vprintf(wba_str_t *ws, const s8 *fmt, va_list va)
{
    va_list vc;
    s32 len; 

    if (!ws) {
        return NULL;
    }
    va_copy(vc, va);
    len = vsnprintf(NULL, 0, fmt, vc);
    if (len < 0) {
        va_end(vc);
        return NULL;
    }
    va_end(vc);

    if (!wba_str_reserve(ws, ws->len + len)) {
        return NULL;
    }

    va_copy(vc, va);
    vsnprintf(ws->buf + ws->len, len + 1, fmt, vc);
    va_end(vc);
    ws->len += len;

    return ws;
}

wba_str_t *wba_strcat_printf(wba_str_t *ws, const s8 *fmt, ...)
{
    va_list va;
    wba_str_t *t;

    va_start(va, fmt);
    t = wba_strcat_vprintf(ws, fmt, va);
    va_end(va);
    return t;
}

void wba_strfree_tokens(wba_str_t **wss, s32 cnt)
{
    s32 i;
    if (wss) {
        for (i = 0; i < cnt; i++) {
            if (wss[i]) wba_strfree(wss[i]);
        }
        free(wss);
    }
}

/* 
 * str and sep are binary safe strings 
 * non-NULL return value should be freed with wba_strfree_tokens even when cnt is 0 */
wba_str_t **wba_strntok(const s8 *str, size_t len, const s8 *sep, size_t seplen, s32 *cnt)
{
    const s8 *p, *q;
    size_t m, n;
    const s8 *start, *end;
    wba_str_t *ws;
    wba_str_t **wss;
    wba_str_t **tmp;
    s32 slots = 5;
    s8 hit;

    if (!cnt) {
        return NULL;
    }
    *cnt = 0;
    if (!str || !sep || seplen == 0) {
        return NULL;
    }

    wss = (wba_str_t **)malloc(sizeof(wba_str_t *) * slots);
    if (!wss) {
        return NULL;
    }
    start = end = NULL;
    for (m = 0, p = str; m < len; m++, p++) {
        hit = 0;
        for (n = 0, q = sep; n < seplen; n++, q++) {
            if (*p == *q) {
                hit = 1;
                break;
            }
        }
        if (!hit) {
            start = !start ? p : start; 
        } else {
            end = start ? p : end; 
        }

        if (start && !end && m == len - 1) {
            end = p + 1; /* last token followed by no sep chars */
        }
        if (start && end) {
            if (*cnt >= slots - 1) {
                slots *= 2;
                tmp = (wba_str_t **)realloc(wss, sizeof(wba_str_t *) * slots);
                if (!tmp) {
                    wba_strfree_tokens(wss, *cnt);
                    return NULL;
                }
                wss = tmp;
            }
            if (!(ws = wba_strnew(end - start))) {
                wba_strfree_tokens(wss, *cnt);
                return NULL;
            }
            wba_strncpy(ws, start, end - start);
            wss[(*cnt)++] = ws;
            start = end = NULL;
        }
    }
    return wss;
}

/* str and sep are null-terminated strings */
wba_str_t **wba_strtok(const s8 *str, const s8 *sep, s32 *cnt)
{
    if (!cnt) {
        return NULL;
    }
    *cnt = 0;
    if (!str || !sep) {
        return NULL;
    }

    return wba_strntok(str, strlen(str), sep, strlen(sep), cnt);
}

