#ifndef INCLUDE_SPRITE
#define INCLUDE_SPRITE

#include "../project-base/src/project_base.h"

typedef struct t_sprite {
	int current_frame;
	int frame_count;
	int frame_width;
	int frame_height;
	float sec_per_frame;
	float time;
	int img_width;
	int img_height;
	int frame_start;
	float zoom;
	bool loop;
} sprite;

typedef struct t_sprite_frame {
	vec2f tl;
	vec2f tr;
	vec2f bl;
	vec2f br;
} sprite_frame;

sprite create_sprite(image* img, int frame_count, int fwidth, int fheight, float sec_per_frame);
void update_sprite(sprite* sprite);
void sprite_set_current_frame(sprite* sprite, int index);
sprite_frame sprite_swap_frame_horizontally(sprite_frame frame);
sprite_frame sprite_swap_rotate_90(sprite_frame frame);
sprite_frame sprite_get_frame(image* img, sprite* sprite);

#endif