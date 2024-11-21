#define ASSET_IMAGE_COUNT 150
#define ASSET_QUEUE_COUNT 150
#define ASSET_FONT_COUNT 20
#define NUM_AUDIO_CHANNELS 64

#include <float.h>

#ifndef __FLT_MAX__
#define __FLT_MAX__ FLT_MAX
#endif

#ifndef M_PI
#define _USE_MATH_DEFINES
#endif

#include "project-base/src/project_base.h"

#include "include/SDL2/SDL_mixer.h"

#include "include/network.h"
#include "include/players.h"
#include "include/objects.h"
#include "include/map.h"
#include "include/audio.h"
#include "include/rounds.h"
#include "include/zombies.h"
#include "include/math_helper.h"
#include "include/bullets.h"
#include "include/throwables.h"
#include "include/pathfinding.h"
#include "include/list.h"
#include "include/allocator.h"
#include "include/game.h"
#include "include/protocol.h"
#include "include/guns.h"
#include "include/overlay.h"
#include "include/asset_defs.h"
#include "include/drops.h"
#include "include/wall_item.h"
#include "include/zombie_chunk.h"
#include "include/sprite.h"
#include "include/editor.h"
#include "include/glass_doors.h"
#include "include/menu.h"
#include "include/fog_of_war.h"

#include "src/map.c"
#include "src/players.c"
#include "src/objects.c"
#include "src/audio.c"
#include "src/rounds.c"
#include "src/zombies.c"
#include "src/bullets.c"
#include "src/throwables.c"
#include "src/math_helper.c"
#include "src/pathfinding.c"
#include "src/list.c"
#include "src/allocator.c"
#include "src/game.c"
#include "src/guns.c"
#include "src/protocol.c"
#include "src/overlay.c"
#include "src/asset_defs.c"
#include "src/drops.c"
#include "src/wall_item.c"
#include "src/zombie_chunk.c"
#include "src/sprite.c"
#include "src/editor.c"
#include "src/glass_doors.c"
#include "src/menu.c"
#include "src/fog_of_war.c"

#define CONFIG_DIRECTORY "zombieshooter"

void update_func(platform_window* window) {
	renderer->render_clear(window, rgb(0,0,0));

	if (global_scene_state == SCENE_GAME) {
		update_game(window);
	}
	else if (global_scene_state == SCENE_MAIN_MENU) {
		update_menu(window);
	}
}


void handle_args(int argc, char **argv) {
	char* ip = 0;
	char* port = 0;
	for (int i = 1; i < argc; i++) {
		char* prev_str = argv[i-1];
		char* str = argv[i];
		if (strcmp(prev_str, "-ip") == 0) {
			ip = str;
		}
		if (strcmp(prev_str, "-port") == 0) {
			port = str;
		}
	}

	connect_to_game(ip, port);
}

int main(int argc, char **argv)
{
	fill_game_structs();

    platform_init(argc, argv, CONFIG_DIRECTORY);

	is_fullscreen = settings_get_number_or_default("FULLSCRN", 1);
	current_res_index = settings_get_number_or_default("RES_INDEX", 2);

	printf("%d %d x %d\n", current_res_index, available_resolutions[current_res_index].x, 
		available_resolutions[current_res_index].y);

	settings_set_number("USE_GPU", 1);
    platform_window *window = platform_open_window_ex("Zombies!", 
		available_resolutions[current_res_index].x, 
		available_resolutions[current_res_index].y, 
		UINT16_MAX, UINT16_MAX, 1366, 768, FLAGS_MINIMIZE, update_func, 0, 0, 0, 0);
	platform_toggle_vsync(window, true);
	if (is_fullscreen) platform_toggle_fullscreen(window, is_fullscreen);

	settings_set_number("USE_GPU", 1);

	if (Mix_OpenAudio(48000, AUDIO_F32SYS, 2, 2048) == 0) {
		if (Mix_AllocateChannels(NUMBER_OF_AUDIO_CHANNELS) == NUM_AUDIO_CHANNELS) {
			log_info("Audio system initialized.");
			Mix_MasterVolume(MIX_MAX_VOLUME);
		}
		else {
			log_info("Channel allocation failed.");
		}
	}
	else {
		log_info("Audio failed.");
	}
	
	load_menu_assets();
	play_music(music_menu);

	init_game();
	
	bool did_handle_args = false;
    while(platform_keep_running(window)) {
		if (global_asset_collection.done_loading_assets && !did_handle_args && global_state.state == GAMESTATE_LOADING_ASSETS) {
			if (argc > 1) {
				handle_args(argc, argv);
				global_scene_state = SCENE_GAME;
			}
			did_handle_args = true;
		}

        platform_handle_events();
    }

    settings_write_to_file();
    platform_destroy();
	pathfinding_destroy();
	destroy_game();
	
	memory_print_leaks();

    return 0;
}