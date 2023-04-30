#ifndef INCLUDE_PATHFINDING
#define INCLUDE_PATHFINDING

#include <projectbase/project_base.h>

#include "allocator.h"
#include "players.h"
#include "objects.h"
#include "list.h"

typedef struct t_pathfinding_request
{
	vec2f start;
	vec2f end;
	array *to_fill;
	uint64_t timestamp;
	mutex mutex;
	bool active;
} pathfinding_request;

array global_pathfinding_queue;

void* pathfinding_thread(void *args);
void make_pathfinding_request(vec2f start, vec2f end, array *to_fill, pathfinding_request *request);

#endif