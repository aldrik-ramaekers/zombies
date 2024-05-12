#include "../include/menu.h"

void draw_background(platform_window* window)
{
	image* img = img_splash_art2;
	int imgw = img->width * (window->height/(float)img->height);
	int imgh = window->height;

	int imgx = (window->width - imgw) / 2 + _global_camera.x;
	int imgy = 0 + _global_camera.y;
	renderer->render_image(img, imgx, imgy, imgw, imgh);
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

void update_menu(platform_window* window)
{
	//draw_background(window);
	draw_screen(window);
}