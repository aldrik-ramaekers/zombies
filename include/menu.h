#ifndef INCLUDE_MENU
#define INCLUDE_MENU

#include <projectbase/project_base.h>

#define PROGRAM_VERSION "0.1.0 | "__DATE__

extern int current_res_index;
extern bool is_fullscreen;
vec2 available_resolutions[10];

void update_menu(platform_window* window);

#endif