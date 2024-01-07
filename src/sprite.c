#include "../include/sprite.h"

sprite create_sprite(image* img, int frame_count, int fwidth, int fheight, float sec_per_frame) {
	sprite s;
	s.current_frame = 0;
	s.frame_count = frame_count;
	s.frame_height = fheight;
	s.frame_width = fwidth;
	s.img_width = img->width;
	s.img_height = img->height;
	s.sec_per_frame = sec_per_frame;
	s.time = 0.0f;
	s.frame_start = 0;
	s.zoom = 1.0f;
	s.loop = true;
	return s;
}

void update_sprite(sprite* sprite) {
	sprite->time += SERVER_TICK_RATE;
	if (sprite->sec_per_frame == 0.0f) return; // sprite does not play.
	int frame = sprite->time / sprite->sec_per_frame;
	sprite->current_frame = frame;
	if (frame >= sprite->frame_count) {
		if (sprite->loop) {
			sprite->time = 0.0f;
			sprite->current_frame = 0;
		}
		else {
			sprite->current_frame = sprite->frame_count-1;
		}
	}
}

void sprite_set_current_frame(sprite* sprite, int index) {
	sprite->current_frame = index;
}

sprite_frame sprite_get_frame(image* img, sprite* sprite) {
	sprite->img_width = img->width;
	sprite->img_height = img->height;

	sprite_frame frame = {0};
	int columns = sprite->img_width / sprite->frame_width;
	int rows = sprite->img_height/ sprite->frame_height;
	float column_percentage = 1.0f / columns;
	float row_percentage = 1.0f / rows;

	int column = (sprite->frame_start + sprite->current_frame) % columns;
	int row = (sprite->frame_start + sprite->current_frame) / columns;

	float offsetx = column_percentage * (sprite->zoom-1.0f);
	float offsety = row_percentage * (sprite->zoom-1.0f);

	frame.tl = (vec2f){column * column_percentage + offsetx, row * row_percentage + offsety};
	frame.tr = (vec2f){column * column_percentage + column_percentage - offsetx, row * row_percentage + offsety};
	frame.bl = (vec2f){column * column_percentage + offsetx, row * row_percentage + row_percentage - offsety};
	frame.br = (vec2f){column * column_percentage + column_percentage - offsetx, row * row_percentage + row_percentage - offsety};

	/*
	frame.tl = (vec2f){0,0};
	frame.tr = (vec2f){0.25f,0};
	frame.bl = (vec2f){0,0.25f};
	frame.br = (vec2f){0.25f,0.25f};
	*/
	return frame;
}

sprite_frame sprite_swap_rotate_90(sprite_frame frame) {
	vec2f tl = frame.bl;
	vec2f tr = frame.tl;
	vec2f bl = frame.br;
	vec2f br = frame.tr;
	frame.tl = tl;
	frame.tr = tr;
	frame.bl = bl;
	frame.br = br;
	return frame;
}

sprite_frame sprite_swap_frame_horizontally(sprite_frame frame) {
	vec2f tl = frame.tr;
	vec2f tr = frame.tl;
	vec2f bl = frame.br;
	vec2f br = frame.bl;
	frame.tl = tl;
	frame.tr = tr;
	frame.bl = bl;
	frame.br = br;
	return frame;
}