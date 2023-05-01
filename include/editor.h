#ifndef INCLUDE_EDITOR
#define INCLUDE_EDITOR

#ifdef MODE_DEBUG
bool is_editing_map = false;

void update_editor(platform_window* window);
void draw_editor(platform_window* window);
#endif

#endif