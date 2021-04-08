#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include "skiplist.h"

int COUNT = 0;

typedef struct {
    int key;
    int value;
} item_t;

static int cmp_key(void *k1, void *k2)
{
    return (*(int *)k1 - *(int *)k2);
}

static void *get_key(void *obj)
{
    item_t *tmp = (item_t *)obj;
    return &tmp->key;
}

static void destroy(void *obj)
{
    if (obj) {
        free(obj);
    }
}

static void print_list(sklist_t *sk_list)
{
    int i;
    sknode_t *node = NULL;
    item_t *item;
    int count, limit;

    printf("max_level: %d\n", sk_list->max_level);
    printf("level: %d\n", sk_list->level);
    printf("size: %d\n", sk_list->size);
    for (i = 0; i < sk_list->level; i++) {
        count = 0;
        limit = 100 / (i + 1);
        printf("level %d:", i);
        for (node = sk_list->head; node; node = node->link[i]) {
            if (node->obj) {
                item = (item_t *)node->obj;
                printf(" %d:%d", item->key, item->value);
                if (++count >= limit) {
                    break;
                }
            }
        }
        printf("\n");
    }
}

static sklist_t *new_list(max_level)
{
    sklist_t *sk_list = sklist_init(max_level);
    sk_list->cmp_key = cmp_key;
    sk_list->get_key = get_key;
    sk_list->destroy = destroy;
}

static unsigned int get_time()
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

static void sk_test01()
{
    int i, j, ret;
    item_t *item;
    sklist_t *sk_list;

    sk_list = new_list(5);

    for (i = 0; i < 1000; i++) {
        item = malloc(sizeof(item_t));
        item->key = i;
        item->value = 0;
        ret = sklist_insert(sk_list, item);
        assert(ret == 0);
    }

    for (i = 0; i < 1000; i++) {
        j = rand() % 1000;
        item = sklist_search(sk_list, &j);
        assert(item);
        assert(item->key == j);
    }

    for (i = 0; i < 100; i++) {
        ret = sklist_delete(sk_list, &i);
        assert(ret == 0);
    }
    for (i = 200; i < 1000; i++) {
        ret = sklist_delete(sk_list, &i);
        assert(ret == 0);
    }
    assert(sk_list->size == 100);

    printf("--------------------test01--------------------\n");
    print_list(sk_list);
}

static void sk_test02()
{
    int i, j, ret;
    item_t *item;
    sklist_t *sk_list;

    sk_list = new_list(5);

    i = 5;
    item = sklist_search(sk_list, &i);
    assert(item == NULL);

    ret = sklist_delete(sk_list, &i);
    assert(ret == 0);
    assert(sk_list->size == 0);

    for (i = 0; i < 10; i++) {
        for (j = 0; j < 3; j++) {
            item = malloc(sizeof(item_t));
            item->key = i;
            item->value = j;
            ret = sklist_insert(sk_list, item); /* key值相同，靠后添加 */
            assert(ret == 0);
        }
    }
    assert(sk_list->size == 30);
    for (i = 0; i < 10; i++) {
        ret = sklist_delete(sk_list, &i); /* 删除第一个key值匹配的项 */
        assert(ret == 0);
        item = sklist_search(sk_list, &i);
        assert(item);
        assert(item->key == i);
        /* 查找时根据level从高到低，首次匹配key则返回，key值重复的项不一定按照插入顺序返回 */
        assert(item->value != 0);
    }
    assert(sk_list->size == 20);

    printf("--------------------test02--------------------\n");
    print_list(sk_list);
}

void sk_test03(int max_level, int size)
{
    int i, j, ret;
    item_t *item;
    sklist_t *sk_list;
    unsigned int start, itime, stime;

    printf("--------------------test03--------------------\n");
    start = get_time();
    sk_list = new_list(max_level);
    for (i = 0; i < size; i++) {
        item = malloc(sizeof(item_t));
        item->key = i;
        item->value = 0;
        ret = sklist_insert(sk_list, item);
        assert(ret == 0);
    }
    itime =  get_time() - start;
    assert(sk_list->size == size);

    start = get_time();
    printf("start searching...\n");
    for (i = 0; i < 100; i++) {
        j = rand() % size;
        item = sklist_search(sk_list, &j);
        assert(item);
        assert(item->key == j);
        printf("%d ", j);
    }
    printf("\ndone!\n");
    stime = get_time() - start;
            
    printf("itime(ms) %u stime(ms) %u avg(%d) %f\n\n", itime,
            stime, 100, (float)stime/100);
    print_list(sk_list);
}

void sk_test04()
{
    int i, j, ret;
    item_t *item;
    sklist_t *sk_list;
    sknode_t *pos;
    
    sk_list = new_list(5);
    i = 1;
    item = sklist_search_ext(sk_list, &i, &pos);
    assert(item == NULL);
    assert(pos = sk_list->head);

    for (i = 10; i < 30; i++) {
        item = malloc(sizeof(item_t));
        item->key = i;
        item->value = 0;
        ret = sklist_insert(sk_list, item);
        assert(ret == 0);
    }

    for (i = 0; i < 40; i++) {
        item = sklist_search_ext(sk_list, &i, &pos);
        if (i < 10) {
            assert(item == NULL);
            assert(pos);
            assert(pos == sk_list->head);
            //assert(((item_t *)pos->obj)->key == 10);
        } else if (i >= 30) {
            assert(item == NULL);
            assert(pos);
            assert(((item_t *)pos->obj)->key == 29);
            //assert(pos == sk_list->head);
        } else {
            assert(item);
            assert(pos);
            assert(item == pos->obj);
        }
    }

    printf("--------------------test04--------------------\n");
    print_list(sk_list);

}

static void destroy05(void *obj)
{
    item_t *item = (item_t *)obj;
    if (item) {
        printf("free item %d\n", item->key);
        free(item);
    }
}

void sk_test05()
{
    int i, j, ret;
    item_t *item;
    sklist_t *sk_list;
    
    sk_list = new_list(5);
    sk_list->destroy = destroy05;

    for (i = 0; i < 10; i++) {
        item = malloc(sizeof(item_t));
        item->key = i;
        item->value = 0;
        ret = sklist_insert(sk_list, item);
        assert(ret == 0);
    }

    printf("--------------------test05--------------------\n");
    sklist_free(sk_list);
}

int main()
{
    srand(time(NULL));

    sk_test01();
    sk_test02();
    sk_test03(16, 512000);
    sk_test04();
    sk_test05();

    return 0;
}
