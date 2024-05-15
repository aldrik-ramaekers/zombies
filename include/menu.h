#ifndef INCLUDE_MENU
#define INCLUDE_MENU

#include <projectbase/project_base.h>

#define PROGRAM_VERSION "0.1.0 | "__DATE__

extern int current_res_index;
extern bool is_fullscreen;
vec2 available_resolutions[] = {
	(vec2){1366, 769},
	(vec2){1440, 900},
	(vec2){1600, 900},
	(vec2){1920, 1080},
	(vec2){2560, 1080},
	(vec2){2560, 1440},
	(vec2){2560, 1600},
	(vec2){3440, 1440},
	(vec2){3840, 2160},
	(vec2){5120, 1440},
};

void update_menu(platform_window* window);

#endif