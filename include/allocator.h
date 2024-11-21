#ifndef INCLUDE_ALLOCATOR
#define INCLUDE_ALLOCATOR

#include "../project-base/src/project_base.h"

typedef struct t_allocator {
	void* memory;
	uint64_t cursor;
	uint64_t size;
	mutex mutex;
} allocator;

allocator create_allocator(uint64_t size);
void* allocator_alloc(allocator* al, uint64_t size);
void destroy_allocator(allocator* al);
void allocator_clear(allocator* al);

#endif