#include "include/list.h"

list list_create(u16 size, allocator* al)
{
	list l;
	l.size = size;
	l.count = 0;
	l.start = 0;
	l.al = al;
	
	return l;
}

void list_destroy(list* list) {
	list->start = 0;
	list->count = 0;
}

list_item *list_push(list *list, void *data)
{
	list_item *item = allocator_alloc(list->al, sizeof(list_item));
	item->next = 0;
	item->data = data;
	
	if (list->count == 0)
	{
		list->start = item;
	}
	else
	{
		list_item *current_item = list->start;
		while(current_item)
		{
			if (!current_item->next)
			{
				current_item->next = item;
				goto done;
			}
			else
			{
				current_item = current_item->next;
			}
		}
	}
	
	done:
	list->count++;
	return item;
}

void* list_at(list *list, u32 index)
{
	list_item *current_item = list->start;
	s32 count = 0;
	while(current_item)
	{
		if (count == index)
			return current_item->data;
		count++;
		current_item = current_item->next;
	}
	return 0;
}

void list_remove_at(list *list, u32 index)
{
	list_item *prev = 0;
	list_item *current_item = list->start;
	s32 count = 0;
	while(current_item)
	{
		if (count == index)
		{
			if (prev)
				prev->next = current_item->next;
			else
				list->start = current_item->next;
			
			goto done;
		}
		
		count++;
		prev = current_item;
		current_item = current_item->next;
	}
	
	done:
	list->count--;
}

void list_debug_print(list *list)
{
	list_item *current_item = list->start;
	s32 count = 0;
	while(current_item)
	{
		printf("%d %p\n", count, current_item->data);
		
		count++;
		current_item = current_item->next;
	}
}