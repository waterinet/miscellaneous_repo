#include <stdio.h>
#include <string.h>
#include <malloc.h>

typedef struct snode {
    char* data;
    struct snode* next;
} snode;

typedef struct slist {
    size_t size;
    snode* head;
    snode* tail;
} slist;

slist token_list = { 0, 0, 0 };

snode* new_node(const char* data) {
    snode* node = (snode*)malloc(sizeof(snode));
    node->data = (char*)malloc(strlen(data) + 1);
    strcpy(node->data, data);
    node->next = 0;
    return node;
}

void add_node(slist* list, snode* node) {
    if (list->size > 0) {
		list->tail->next = node;
		list->tail = list->tail->next;
        ++list->size;
    }
    else {
        list->tail = node;
        list->head = list->tail;
        ++list->size;
    }
}

int main() {
   add_node(&token_list, new_node("aa")); 
   add_node(&token_list, new_node("bb"));
   add_node(&token_list, new_node("cc"));
   add_node(&token_list, new_node(""));
   add_node(&token_list, new_node("dd"));

   snode* p = token_list.head;
   while (p != 0) {
	   printf("%s\n", p->data);
	   p = p->next;
   };
}
