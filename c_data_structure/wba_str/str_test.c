#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "wba_str.h"

s8 *chr2str(s8 c)
{
    static s8 buf[3] = {0};
    if (isprint(c)) {
        snprintf(buf, 3, "%c", c);
    } else if (c == '\0') {
        snprintf(buf, 3, "%s", "\\0");
    } else {
        snprintf(buf, 3, "%s", "\\o");
    }
    return buf;
}
void print_buf(s8 *buf, size_t len)
{
    int i = 0;
    int limit = 256;

    if (!buf) {
        printf("buf is NULL\n");
        return;
    }
    printf("[");
    while (i < len && i < limit) {
        printf("%s", chr2str(buf[i]));
        i++;
    }
    printf("]");
    if (len <= limit) {
        printf("[%s]\n", chr2str(buf[i]));
    } else {
        printf("[...]\n");
    }
}
void print_str(wba_str_t *s) {
    if (s) {
        printf("len: %zu, cap: %zu\n", s->len, s->cap);
        print_buf(s->buf, s->len);
    } else {
        printf("NULL\n");
    }
    printf("--------------------\n");
}

void print_tokens(wba_str_t **wss, int cnt) {
    int i;
    if (wss) {
        printf("<count>: %d\n", cnt);
        for (i = 0; i < cnt; i++) {
            printf("str[%d]:\n", i);
            print_str(wss[i]);
        }
    } else {
        printf("NULL\n");
    }
    printf("====================\n");
}

void newfree_test01()
{
    wba_str_t *ws = wba_strnew(0);
    print_str(ws);

    ws = wba_strnew(16);
    print_str(ws);

    ws = wba_strnew(1024);
    print_str(ws);

    if (wba_strnew(SIZE_MAX - 1) != NULL) {
        printf("test fail!\n");
        exit(1);
    }
}

void newfree_test02()
{
    wba_str_t *ws;

    while (1) {
        ws = wba_strnew(1024*1024);
        sleep(1);
        wba_strfree(ws);
    }
}

void newfree_test03()
{
    wba_str_t *ws = wba_strnewlen("", 0);
    print_str(ws);
    ws = wba_strnewlen("empty", 0);
    print_str(ws);
    ws = wba_strnewlen(NULL, 0);
    print_str(ws);

    ws = wba_strnewlen(NULL, 5);
    print_str(ws);
    ws = wba_strnewlen("", 5);
    print_str(ws);
    ws = wba_strnewlen("abcde", 5);
    print_str(ws);
    ws = wba_strnewlen("You need speed", 5);
    print_str(ws);

    ws = wba_strnewlen("aaa", 1024);
    print_str(ws);
    if (wba_strnewlen("", SIZE_MAX - 1) != NULL) {
        printf("test fail!\n");
        exit(1);
    }
}

void newfree_test04()
{
    wba_str_t *ws = wba_strnew2(NULL);
    print_str(ws);
    ws = wba_strnew2("");
    print_str(ws);
    ws = wba_strnew2("aaaa\0aaaa");
    print_str(ws);
}

void grow_test01()
{
    wba_str_t *ws = wba_strnew(0);
    print_str(ws);
    wba_strgrow(ws, 1024);
    print_str(ws);
    if (wba_strgrow(ws, SIZE_MAX) != NULL) {
        printf("test fail\n");
        exit(1);
    }
    print_str(ws);
}

void grow_test02()
{
    wba_str_t *ws = wba_strnew(1024);
    print_str(ws);
    if (wba_strgrow(ws, 512) != ws || ws->cap != 1024) {
        printf("test fail!\n");
        exit(1);
    }
    if (wba_strgrow(0, 0) != NULL) {
        printf("test fail!\n");
        exit(1);
    }
}

void cpycat_test01()
{
    wba_str_t *ws = wba_strnew(0);
    wba_strcpy(ws, "aaaaa");
    wba_strcat(ws, "aaaaa");
    wba_strcpy(ws, "b");
    wba_strcat(ws, "aaaaaaaaaa");
    print_str(ws);
    wba_strclr(ws);
    print_str(ws);
}

void cpycat_test02()
{
    int i;
    wba_str_t *ws = wba_strnew(0);
    for (i = 0; i < 32; i++) {
        wba_strncat(ws, "\0", 1);
        print_str(ws);
        sleep(2);
    }
}

void cpycat_test03()
{
    s8 *buf = (s8 *)malloc(1024*1024*1024); 
    memset(buf, '$', 1024*1024*1024);
    wba_str_t *ws = wba_strnew(0);
    while (1) {
        if (wba_strncat(ws, buf, 1024*1024*1024) == NULL) {
            printf("not enough memory!\n");
            exit(1);
        }
        print_str(ws);
    }
}

void cpycat_test04()
{
    size_t size = 1024*1024*1024;
    s8 *buf = (s8 *)malloc(size);
    wba_str_t *ws;
    while (1) {
        ws = wba_strnew(0);
        if (wba_strncpy(ws, buf, size) == NULL) {
            printf("not enouth memory!\n");
            exit(1);
        }
        print_str(ws);
    }
}

typedef wba_str_t* (*fn)(wba_str_t*, const s8*, size_t);
void cpycat_test05(fn f)
{
    s8 buf[8];

    wba_str_t *ws = wba_strnew(0);
    if (f(0, buf, 8) != NULL) {
        printf("test fail!\n");
        exit(1);
    }
    if (f(ws, 0, 8) != ws) {
        printf("test fail!\n");
        exit(1);
    }
    if (f(ws, buf, 0) != ws) {
        printf("test fail!\n");
        exit(1);
    }
    printf("PASS!\n");
}

void cpycat_test06()
{
    s8 buf[] = "SDS is a string library for C designed to augment the limited libc string "
        "handling functionalities by adding heap allocated strings";
    printf("sizeof(buf): %d\n", sizeof(buf));

    wba_str_t *ws = wba_strnew(16);
    wba_strcpy(ws, buf);
    print_str(ws);

    wba_strcat(ws, "|");
    wba_strcat(ws, buf);
    print_str(ws);

    wba_strclr(ws);
    buf[10] = '\0';
    buf[20] = '\0';
    wba_strcpy(ws, buf);
    wba_strcat(ws, buf);
    print_str(ws);

    wba_strclr(ws);
    wba_strncpy(ws, buf, 29);
    wba_strncat(ws, buf+74, 57);
    print_str(ws);

    wba_strclr(ws);
    wba_strncpy(ws, buf, 1024);
    print_str(ws);

    wba_strclr(ws);
    wba_strncat(ws, buf, 1024);
    print_str(ws);
}

void cpycat_test07()
{
    wba_str_t *ws = wba_strnew(0);
    if (wba_strcat_printf(0, "%d", 1000) != NULL) {
        printf("test fail!\n");
        exit(1);
    }
    if (wba_strcat_printf(ws, 0, 1000) != NULL) {
        printf("test fail!\n");
        exit(1);
    }
    
    if (wba_strcat_printf(ws, "") != NULL) {
        print_str(ws);
    }

    if (wba_strcat_printf(ws, "", 100) != NULL) {
        print_str(ws);
    }
}

void cpycat_test08()
{
    wba_str_t *ws = wba_strnew(0);
    wba_strcat_printf(ws, "we %s <%p>, %d", "ashen", ws, 10086);
    print_str(ws);

    const char* s = "Hello";
    wba_strcat_printf(ws, "\t.%10s.\n\t.%-10s.\n\t.%*s.\n", s, s, 10, s);
    print_str(ws);

    wba_strcat_printf(ws, "<<<Rounding:\t%f %.0f %.32f\n", 1.5, 1.5, 1.3);
    print_str(ws);
}

void tok_test01()
{
    wba_str_t **wss;
    s32 cnt = 0;
    s8 *str = "!anew day has  come\t \t, we^are$$$waiting!!!";

    wss = wba_strtok(str, " ", &cnt);
    print_tokens(wss, cnt);

    wss = wba_strtok(str, " \t", &cnt);
    print_tokens(wss, cnt);

    wss = wba_strtok(str, " \t!", &cnt);
    print_tokens(wss, cnt);
}

void tok_test02()
{
    wba_str_t **wss;
    s32 cnt = 0;
    s8 *str1 = "!@#new#@!";
    s8 *str2 = "new!@#new#@!new";
    s8 *str3 = "!@#new#@!new";
    s8 *str4 = "new!@#new#@!";
    s8 *str5 = "!@#";
    s8 *str6 = "new";
    s8 *str7 = "";

    wss = wba_strtok(str7, "!@#", &cnt);
    print_tokens(wss, cnt);

    if (wba_strtok(0, "a", &cnt) != NULL) {
        printf("test fail!\n");
        exit(1);
    }
    if (wba_strtok(str7, "", &cnt) != NULL) {
        printf("test fail!\n");
        exit(1);
    }
    if (wba_strtok(str7, 0, &cnt) != NULL) {
        printf("test fail!\n");
        exit(1);
    }
    if (wba_strtok(str7, "a", 0) != NULL) {
        printf("test fail!\n");
        exit(1);
    }
}

void tok_test03()
{
    wba_str_t **wss;
    s32 cnt = 0;

    s8 str[] = "\0aaaa,cbd,\0fw\0";

    wss = wba_strntok(str, sizeof(str), "\0", 1, &cnt);
    print_tokens(wss, cnt);

    wss = wba_strntok(str, sizeof(str), "\0,", 2, &cnt);
    print_tokens(wss, cnt);

    wss = wba_strntok(str, 3, "\0aaaa", 1, &cnt);
    print_tokens(wss, cnt);
}

void tok_test04()
{
    wba_str_t **wss;
    s32 i;
    s32 cnt = 0;

    srand(time(0));
    s8 *buf = (s8 *)malloc(1024*1024);
    memset(buf, 'a', 1024*1024);
    s32 base = 1024;
    for (i = 0; i < 1024; i++) {
        s32 index = i * (rand() % base);
        buf[index] = '\0';
    }

    while (1) {
        wss = wba_strntok(buf, 1024*1024, "\0", 1, &cnt);
        if (wss) {
            printf("<count>: %d\n", cnt);
        } else {
            printf("NULL\n");
        }
        printf("------------------\n");
        wba_strfree_tokens(wss, cnt);
    }
}

int main() {
    //newfree_test01();
    //newfree_test02();
    //newfree_test03();
    newfree_test04();
    //grow_test01();
    //grow_test02();
    //cpycat_test01();
    //cpycat_test02();
    //cpycat_test03();
    //cpycat_test04();
    //cpycat_test05(wba_strncat);
    //cpycat_test05(wba_strncpy);
    //cpycat_test07();
    //cpycat_test08();
    //tok_test01();
    //tok_test02();
    //tok_test03();
    //tok_test04();
}
