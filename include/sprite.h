#ifndef INCLUDE_SPRITE
#define INCLUDE_SPRITE

#include <projectbase/project_base.h>

typedef struct t_sprite {
	image* image;
	int current_frame;
	int frame_count;
	int frame_width;
	int frame_height;
	float sec_per_frame;
	float time;
} sprite;

typedef struct t_sprite_frame {
	vec2f tl;
	vec2f tr;
	vec2f bl;
	vec2f br;
} sprite_frame;

sprite create_sprite(image* img, int frame_count, int fwidth, int fheight, float sec_per_frame);
void update_sprite(sprite* sprite);
sprite_frame sprite_get_frame(sprite* sprite);

#endif