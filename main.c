#include <projectbase/project_base.h>

#include "include/players.h"
#include "include/objects.h"
#include "include/map.h"
#include "include/zombies.h"
#include "include/math_helper.h"
#include "include/bullets.h"
#include "include/pathfinding.h"
#include "include/list.h"
#include "include/allocator.h"
#include "include/game.h"
#include "include/protocol.h"
#include "include/guns.h"

#include "src/map.c"
#include "src/players.c"
#include "src/objects.c"
#include "src/zombies.c"
#include "src/bullets.c"
#include "src/math_helper.c"
#include "src/pathfinding.c"
#include "src/list.c"
#include "src/allocator.c"
#include "src/game.c"
#include "src/guns.c"
#include "src/protocol.c"

#define CONFIG_DIRECTORY "zombieshooter"

font* fnt;
void update_func(platform_window* window) {
	renderer->render_clear(window, rgb(0,255,0));
	update_game(window);

	char id[50];
	sprintf(id, "%d", my_id);
	renderer->render_text(fnt, 0, 0, id, rgb(255,255,255));
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
}

int main(int argc, char **argv)
{    
    platform_init(argc, argv, CONFIG_DIRECTORY);

    platform_window *window = platform_open_window_ex("Zombies!", 700, 700, 1200, 1000, 500, 500, FLAGS_MINIMIZE, update_func, 0, 0, 0, 0);
	settings_set_number("USE_GPU", 1);
	platform_toggle_vsync(window, true);

	init_game();
	handle_args(argc, argv);

	fnt = assets_load_font(mono_ttf, mono_ttf+mono_ttf_len, 24);

    while(platform_keep_running(window)) {
        platform_handle_events();
    }

    settings_write_to_file();
    platform_destroy();
	pathfinding_destroy();
	destroy_game();
	
	memory_print_leaks();

    return 0;
}