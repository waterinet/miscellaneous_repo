#ifndef _WBA_STR_H_
#define _WBA_STR_H_
#include <string.h>
#include <stdarg.h>

typedef char s8;
typedef int  s32;

typedef struct wba_str_s {
    size_t len;
    size_t cap; /* capacity, cap == len + free + 1 */
    s8 *buf;
} wba_str_t;

wba_str_t *wba_strnew(size_t initlen);
void wba_strfree(wba_str_t*);
void wba_strclr(wba_str_t*);
wba_str_t *wba_strgrow(wba_str_t*, size_t len);
wba_str_t *wba_strncpy(wba_str_t*, const s8*, size_t);
wba_str_t *wba_strcpy(wba_str_t*, const s8*);
wba_str_t *wba_strncat(wba_str_t*, const void*, size_t);
wba_str_t *wba_strcat(wba_str_t*, const s8*);
wba_str_t *wba_strcat_vprintf(wba_str_t*, const s8*, va_list);
wba_str_t *wba_strcat_printf(wba_str_t*, const s8*, ...);
void wba_strfree_tokens(wba_str_t**, s32);
wba_str_t **wba_strntok(const s8*, size_t, const s8*, size_t, s32*);
wba_str_t **wba_strtok(const s8*, const s8*, s32*);

#endif /* _WBA_STR_H_ */
