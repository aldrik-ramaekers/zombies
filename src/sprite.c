#include "../include/sprite.h"

sprite create_sprite(image* img, int frame_count, int fwidth, int fheight, float sec_per_frame) {
	sprite s;
	s.current_frame = 0;
	s.frame_count = frame_count;
	s.frame_height = fheight;
	s.frame_width = fwidth;
	s.image = img;
	s.sec_per_frame = sec_per_frame;
	s.time = 0.0f;
	return s;
}

void update_sprite(sprite* sprite) {
	int frame = sprite->time / sprite->sec_per_frame;
	sprite->current_frame = frame;
	sprite->time += update_delta;
}

sprite_frame get_frame(sprite* sprite) {
	sprite_frame frame;
	frame.tl = (vec2f){0,0};
	frame.tr = (vec2f){1,0};
	frame.bl = (vec2f){0,1};
	frame.br = (vec2f){1,1};
	return frame;
}