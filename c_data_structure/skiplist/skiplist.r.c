#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "skiplist.h"

extern int COUNT;

sknode_t *new_node(int key, int n)
{
    int i;
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
    node->key = key;
    node->level = n;
    memset(node->link, 0, sizeof(sknode_t *) * n);
    return node;
}

void destroy_node(sknode_t *node)
{
    if (node) {
        free(node->link);
        free(node);
    }
}

static sknode_t *search_node(sknode_t *node, int key, int n)
{
    if (!node) {
        return NULL;
    }
    printf("node->key: %d, key: %d\n", node->key, key);
    COUNT++;
    if (node->key == key) {
        return node;
    }
    COUNT++;
    if (!node->link[n] || key < node->link[n]->key) {
        if (n == 0) {
            return NULL;
        }
        return search_node(node, key, n - 1);
    }
    return search_node(node->link[n], key, n);

}

sknode_t *sklist_search(sklist_t *sk_list, int key)
{
    return search_node(sk_list->head, key, sk_list->level - 1); 
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

static void insert_node(sknode_t *node, sknode_t *new, int n)
{
    if (!node->link[n] || new->key < node->link[n]->key) {
        if (n < new->level) {
            new->link[n] = node->link[n];
            node->link[n] = new;
        }
        if (n == 0) {
            return;
        }
        insert_node(node, new, n - 1);
    } else {
        insert_node(node->link[n], new, n);
    }
}

void sklist_insert(sklist_t *sk_list, int key)
{
    sknode_t *node;
    
    node = new_node(key, rand_level(sk_list));
    insert_node(sk_list->head, node, sk_list->level - 1);
    sk_list->size++;
}

static void delete_node(sknode_t *node, int key, int n)
{
    sknode_t *next;

    if (!node->link[n] || key <= node->link[n]->key) {
        if (node->link[n] && key == node->link[n]->key) {
            next = node->link[n];
            node->link[n] = next->link[n];
            if (n == 0) {
                destroy_node(next);
                return;
            }
        }
        if (n == 0) {
            return;
        }
        delete_node(node, key, n - 1);
    } else {
        delete_node(node->link[n], key, n);
    }
}

void sklist_delete(sklist_t *sk_list, int key)
{
    delete_node(sk_list->head, key, sk_list->level - 1);
    sk_list->size--;
}

sklist_t *sklist_init(int max_level)
{
    sklist_t *sk_list;
    
    sk_list = (sklist_t *)malloc(sizeof(sklist_t));
    if (!sk_list) {
        return NULL;
    }
    memset(sk_list, 0, sizeof(sklist_t));
    sk_list->head = new_node(-1, max_level);
    if (!sk_list->head) {
        free(sk_list);
        return NULL;
    }
    sk_list->max_level = max_level;

    return sk_list;
}
