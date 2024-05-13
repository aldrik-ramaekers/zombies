#include "../include/menu.h"

typedef enum t_menu_state {
	MENU_STATE_MAIN,
	MENU_STATE_LEVEL_SELECT,
} menu_state;

menu_state current_menu_state = MENU_STATE_MAIN;
float menu_state_change_duration = 0.2f;
float sec_since_state_change = 999.0f;
float sec_since_load_complete = 0.0f;
void draw_black_overlay(platform_window* window)
{
	if (img_splash_art2->loaded && fnt_4->loaded) {
		sec_since_load_complete += update_delta;
	}

	int fade = 255 - (sec_since_load_complete*255);
	if (fade < 0) fade = 0;
	renderer->render_rectangle(0, 0, window->width, window->height, rgba(0,0,0,fade));
}

float select_animation_duration = 0.0f;
static bool draw_menu_option(platform_window* window, int x, int y, int w, int h, char* text, bool is_hovered)
{
	color txt = rgb(102, 255, 102);
	color txt_bg = rgb(90, 176, 90);
	
	if (is_hovered) {
		renderer->render_rectangle(x, y, w, h, rgba(245,245,245, 100));
		//txt = rgb(255,255,255);
	}
	
	font* fnt = get_font(window, 1.0f);
	int offset_x = (w/2) - (renderer->calculate_text_width(fnt, text)/2);

	if (is_hovered && (select_animation_duration*100) < 2*M_PI) offset_x += sin(select_animation_duration*100.0f)*10.0f;

	renderer->render_text(fnt, x+offset_x + 1, y + (h/2) - (fnt->px_h/2) - 1, text, txt_bg);
	renderer->render_text(fnt, x+offset_x, y + (h/2) - (fnt->px_h/2), text, txt);

	if (_global_mouse.x + _global_camera.x > x && 
			_global_mouse.x + _global_camera.x < x + w && 
			_global_mouse.y + _global_camera.y > y && 
			_global_mouse.y + _global_camera.y < y + h) 
	{
		return true;
	}

	return false;
}

int selected_menu_option = 0;
void draw_screen(platform_window* window) {
	image* img = img_menu_screen;
	int imgw = img->width * (window->height/(float)img->height);
	int imgh = window->height;

	int imgx = (window->width - imgw) / 2 + _global_camera.x;
	int imgy = 0 + _global_camera.y;

	int text_offset_x = imgx + 1540 * (imgw/(float)img->width);
	int text_offset_y = imgy + 475 * (imgh/(float)img->height);

	int screen_w = 900 * (imgw/(float)img->width);
	int screen_h = 560 * (imgh/(float)img->height);

	renderer->render_image(img_splash_art2, text_offset_x, text_offset_y, screen_w, screen_h);
	renderer->render_rectangle(text_offset_x, text_offset_y, screen_w, screen_h, rgba(40,40,40, 200));

	select_animation_duration += update_delta;

	int item_h = screen_h / 6;
	if (draw_menu_option(window, text_offset_x, text_offset_y + item_h*0, screen_w, item_h, "Play Solo", selected_menu_option == 0)) {
		if (selected_menu_option != 0) {
			select_animation_duration = 0.0f;
			play_sound(-1, wav_menu_hover);
		}
		selected_menu_option = 0;
		if (is_left_clicked()) {
			play_sound(-1, wav_woosh);
			current_menu_state = MENU_STATE_LEVEL_SELECT;
			sec_since_state_change = 0.0f;
		}
	}

	if (draw_menu_option(window, text_offset_x, text_offset_y + item_h*1, screen_w, item_h, "Host Game", selected_menu_option == 1)) {
		if (selected_menu_option != 1) {
			select_animation_duration = 0.0f;
			play_sound(-1, wav_menu_hover);
		}
		selected_menu_option = 1;
	}

	if (draw_menu_option(window, text_offset_x, text_offset_y + item_h*2, screen_w, item_h, "Join Game", selected_menu_option == 2)) {
		if (selected_menu_option != 2) {
			select_animation_duration = 0.0f;
			play_sound(-1, wav_menu_hover);
		}
		selected_menu_option = 2;
	}

	if (draw_menu_option(window, text_offset_x, text_offset_y + item_h*3, screen_w, item_h, "Settings", selected_menu_option == 3)) {
		if (selected_menu_option != 3) {
			select_animation_duration = 0.0f;
			play_sound(-1, wav_menu_hover);
		}
		selected_menu_option = 3;
	}
	
	if (draw_menu_option(window, text_offset_x, text_offset_y + item_h*4, screen_w, item_h, "Credits", selected_menu_option == 4)) {
		if (selected_menu_option != 4) {
			select_animation_duration = 0.0f;
			play_sound(-1, wav_menu_hover);
		}
		selected_menu_option = 4;
	}

	if (draw_menu_option(window, text_offset_x, text_offset_y + item_h*5, screen_w, item_h, "Quit", selected_menu_option == 5)) {
		if (selected_menu_option != 5) {
			select_animation_duration = 0.0f;
			play_sound(-1, wav_menu_hover);
		}
		selected_menu_option = 5;
	}

	renderer->render_image(img, imgx, imgy, imgw, imgh);
}

vec4 draw_screen_change_animation(platform_window* window, float percentage)
{
	image* img = img_menu_screen;
	int imgw = img->width * (window->height/(float)img->height);
	int imgh = window->height;

	int src_imgx = (window->width - imgw) / 2 + _global_camera.x;
	int src_imgy = 0 + _global_camera.y;

	int dest_imgw = imgw*2;
	int dest_imgh = imgh*2;
	int dest_imgx = src_imgx - dest_imgw/4;
	int dest_imgy = src_imgy - dest_imgh/15;

	int p_imgw = imgw + ((dest_imgw - imgw)*percentage);
	int p_imgh = imgh + ((dest_imgh - imgh)*percentage);
	int p_imgx = src_imgx + ((dest_imgx - src_imgx)*percentage);
	int p_imgy = src_imgy + ((dest_imgy - src_imgy)*percentage);

	renderer->render_image(img, p_imgx, p_imgy, p_imgw, p_imgh);

	int text_offset_x = p_imgx + 1540*2 * (imgw/(float)img->width);
	int text_offset_y = p_imgy + 475*2 * (imgh/(float)img->height);

	int screen_w = 900*2 * (imgw/(float)img->width);
	int screen_h = 560*2 * (imgh/(float)img->height);
	
	return (vec4){text_offset_x, text_offset_y, screen_w, screen_h};
}

vec4 renderable_rec;
image* level_img_bg = 0;
int selected_level_index = 0;
void draw_level_select(platform_window* window)
{
	color txt = rgb(102, 255, 102);
	color txt_bg = rgb(90, 176, 90);

	if (level_img_bg == 0) level_img_bg = img_splash_art2;

	renderer->render_image(level_img_bg, renderable_rec.x, renderable_rec.y, renderable_rec.w, renderable_rec.h);
	renderer->render_rectangle(renderable_rec.x, renderable_rec.y, renderable_rec.w, renderable_rec.h, rgba(40,40,40, 200));

	char* level_select_text = "Select Map";
	font* fnt = get_font(window, 1.5f);
	int text_w = renderer->calculate_text_width(fnt, level_select_text);
	int text_y = renderable_rec.y + 30;

	renderer->render_text(fnt, renderable_rec.x + (renderable_rec.w/2)-(text_w/2), text_y, level_select_text, txt);
	text_y += 30;

	select_animation_duration += update_delta;
	
	int item_h = renderable_rec.h / 6;
	if (draw_menu_option(window, renderable_rec.x, text_y, renderable_rec.w, item_h, "Amalthea Mark II", selected_level_index == 0)) {
		if (selected_level_index != 0) {
			select_animation_duration = 0.0f;
			play_sound(-1, wav_menu_hover);
		}
		selected_level_index = 0;
		level_img_bg = img_splash_art3;
		if (is_left_clicked()) {
			global_scene_state = SCENE_GAME;
			// start game/go to lobby.
		}
	}
	else {
		selected_level_index = -1;
		level_img_bg = img_splash_art2;
	}

	renderable_rec = draw_screen_change_animation(window, 1.0f);
}

void update_menu(platform_window* window)
{
	sec_since_state_change += update_delta;
	if (current_menu_state != MENU_STATE_MAIN) {
		if (sec_since_state_change <= menu_state_change_duration) {
			renderable_rec = draw_screen_change_animation(window, sec_since_state_change / menu_state_change_duration);
		}
		else if (current_menu_state == MENU_STATE_LEVEL_SELECT) {
			draw_level_select(window);
		}

		if (keyboard_is_key_down(KEY_ESCAPE))
		{
			play_sound(-1, wav_woosh);
			current_menu_state = MENU_STATE_MAIN;
			if (sec_since_state_change >= menu_state_change_duration)
				sec_since_state_change = 0.0f;
			else
				sec_since_state_change = menu_state_change_duration - sec_since_state_change;
		}
	}
	else {
		if (sec_since_state_change <= menu_state_change_duration) {
			draw_screen_change_animation(window, 1.0f - (sec_since_state_change / menu_state_change_duration));
		}
		else {
			draw_screen(window);
		}
	}
		
	draw_black_overlay(window);
}