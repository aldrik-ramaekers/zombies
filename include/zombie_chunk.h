#ifndef ZOMBIE_CHUNK
#define ZOMBIE_CHUNK

#include <projectbase/project_base.h>

#include "objects.h"

typedef enum t_zombie_chunk_type
{
	CHUNK_FOOT,
	CHUNK_HAND,
	CHUNK_SPLATTER,
} zombie_chunk_type;

typedef struct t_zombie_chunk {
	bool active;
	vec3f start_position;
	vec3f position;
	vec3f target_position;
	float duration;
	vec2f direction;
	zombie_chunk_type type;
	int random_chunk;
	float rotation;
	float rotation_speed;
} zombie_chunk;

#define MAX_ZOMBIE_CHUNKS (50)
zombie_chunk zombie_chunks[MAX_ZOMBIE_CHUNKS] = {0};

#define CHUNK_DURATION_OF_DROP (0.6f)
#define CHUNK_MAX_DURATION (5.0f)
#define CHUNK_FADE_TIME (2.0f)

void draw_zombie_chunks(platform_window* window);
void spawn_zombie_chunk(vec3f center);
void spawn_zombie_splatter(vec3f center);
void update_zombie_chunks_server();

#endif