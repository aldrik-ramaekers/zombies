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
	return s;
}

void update_sprite(sprite* sprite) {
	sprite->time += SERVER_TICK_RATE;
	int frame = sprite->time / sprite->sec_per_frame;
	sprite->current_frame = frame;
	if (frame >= sprite->frame_count) {
		sprite->time = 0.0f;
		sprite->current_frame = 0;
	}
}

sprite_frame sprite_get_frame(sprite* sprite) {
	sprite_frame frame = {0};
	int columns = sprite->img_width / sprite->frame_width;
	int rows = sprite->img_height/ sprite->frame_height;
	float column_percentage = 1.0f / columns;
	float row_percentage = 1.0f / rows;

	int column = sprite->current_frame % columns;
	int row = sprite->current_frame / columns;

	frame.tl = (vec2f){column * column_percentage, row * row_percentage};
	frame.tr = (vec2f){column * column_percentage + column_percentage, row * row_percentage};
	frame.bl = (vec2f){column * column_percentage, row * row_percentage + row_percentage};
	frame.br = (vec2f){column * column_percentage + column_percentage, row * row_percentage + row_percentage};

	/*
	frame.tl = (vec2f){0,0};
	frame.tr = (vec2f){0.25f,0};
	frame.bl = (vec2f){0,0.25f};
	frame.br = (vec2f){0.25f,0.25f};
	*/
	return frame;
}