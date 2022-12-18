#include "../include/allocator.h"

allocator create_allocator(uint64_t size) {
	allocator allocator;
	allocator.cursor = 0;
	allocator.size = size;
	allocator.memory = mem_alloc(size);
	allocator.mutex = mutex_create();
	return allocator;
}

void* allocator_alloc(allocator* al, uint64_t size) {
	mutex_lock(&al->mutex);
	if (al->cursor + size < al->size) {
		al->cursor += size;
		void* result = al->memory + al->cursor - size;
		mutex_unlock(&al->mutex);
		return result;
	}
	log_assert(0, "Allocator out of space");
	mutex_unlock(&al->mutex);
}

void destroy_allocator(allocator* al) {
	mutex_lock(&al->mutex);
	mem_free(al->memory);
	mutex_unlock(&al->mutex);
}

void allocator_clear(allocator* al) {
	mutex_lock(&al->mutex);
	al->cursor = 0;
	mutex_unlock(&al->mutex);
}