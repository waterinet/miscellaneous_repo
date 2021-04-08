#ifndef _SKIPLIST_H_
#define _SKIPLIST_H_

typedef struct sknode_s {
    struct sknode_s **link;
    int level;
    void *obj;
} sknode_t;

typedef struct sklist_s {
    struct sknode_s *head;
    int max_level;
    int level;
    int size;
    int (*cmp_key)(void *k1, void *k2);
    void *(*get_key)(void *obj);
    void (*destroy)(void *obj);
} sklist_t;

void *sklist_search(sklist_t *sk_list, void *key);
void *sklist_search_ext(sklist_t *sk_list, void *key, sknode_t **pos);
int sklist_insert(sklist_t *sk_list, void *obj);
int sklist_delete(sklist_t *sk_list, void *key);

sklist_t *sklist_init(int max_level);
void sklist_free(sklist_t *sk_list);

#endif /* _SKIPLIST_H_ */
