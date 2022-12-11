#include "include/allocator.h"

allocator create_allocator(uint64_t size) {
	allocator allocator;
	allocator.cursor = 0;
	allocator.size = size;
	allocator.memory = mem_alloc(size);
	return allocator;
}

void* allocator_alloc(allocator* al, uint64_t size) {
	if (al->cursor + size < al->size) {
		al->cursor += size;
		return al->memory + al->cursor - size;
	}
	log_assert(0, "Allocator out of space");
}

void destroy_allocator(allocator* al) {
	mem_free(al->memory);
}