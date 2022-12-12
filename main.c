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

#include "src/map.c"
#include "src/players.c"
#include "src/objects.c"
#include "src/zombies.c"
#include "src/bullets.c"
#include "src/math_helper.c"
#include "src/pathfinding.c"
#include "src/list.c"
#include "src/allocator.c"

#define CONFIG_DIRECTORY "zombieshooter"

font* fnt;
void update_func(platform_window* window) {
	renderer->render_clear(window, rgb(0,255,0));

	draw_grid(window);
	draw_spawners(window);

	char buf[200];
	sprintf(buf, "QUEUE: %d", global_pathfinding_queue.length);
	renderer->render_text(fnt, _global_camera.x, _global_camera.y, buf, rgb(255,255,255));
}

void resize_func(platform_window* window, u32 change_x,u32 change_y) {

} 

void close_func(platform_window* window) {
	
}

int main(int argc, char **argv)
{    
    platform_init(argc, argv, CONFIG_DIRECTORY);

    platform_window *window = platform_open_window_ex("Zombies!", 700, 700, 1200, 1000, 500, 500, FLAGS_MINIMIZE, update_func, resize_func, close_func, 0, 0);
	settings_set_number("USE_GPU", 1);
	platform_toggle_vsync(window, true);
	
	fnt = assets_load_font(mono_ttf, mono_ttf+mono_ttf_len, 16);
	load_map_from_data();
	create_objects();

	pathfinding_init();

	network_server *server = networking_create_server();

	thread t = thread_start(pathfinding_thread, 0);
	thread_detach(&t);

	network_client *client = network_connect_to_server("127.0.0.1", "27015");
	network_client_send(client, "Bing Bong");

    while(platform_keep_running(window)) {
        platform_handle_events();
    }

    settings_write_to_file();
    platform_destroy();
	pathfinding_destroy();

	network_client_close(client);
	networking_destroy_server(server);

	memory_print_leaks();

    return 0;
}