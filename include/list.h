
#ifndef INCLUDE_LIST
#define INCLUDE_LIST

#include <projectbase/project_base.h>

#include "allocator.h"

typedef struct t_list_item
{
	void *next;
	void *data;
} list_item;

typedef struct t_list
{
	list_item *start;
	u32 count;
	u16 size;
	mutex mutex;
	allocator* al;
} list;

void list_destroy(list* list);
list list_create(u16 size, allocator* al);
list_item *list_push(list *list, void *data);
void* list_at(list *list, u32 index);
void list_remove_at(list *list, u32 index);
void list_debug_print(list *list);

#endif