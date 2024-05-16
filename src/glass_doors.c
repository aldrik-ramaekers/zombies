#include "../include/glass_doors.h"

void create_glass_door(object o)
{
	for (int i = 0; i < MAX_GLASS_DOORS; i++)
	{
		if (glass_doors[i].obj.active) continue;
		glass_doors[i].obj = o;
		glass_doors[i].time_since_open = 0.0f;
		glass_doors[i].is_open = 0;
		return;
	}
}

void update_glass_doors_server()
{
	for (int i = 0; i < MAX_GLASS_DOORS; i++)
	{
		if (!glass_doors[i].obj.active) continue;
		bool do_open = 0;

		// Open for players
		for (int x = 0; x < MAX_PLAYERS; x++) {
			if (!players[x].active) continue;
			
			if (distance_between((vec2f){.x = glass_doors[i].obj.position.x, .y = glass_doors[i].obj.position.y}, 
				(vec2f){.x = players[x].playerx, .y = players[x].playery}) < 3.0f) {
				do_open = 1;
				break;
			}
		}

		// Open for zombies
		for (int x = 0; x < SERVER_MAX_ZOMBIES; x++) {
			zombie o = zombies[x];
			
			if (distance_between((vec2f){.x = glass_doors[i].obj.position.x, .y = glass_doors[i].obj.position.y}, 
				(vec2f){.x = o.position.x, .y = o.position.y}) < 3.0f) {
				do_open = 1;
				break;
			}
		}

		if (do_open && glass_doors[i].is_open == 0) {
			add_audio_event_to_queue(EVENT_DOOR_OPEN, player_id, glass_doors[i].obj.position);
		}
		else if (!do_open && glass_doors[i].is_open == 1) {
			add_audio_event_to_queue(EVENT_DOOR_CLOSE, player_id, glass_doors[i].obj.position);
		}

		glass_doors[i].is_open = do_open;
	}
}

void draw_glass_doors(platform_window* window, uint32_t ystart, uint32_t yend)
{
	for (int i = 0; i < MAX_GLASS_DOORS; i++)
	{
		if (!glass_doors[i].obj.active) continue;
		if (!((int)glass_doors[i].obj.position.y >= ystart && (int)glass_doors[i].obj.position.y <= yend+1)) continue;
		
		object o = glass_doors[i].obj;
		box box = get_box_of_object(window, o);

		image* img = 0;
		if (o.type == OBJECT_GLASS_DOOR_H) img = glass_doors[i].is_open ? img_glass_door_h_open : img_glass_door_h_closed;
		if (o.type == OBJECT_GLASS_DOOR_V) img = glass_doors[i].is_open ? img_glass_door_v_open : img_glass_door_v_closed;
		if (img) {
			renderer->render_image(img, box.tl_u.x, box.tl_u.y, 
				box.br_d.x - box.tl_d.x, box.br_d.y - box.tr_u.y);
		}
	}
}