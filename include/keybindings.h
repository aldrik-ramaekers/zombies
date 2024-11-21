#ifndef INCLUDE_KEYBINDINGS
#define INCLUDE_KEYBINDINGS

#include "../project-base/src/project_base.h"

typedef struct t_keybinding {
	int key;
	char* text;
} keybinding;

keybinding keybind_wall_purchase = {KEY_F, "F"};

#endif