#ifndef INCLUDE_MENU
#define INCLUDE_MENU

#include "../project-base/src/project_base.h"

#define PROGRAM_VERSION "0.1 | "__DATE__

typedef enum t_menu_state {
	MENU_STATE_MAIN,
	MENU_STATE_LEVEL_SELECT,
	MENU_STATE_CREDITS,
	MENU_STATE_SETTINGS,
	MENU_STATE_JOIN_GAME,
} menu_state;

extern int current_res_index;
extern bool is_fullscreen;
vec2 available_resolutions[10];
extern menu_state current_menu_state;

void update_menu(platform_window* window);

#endif