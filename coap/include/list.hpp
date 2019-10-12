#ifndef LIST_HPP
#define LIST_HPP

typedef struct list_s {
	struct list_s * next;
	uint16_t id;
}list_t;

list_t * list_new(list_t * nn);
void list_free(list_t * p);
void list_delete(list_t * head);
list_t * list_remove_node (list_t * previous);
void list_delete_node (list_t * previous);
void list_insert_node (list_t * previous, list_t * new_node);
bool list_create_node(list_t * previous);
list_t * get_last_node(list_t * head);



#endif