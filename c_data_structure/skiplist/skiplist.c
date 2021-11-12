#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "skiplist.h"

static sknode_t *new_node(void *obj, int n)
{
    sknode_t *node;

    node = (sknode_t *)malloc(sizeof(sknode_t));
    if (!node) {
        return NULL;
    }
    memset(node, 0, sizeof(sknode_t));
    node->link = (sknode_t **)malloc(sizeof(sknode_t *) * n);
    if (!node->link) {
        free(node);
        return NULL;
    }
    node->obj = obj;
    node->level = n;
    memset(node->link, 0, sizeof(sknode_t *) * n);
    return node;
}

static void destroy_node(sklist_t *sk_list, sknode_t *node)
{
    if (node) {
        if (sk_list->destroy && node->obj) {
            sk_list->destroy(node->obj);
        }
        free(node->link);
        free(node);
    }
}

static void *search_node(sklist_t *sk_list, void *key, sknode_t **pos)
{
    sknode_t *node;
    int i;
    void *node_key;

    if (pos) {
        *pos = NULL;
    }
    if (!sk_list || !sk_list->head || !key) {
        return NULL;
    }
    if (!sk_list->get_key || !sk_list->cmp_key) {
        return NULL;
    }

    node = sk_list->head;
    i = sk_list->level;
    while (i--) {
        /* return first matching obj */
        while (node->link[i] != sk_list->head) {
            node_key = sk_list->get_key(node->link[i]->obj);
            if (sk_list->cmp_key(key, node_key) <= 0) {
                break;
            }
            node = node->link[i];
        }
        if (node->link[i] == sk_list->head) {
            continue;
        }
        node_key = sk_list->get_key(node->link[i]->obj);
        if (sk_list->cmp_key(key, node_key) == 0) {
            if (pos) {
                *pos = node->link[i];
            }
            return node->link[i]->obj;
        }
    }
    if (pos) {
        *pos = node;
    }

    return NULL;
}

void *sklist_search(sklist_t *sk_list, void *key)
{
    return search_node(sk_list, key, NULL);
}

void *sklist_search_ext(sklist_t *sk_list, void *key, sknode_t **pos)
{
    return search_node(sk_list, key, pos);
}

static int rand_level(sklist_t* sk_list)
{
    int i, j, t;

    t = rand();
    /* max(i) == max_level */
    for (i = 1, j = 2; i < sk_list->max_level; i++, j += j) {
        if (t > RAND_MAX / j) {
            break;
        }
    }
    if (i > sk_list->level) {
        sk_list->level = i;
    }

    return i;
}

int sklist_insert(sklist_t *sk_list, void *obj)
{
    sknode_t *node, *new;
    int i;
    void *key, *node_key;
    
    if (!sk_list || !sk_list->head || !obj) {
        return -1;
    }
    if (!sk_list->get_key || !sk_list->cmp_key) {
        return -1;
    }

    node = sk_list->head;
    new = new_node(obj, rand_level(sk_list));
    if (!new) {
        return -1;
    }
    i = sk_list->level;
    /* link new node from top-level to bottom-level */
    while (i--) {
        while (node->link[i] != sk_list->head) {
            key = sk_list->get_key(obj);
            node_key = sk_list->get_key(node->link[i]->obj);
            if (sk_list->cmp_key(key, node_key) < 0) {
                break;
            }
            node = node->link[i];
        }
        /* 1 <= new->level <= sk_list->level */
        if (i < new->level) {
            new->link[i] = node->link[i];
            node->link[i] = new;
            if (i == 0) {
                new->prev = node;
                new->link[i]->prev = new;
            }
        }
    }
    sk_list->size++;

    return 0;
}

int sklist_delete(sklist_t *sk_list, void *key)
{
    sknode_t *node, *next;
    int i;
    void *node_key;

    if (!sk_list || !sk_list->head || !key) {
        return -1;
    }
    if (!sk_list->get_key || !sk_list->cmp_key) {
        return -1;
    }

    i = sk_list->level;
    node = sk_list->head;
    while (i--) {
        while (node->link[i] != sk_list->head) {
            node_key = sk_list->get_key(node->link[i]->obj);
            if (sk_list->cmp_key(key, node_key) <= 0) {
                break;
            }
            node = node->link[i];
        }
        if (node->link[i] == sk_list->head) {
            continue;
        }
        node_key = sk_list->get_key(node->link[i]->obj);
        if (sk_list->cmp_key(key, node_key) == 0) {
            next = node->link[i];
            node->link[i] = next->link[i];
            /* this implies node == head and list[i] is empty */
            if (node == node->link[i]) {
                sk_list->level--;
            }
            if (i == 0) {
                next->link[i]->prev = node;
                destroy_node(sk_list, next);
                sk_list->size--;
            }
        }
    }

    return 0;
}

sklist_t *sklist_init(int max_level)
{
    sklist_t *sk_list;
    int i;
    
    if (max_level <= 0) {
        return NULL;
    }
    sk_list = (sklist_t *)malloc(sizeof(sklist_t));
    if (!sk_list) {
        return NULL;
    }
    memset(sk_list, 0, sizeof(sklist_t));
    sk_list->head = new_node(NULL, max_level);
    if (!sk_list->head) {
        free(sk_list);
        return NULL;
    }
    sk_list->max_level = max_level;
    for (i = 0; i < max_level; i++) {
        sk_list->head->link[i] = sk_list->head;
    }
    sk_list->head->prev = sk_list->head;

    return sk_list;
}

void sklist_free(sklist_t *sk_list)
{
    sknode_t *node, *next;
    void *key;
    
    if (sk_list) {
        node = sk_list->head->link[0];
        while (node != sk_list->head) {
            next = node->link[0];
            key = sk_list->get_key(node->obj);
            sklist_delete(sk_list, key);
            node = next;
        }
        destroy_node(sk_list, sk_list->head);
        free(sk_list);
    }
}

