#ifndef _SKIPLIST_H_
#define _SKIPLIST_H_

typedef struct sknode_s {
    struct sknode_s **link;
    struct sknode_s *prev;
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

#define sklist_foreach(node, data, list)     \
    for (node = (list)->head->link[0], data = (typeof(data))node->obj;  \
	     node != (list)->head;  \
         node = node->link[0], data = (typeof(data))node->obj)
		 
#define sklist_foreach_safe(node, n, data, list)      \
    for (node = (list)->head->link[0], n = node->link[0],  \
         data = (typeof(data))node->obj; \
         node != (list)->head;  \
		 node = n, n = node->link[0], data = (typeof(data))node->obj)

#define sklist_foreach_reverse(node, data, list)    \
    for (node = (list)->head->prev, data = (typeof(data))node->obj;  \
         node != (list)->head;  \
         node = node->prev, data = (typeof(data))node->obj)

#define sklist_foreach_safe_reverse(node, n, data, list)    \
    for (node = (list)->head->prev, n = node->prev,  \
         data = (typeof(data))node->obj;  \
         node != (list)->head;  \
         node = n, n = node->prev, data = (typeof(data))node->obj)

#endif /* _SKIPLIST_H_ */
