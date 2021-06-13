#include "list.hpp"
#include <cstdio>
#include <cstdlib>



list_t * list_new(list_t * new_node)
{
	return (list_t *)malloc(sizeof(*new_node));
}

void list_free(list_t * p)
{
	free(p);
	p = NULL;
}

void list_delete(list_t * head)
{
	list_t * last = NULL;
	list_t * p = NULL;

	while (1)
	{
		for (p = head; p != NULL; p = p->next)
		{
			last = p;
		}
		if (last == head)
			return;

		list_free(last);
	}
}

list_t * list_remove_node (list_t * previous)
{
	list_t * removed;
	assert(previous != NULL);
	assert(removed != NULL);
	removed = previous->next;
	previous->next = removed->next;
	return removed;
}

void list_delete_node (list_t * previous)
{
	list_t * removed;
	removed = list_remove_node (previous);
	list_free(removed);
}


void list_insert_node (list_t * previous, list_t * new_node)
{
	assert(previous != NULL);
	assert(new_node != NULL);
	new_node->next = previous->next;
	previous->next = new_node;
}

bool list_create_node(list_t * previous)
{
	list_t * new_node;
	new_node = list_new(*previous);
	if (new_node == NULL) return false;
	new_node->id = previous->id + 1;
	list_insert_node (previous, new_node);
	return true;
}

list_t * get_last_node(list_t * head)
{
	list_t * last = NULL;
	list_t * p = NULL;

	for (p = head; p != NULL; p = p->next)
	{
		last = p;
	}
	return last;
}