#define ASSET_IMAGE_COUNT 50
#define ASSET_QUEUE_COUNT 50

#include <projectbase/project_base.h>

#include "include/SDL2/SDL_mixer.h"

#include "include/players.h"
#include "include/objects.h"
#include "include/map.h"
#include "include/audio.h"
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

#include "src/map.c"
#include "src/players.c"
#include "src/objects.c"
#include "src/audio.c"
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

#define CONFIG_DIRECTORY "zombieshooter"

void update_func(platform_window* window) {
	renderer->render_clear(window, rgb(0,255,0));
	update_game(window);
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

	if (ip && port) {
		if (strcmp(ip, "127.0.0.1") == 0) {
			start_server(port);
		}
		connect_to_server(ip, port);
	}

	log_info("STATE: GAMESTATE_PLAYING");
	global_state.state = GAMESTATE_PLAYING;
}

int main(int argc, char **argv)
{    
    platform_init(argc, argv, CONFIG_DIRECTORY);

	settings_set_number("USE_GPU", 1);
    platform_window *window = platform_open_window_ex("Zombies!", 800, 600, 1920, 1080, 500, 500, FLAGS_MINIMIZE, update_func, 0, 0, 0, 0);
	platform_toggle_vsync(window, false);
	//platform_toggle_fullscreen(window, true);

	settings_set_number("USE_GPU", 1);

	if (Mix_OpenAudio(48000, AUDIO_F32SYS, 2, 2048) == 0) {
		if (Mix_AllocateChannels(NUMBER_OF_AUDIO_CHANNELS) == 64) {
			log_info("Audio system initialized.");
			Mix_MasterVolume(MIX_MAX_VOLUME/4);
		}
		else {
			log_info("Channel allocation failed.");
		}
	}
	else {
		log_info("Audio failed.");
	}

	init_game();
	
	bool did_handle_args = false;
    while(platform_keep_running(window)) {
		if (global_asset_collection.done_loading_assets && !did_handle_args && global_state.state == GAMESTATE_LOADING_ASSETS) {
			handle_args(argc, argv);
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