#ifndef _SKIPLIST_H_
#define _SKIPLIST_H_

typedef struct sknode_s {
    struct sknode_s **link;
    int level;
    int key;
} sknode_t;

typedef struct sklist_s {
    struct sknode_s *head;
    int max_level;
    int level;
    int size;
} sklist_t;


sknode_t *sklist_search(sklist_t *head, int key);
void sklist_insert(sklist_t *head, int key);
void sklist_delete(sklist_t *head, int key);
sklist_t *sklist_init(int max_level);

#endif /* _SKIPLIST_H_ */
