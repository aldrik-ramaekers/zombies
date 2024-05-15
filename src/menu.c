#include "../include/menu.h"

int current_res_index = 0;
bool is_fullscreen = false;

typedef enum t_menu_state {
	MENU_STATE_MAIN,
	MENU_STATE_LEVEL_SELECT,
	MENU_STATE_CREDITS,
	MENU_STATE_SETTINGS,
	MENU_STATE_JOIN_GAME,
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
	if (draw_menu_option(window, text_offset_x, text_offset_y + item_h*0, screen_w, item_h, "Start Game", selected_menu_option == 0)) {
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

	if (draw_menu_option(window, text_offset_x, text_offset_y + item_h*1, screen_w, item_h, "Join Game", selected_menu_option == 1)) {
		if (selected_menu_option != 1) {
			select_animation_duration = 0.0f;
			play_sound(-1, wav_menu_hover);
		}
		selected_menu_option = 1;

		if (is_left_clicked()) {
			play_sound(-1, wav_woosh);
			current_menu_state = MENU_STATE_JOIN_GAME;
			sec_since_state_change = 0.0f;
			_global_keyboard.take_input = 1;
		}
	}

	if (draw_menu_option(window, text_offset_x, text_offset_y + item_h*2, screen_w, item_h, "Settings", selected_menu_option == 2)) {
		if (selected_menu_option != 2) {
			select_animation_duration = 0.0f;
			play_sound(-1, wav_menu_hover);
		}
		selected_menu_option = 2;
		if (is_left_clicked()) {
			play_sound(-1, wav_woosh);
			current_menu_state = MENU_STATE_SETTINGS;
			sec_since_state_change = 0.0f;
		}	
	}
	
	if (draw_menu_option(window, text_offset_x, text_offset_y + item_h*3, screen_w, item_h, "Credits", selected_menu_option == 3)) {
		if (selected_menu_option != 3) {
			select_animation_duration = 0.0f;
			play_sound(-1, wav_menu_hover);
		}
		selected_menu_option = 3;
		if (is_left_clicked()) {
			play_sound(-1, wav_woosh);
			current_menu_state = MENU_STATE_CREDITS;
			sec_since_state_change = 0.0f;
		}
	}

	if (draw_menu_option(window, text_offset_x, text_offset_y + item_h*4, screen_w, item_h, "Quit", selected_menu_option == 4)) {
		if (selected_menu_option != 4) {
			select_animation_duration = 0.0f;
			play_sound(-1, wav_menu_hover);
		}
		selected_menu_option = 4;
		if (is_left_clicked()) {
			window->is_open = 0;
		}
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
			start_solo_game();
		}
	}
	else {
		selected_level_index = -1;
		level_img_bg = img_splash_art2;
	}

	renderable_rec = draw_screen_change_animation(window, 1.0f);
}

void draw_credits(platform_window* window)
{
	color txt = rgb(102, 255, 102);

	int text_x = renderable_rec.x;
	int text_y = renderable_rec.y + 20;

	renderer->render_image(img_splash_art2, renderable_rec.x, renderable_rec.y, renderable_rec.w, renderable_rec.h);
	renderer->render_rectangle(renderable_rec.x, renderable_rec.y, renderable_rec.w, renderable_rec.h, rgba(40,40,40, 200));

	#define CREDITS_ADD_LINE(__txt, __fnt) {font* _fnt = get_font(window, __fnt); \
		renderer->render_text(_fnt, text_x + (renderable_rec.w/2) - (renderer->calculate_text_width(_fnt, __txt)/2), text_y, __txt, txt); \
		text_y += _fnt->px_h + 10;}
	
	CREDITS_ADD_LINE("Zombies!", 2.0f);
	CREDITS_ADD_LINE("", 1.0f);
	CREDITS_ADD_LINE("A game by:", 0.8f);
	CREDITS_ADD_LINE("Aldrik Ramaekers", 1.0f);
	CREDITS_ADD_LINE("", 1.0f);
	CREDITS_ADD_LINE("Design, code, textures and levels by:", 0.8f);
	CREDITS_ADD_LINE("Aldrik Ramaekers", 1.0f);
	CREDITS_ADD_LINE("", 1.0f);
	CREDITS_ADD_LINE("Sounds and music by numerous artists found at freesound.org", 1.0f);
	CREDITS_ADD_LINE("", 1.0f);
	CREDITS_ADD_LINE("Splash art has been created using text2img AI", 1.0f);
	CREDITS_ADD_LINE("", 1.0f);
	CREDITS_ADD_LINE("Thanks for playing!", 1.0f);

	renderable_rec = draw_screen_change_animation(window, 1.0f);
}

static bool draw_settings_btn(platform_window* window, char* txt, int x, int y, int w, int h)
{
	font* fnt = get_font(window, 1.0f);

	static int was_hovered = 0;
	bool result = false;
	if (_global_mouse.x + _global_camera.x > x && 
			_global_mouse.x + _global_camera.x < x + w && 
			_global_mouse.y + _global_camera.y > y && 
			_global_mouse.y + _global_camera.y < y + h) 
	{
		result = true;

		if (result && was_hovered == 0) {
			play_sound(-1, wav_menu_hover);
		}

		was_hovered = x+y;
	}
	else if (was_hovered == x+y) {
		was_hovered = 0;
	}

	int pad = 2;
	renderer->render_rectangle(x, y, w, h, rgba(0,0,0, result ? 240 : 160));
	renderer->render_rectangle(x+pad, y+pad, w-pad*2, h-pad*2, rgba(255,255,255, result ? 240 : 160));
	renderer->render_text(fnt, x+(w/2)-(fnt->px_h/2), y+(h/2)-(fnt->px_h/2), txt, rgb(0,0,0));

	return result;
}

void draw_settings(platform_window* window)
{
	color txt = rgb(255,255,255);

	int btn_s = 50;
	int row_y = renderable_rec.y + 50;
	int btn_offset = 100;

	renderer->render_image(img_splash_art2, renderable_rec.x, renderable_rec.y, renderable_rec.w, renderable_rec.h);
	renderer->render_rectangle(renderable_rec.x, renderable_rec.y, renderable_rec.w, renderable_rec.h, rgba(40,40,40, 200));
	font* fnt = get_font(window, 1.5f);

	{ // Resolution.
		renderer->render_rectangle(renderable_rec.x + btn_offset, row_y, renderable_rec.w - (btn_offset*2), btn_s, rgba(245,245,245, 100));

		if (draw_settings_btn(window, "<", renderable_rec.x + btn_offset, row_y, btn_s, btn_s)) {
			if (is_left_clicked()) {
				current_res_index--;
				if (current_res_index < 0) // Loop around
					current_res_index = sizeof(available_resolutions)/sizeof(vec2)-1;

				if (is_fullscreen) {
					is_fullscreen = 0;
					settings_set_number("FULLSCRN", is_fullscreen);
					platform_toggle_fullscreen(window, is_fullscreen);
				}

				settings_set_number("RES_INDEX", current_res_index);
				platform_window_set_size(window, available_resolutions[current_res_index].x, available_resolutions[current_res_index].y);
			}
		}

		if (draw_settings_btn(window, ">", renderable_rec.x + renderable_rec.w - btn_offset - btn_s, row_y, btn_s, btn_s)) {
			if (is_left_clicked()) {
				current_res_index++;
				if (current_res_index >= sizeof(available_resolutions)/sizeof(vec2)) // Loop around
					current_res_index = 0;

				if (is_fullscreen) {
					is_fullscreen = 0;
					settings_set_number("FULLSCRN", is_fullscreen);
					platform_toggle_fullscreen(window, is_fullscreen);
				}

				settings_set_number("RES_INDEX", current_res_index);
				platform_window_set_size(window, available_resolutions[current_res_index].x, available_resolutions[current_res_index].y);
			}
		}

		// Resolution text.
		char resolution_txt[50];
		sprintf(resolution_txt, "%d x %d", available_resolutions[current_res_index].x, available_resolutions[current_res_index].y);
		int text_w = renderer->calculate_text_width(fnt, resolution_txt);
		renderer->render_text(fnt, renderable_rec.x + (renderable_rec.w/2) - (text_w/2), row_y + (btn_s/2) - (fnt->px_h/2), resolution_txt, txt);
	}

	row_y += btn_s + 20;

	{ // Fullscreen.
		renderer->render_rectangle(renderable_rec.x + btn_offset, row_y, renderable_rec.w - (btn_offset*2), btn_s, rgba(245,245,245, 100));

		if (draw_settings_btn(window, "<", renderable_rec.x + btn_offset, row_y, btn_s, btn_s)) {
			if (is_left_clicked()) {
				is_fullscreen = !is_fullscreen;
				settings_set_number("FULLSCRN", is_fullscreen);
				platform_toggle_fullscreen(window, is_fullscreen);
			}
		}

		if (draw_settings_btn(window, ">", renderable_rec.x + renderable_rec.w - btn_offset - btn_s, row_y, btn_s, btn_s)) {
			if (is_left_clicked()) {
				is_fullscreen = !is_fullscreen;
				settings_set_number("FULLSCRN", is_fullscreen);
				platform_toggle_fullscreen(window, is_fullscreen);
			}
		}

		// fullscreen text.
		char fullscrn_txt[50];
		sprintf(fullscrn_txt, "%s", is_fullscreen ? "Fullscreen" : "Windowed");
		int text_w = renderer->calculate_text_width(fnt, fullscrn_txt);
		renderer->render_text(fnt, renderable_rec.x + (renderable_rec.w/2) - (text_w/2), row_y + (btn_s/2) - (fnt->px_h/2), fullscrn_txt, txt);
	}

	renderable_rec = draw_screen_change_animation(window, 1.0f);
}

void draw_join_game(platform_window* window)
{
	if (_global_keyboard.input_text_len >= 20) {
		_global_keyboard.input_text[19] = 0;
		keyboard_set_input_text(_global_keyboard.input_text);
	}

	color txt = rgb(102, 255, 102);
	int tb_offset = 100;
	int tb_w = renderable_rec.w - (tb_offset*2);
	int tb_h = 50;
	int tb_pad = 2;
	int render_y = renderable_rec.y + tb_offset;

	renderer->render_image(img_splash_art2, renderable_rec.x, renderable_rec.y, renderable_rec.w, renderable_rec.h);
	renderer->render_rectangle(renderable_rec.x, renderable_rec.y, renderable_rec.w, renderable_rec.h, rgba(40,40,40, 200));

	char* join_game_text = "Join Game";
	font* fnt = get_font(window, 1.5f);
	int text_w = renderer->calculate_text_width(fnt, join_game_text);
	int text_y = renderable_rec.y + 30;

	renderer->render_text(fnt, renderable_rec.x + (renderable_rec.w/2)-(text_w/2), text_y, join_game_text, txt);

	render_y += 30;

	vec4 tb_rec = (vec4){renderable_rec.x + tb_offset, render_y, tb_w, tb_h};

	renderer->render_rectangle(tb_rec.x, tb_rec.y, tb_rec.w, tb_rec.h, rgba(0,0,0, 160));
	renderer->render_rectangle(tb_rec.x+tb_pad, tb_rec.y+tb_pad, tb_rec.w-tb_pad*2, tb_rec.h-tb_pad*2, rgba(255,255,255, 160));

	int txt_x = renderer->render_text(fnt, tb_rec.x+tb_pad + 20, tb_rec.y+tb_pad + (tb_h/2) - (fnt->px_h/2), _global_keyboard.input_text, txt);
	renderer->render_rectangle(tb_rec.x+tb_pad + 20+txt_x, tb_rec.y+tb_pad + 10, 2, tb_h - 25, txt);

	text_y += 200;
	int item_h = 40;

	select_animation_duration += update_delta;

	static bool btn_hovered = 0;
	bool do_join = 0;
	if (draw_menu_option(window, renderable_rec.x, text_y, renderable_rec.w, item_h, "Join", btn_hovered)) {
		if (btn_hovered != 1) {
			select_animation_duration = 0.0f;
			play_sound(-1, wav_menu_hover);
		}
		btn_hovered = 1;

		if (is_left_clicked()) {
			play_sound(-1, wav_woosh);
			do_join = 1;
		}
	}
	else {
		btn_hovered = 0;
	}

	if (keyboard_is_key_pressed(KEY_ENTER) || do_join) {
		char* ip = _global_keyboard.input_text;
		if (_global_keyboard.input_text_len > 0) {
			if (connect_to_game(ip, DEFAULT_PORT)) {
				global_scene_state = SCENE_GAME;
			}
			else {
				log_infox("Failed to connect to %s", ip);
			}
		}
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
		else if (current_menu_state == MENU_STATE_CREDITS) {
			draw_credits(window);
		}
		else if (current_menu_state == MENU_STATE_SETTINGS) {
			draw_settings(window);
		}
		else if (current_menu_state == MENU_STATE_JOIN_GAME) {
			draw_join_game(window);
		}

		if (keyboard_is_key_down(KEY_ESCAPE))
		{
			_global_keyboard.take_input = 0;
			keyboard_set_input_text("");
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

	// Version
	{
		renderer->render_text(fnt_16, 10, window->height - fnt_16->px_h - 10, PROGRAM_VERSION, rgb(255,255,255));
	}
		
	draw_black_overlay(window);
}