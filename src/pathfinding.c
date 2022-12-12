#include "../include/pathfinding.h"

static float distance_between(vec2f v1, vec2f v2)
{
	return ((v1.x-v2.x)*(v1.x-v2.x)+(v1.y-v2.y)*(v1.y-v2.y));
}

bool can_walk_at(float x, float y)
{
	for (int i = 0; i < max_objects; i++) {
		object o = objects[i];
		if (!o.active) continue;
		if (x >= o.position.x && x < o.position.x + o.size.x && y >= o.position.y && y < o.position.y + o.size.y) return false;
	}
	return true;
}

bool find_path_to(vec2f start_pos, vec2f end_pos, array *to_fill, pathfinding_request* request)
{
	uint64_t timestamp = platform_get_time(TIME_PROCESS, TIME_MS);
	allocator al = create_allocator(500000);

	struct path_node {
		struct path_node *parent;
		vec2f position;
		s32 g;
		s32 h;
		s32 f;
	};
	
	list open_array = list_create(sizeof(struct path_node), &al);
	list closed_array = list_create(sizeof(struct path_node), &al);
	
	struct path_node *start_node = allocator_alloc(&al, sizeof(struct path_node));
	start_node->g = 0;
	start_node->h = 0;
	start_node->f = 0;
	start_node->parent = 0;
	start_node->position = start_pos;
	
	list_push(&open_array, (uint8_t*)start_node);
	
	struct path_node *current_node = 0;
	while(open_array.count > 0)
	{
		// Get the current node
		current_node = list_at(&open_array, 0);
		s32 current_index = 0;
		
		for (s32 i = 0; i < open_array.count; i++)
		{
			struct path_node *item = list_at(&open_array, i);
			if (item->f < current_node->f)
			{
				current_node = item;
				current_index = i;
			}
		}
		
		//Pop current off open array, add to closed array
		list_push(&closed_array, (uint8_t*)current_node);
		
		if (closed_array.count > 1000) return false;
		if (open_array.count > 1000) return false;
		
		// Found the goal
		if (distance_between(current_node->position, end_pos) <= 0)
		{
			mutex_lock(&request->mutex);

			if (to_fill)
				to_fill->length = 0;
			struct path_node *current = current_node;
			
			while(current != 0)
			{
				if (to_fill)
				{
					vec2f v;
					v.x = current->position.x + 0.5;
					v.y = current->position.y + 0.5;
					array_push(to_fill, (uint8_t*)&v);
				}
				
				struct path_node *prev = current;
				current = current->parent;
			}
			
			if (to_fill) {
				array_remove_at(to_fill, to_fill->length-1);
				//printf("PATHFINDING TOOK: %fms\n", (platform_get_time(TIME_PROCESS, TIME_MS)-timestamp)/1000.0f);
			}
			mutex_unlock(&request->mutex);

			list_destroy(&open_array);
			list_destroy(&closed_array);

			destroy_allocator(&al);

			return true;
		}
		
		vec2 adjecent[8] = { {0, -1}, {0, 1}, {-1, 0}, {1, 0}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1} };
		
		list children = list_create(sizeof(struct path_node), &al);
		
		// Generate children
		for (s32 i = 0; i < 8; i++)
		{
			//Get node position
			vec2f node_position;
			node_position.x = current_node->position.x + adjecent[i].x;
			node_position.y = current_node->position.y + adjecent[i].y;
			
			//Make sure within range
			if (!is_in_bounds(node_position.x, node_position.y) || !can_walk_at(node_position.x, node_position.y)) continue;
			
			// if diagonal move, check if not cutting off building
			if (i >= 4) {
				if (i == 4) // top left
				{
					if (!can_walk_at(node_position.x+1, node_position.y)) continue;
					if (!can_walk_at(node_position.x, node_position.y+1)) continue;
				}
				if (i == 5) // down left
				{
					if (!can_walk_at(node_position.x+1, node_position.y)) continue;
					if (!can_walk_at(node_position.x, node_position.y-1)) continue;
				}
				if (i == 6) // top right
				{
					if (!can_walk_at(node_position.x-1, node_position.y)) continue;
					if (!can_walk_at(node_position.x, node_position.y+1)) continue;
				}
				if (i == 7) // down right
				{
					if (!can_walk_at(node_position.x-1, node_position.y)) continue;
					if (!can_walk_at(node_position.x, node_position.y-1)) continue;
				}
			}
			
			// Create new node
			struct path_node *new_node = allocator_alloc(&al, sizeof(struct path_node));
			new_node->g = 0;
			new_node->h = 0;
			new_node->f = 0;
			new_node->parent = current_node;
			new_node->position = node_position;
			
			if (node_position.x == current_node->position.x &&
				node_position.y == current_node->position.y)
				continue;
			
			// Child is on the closed array
			for (s32 c = 0; c < closed_array.count; c++)
			{
				struct path_node *closed_child = list_at(&closed_array, c);
				
				if (closed_child->position.x == new_node->position.x &&
					closed_child->position.y == new_node->position.y)
				{
					goto skip_adjecent;
				}
			}
			
			list_push(&children,(uint8_t*)new_node);
			
			skip_adjecent:;
		}
		
		// Loop through children
		for (s32 i = 0; i < children.count; i++)
		{
			struct path_node *child = list_at(&children, i);
			
			// Child is on the closed array
			for (s32 c = 0; c < closed_array.count; c++)
			{
				struct path_node *closed_child = list_at(&closed_array, c);
				
				if (closed_child->position.x == child->position.x &&
					closed_child->position.y == child->position.y)
				{
					goto skip_child;
				}
			}
			
			// Create the f, g, and h values
			child->g = current_node->g + 1;
			child->h = distance_between(child->position, end_pos);
			child->f = child->g + child->h;
			
			// Child is already in the open array
			for (s32 c = 0; c < open_array.count; c++)
			{
				struct path_node *open_child = list_at(&open_array, c);
				
				if ((open_child->position.x == child->position.x &&
					 open_child->position.y == child->position.y) && 
					child->g >= open_child->g)
				{
					goto skip_child;
				}
			}
			
			// Add the child to the open array
			list_push(&open_array, (uint8_t*)child);
			
			skip_child:;
		}
		
		list_remove_at(&open_array, current_index);
	}

	destroy_allocator(&al);
	return false;
}

vec2f get_open_tile_next_to_target(float x, float y)
{
	vec2f adjecent[8] = { {0, -1}, {0, 1}, {-1, 0}, {1, 0}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1} };
	
	vec2f v_s;
	v_s.x = x;
	v_s.y = y;
	
	s32 closest_index = -1;
	s32 closest_dist = 999999;
	vec2f v;
	for (s32 i = 0; i < 8; i++)
	{
		if (can_walk_at(x + adjecent[i].x, y + adjecent[i].y))
		{
			vec2f vv;
			vv.x = x + adjecent[i].x;
			vv.y = y + adjecent[i].y;
			
			s32 dist = distance_between(v_s, vv);
			if (dist < closest_dist)
			{
				v = vv;
				closest_dist = dist;
				closest_index = i;
			}
		}
	}
	
	if (closest_index != -1)
	{
		return v;
	}
	
	v.x = -1;
	v.y = -1;
	return v;
}

void pathfinding_init()
{
	global_pathfinding_queue = array_create(sizeof(pathfinding_request));
	array_reserve(&global_pathfinding_queue, 1000);
}

void pathfinding_destroy()
{
	array_destroy(&global_pathfinding_queue);
}

void* pathfinding_thread(void *args)
{
	while(1)
	{
		mutex_lock(&global_pathfinding_queue.mutex);
		if (global_pathfinding_queue.length)
		{
			pathfinding_request* request = *(pathfinding_request**)array_at(&global_pathfinding_queue, 0);
			array_remove_at(&global_pathfinding_queue, 0);
			mutex_unlock(&global_pathfinding_queue.mutex);
			find_path_to(request->start, request->end, request->to_fill, request);
		}
		else
		{
			mutex_unlock(&global_pathfinding_queue.mutex);
		}
	}
	
	return 0;
}

void make_pathfinding_request(vec2f start, vec2f end, array *to_fill, pathfinding_request *request)
{
	start.x = (int)start.x;
	start.y = (int)start.y;
	end.x = (int)end.x;
	end.y = (int)end.y;

	mutex_lock(&global_pathfinding_queue.mutex);
	for (int i = 0; i < global_pathfinding_queue.length; i++) {
		pathfinding_request* ereq = *(pathfinding_request**)array_at(&global_pathfinding_queue, i);
		if (request == ereq) {
			ereq->start = start;
			ereq->end = end;
			ereq->timestamp = platform_get_time(TIME_PROCESS, TIME_MS);
			mutex_unlock(&global_pathfinding_queue.mutex);
			return;
		}
	}

	request->start = start;
	request->end = end;
	request->to_fill = to_fill;

	request->timestamp = 0;
	array_push(&global_pathfinding_queue, (uint8_t*)&request);
	mutex_unlock(&global_pathfinding_queue.mutex);
}