#include "include/players.h"

float get_bullet_size_in_tile(platform_window* window) {
	return 1 / 8.0f;
}

float get_bullet_size(platform_window* window) {
	return get_tile_width(window) * get_bullet_size_in_tile(window);
}

float get_player_size_in_tile() {
	return 0.5;
}

float get_player_size(platform_window* window) {
	int player_size = get_tile_width(window) * get_player_size_in_tile();
}

object check_if_player_collided_with_object(platform_window* window) {
	map_info info = get_map_info(window);
	float player_size = get_player_size(window);

	for (int i = 0; i < max_objects; i++) {
		object o = objects[i];
		if (!o.active) continue;

		box box = get_box_of_object(window, o);
		float x_to_check = playerx;
		float player_size_in_tile_px = player_size / (float)info.tile_width;
		float y_to_check = playery + player_size_in_tile_px;

		if (x_to_check+player_size_in_tile_px >= o.position.x && x_to_check <= o.position.x+o.size.x
		 && y_to_check >= o.position.y && y_to_check <= o.position.y+o.size.y) {
			return o;
		}
	}

	return (object){0};
}

void take_player_input(platform_window* window) {
	float speed = 0.1f;
	float pad_between_player_and_obj = 0.01f;

	float old_x = playerx;
	float old_y = playery;

	if (keyboard_is_key_down(KEY_W)) {
		float newy = playery - speed;
		if (is_in_bounds(window, playerx, newy)) {
			playery = newy;
			object o = check_if_player_collided_with_object(window);
			if (o.active) playery = o.position.y+o.size.y - get_player_size_in_tile() + pad_between_player_and_obj;
		}
	}

	if (keyboard_is_key_down(KEY_S)) {
		float newy = playery + speed;
		if (is_in_bounds(window, playerx, newy)) {
			playery = newy;
			object o = check_if_player_collided_with_object(window);
			if (o.active) playery = o.position.y - get_player_size_in_tile() - pad_between_player_and_obj;
		}
	}

	if (keyboard_is_key_down(KEY_A)) {
		float newx = playerx - speed;
		if (is_in_bounds(window, newx, playery)) {
			playerx = newx;
			object o = check_if_player_collided_with_object(window);
			if (o.active) playerx = o.position.x+o.size.x + pad_between_player_and_obj;
		}
	}

	if (keyboard_is_key_down(KEY_D)) {
		float newx = playerx + speed;
		if (is_in_bounds(window, newx, playery)) {
			playerx = newx;
			object o = check_if_player_collided_with_object(window);
			if (o.active) playerx = o.position.x-get_player_size_in_tile() - pad_between_player_and_obj;
		}
	}
}

void draw_player(platform_window* window) {
	int size = get_tile_width(window) / 2;
	map_info info = get_map_info(window);
	float height = get_height_of_tile_under_coords(window, playerx, playery);

	take_player_input(window);

	sec_since_last_shot += update_delta;
	if (is_left_down()) {
		if (sec_since_last_shot > 0.3f) {
			shoot(window);
			sec_since_last_shot = 0.0f;
		}
	}

	float player_render_x = playerx*info.tile_width + (playery*info.px_incline);
	float player_render_y = playery*info.tile_height - (height*info.px_raised_per_h);
	renderer->render_rectangle(player_render_x, player_render_y, size, size, rgb(200,150,120));

	_global_camera.x = -(window->width / 2) + player_render_x;
	_global_camera.y = -(window->height / 2) + player_render_y;
}

void shoot(platform_window* window) {
	map_info info = get_map_info(window);
	float bullet_range = 100.0f;

	float hh = get_height_of_tile_under_coords(window, playerx, playery);

	float dirx = (_global_mouse.x - (window->width/2));
	float diry = (_global_mouse.y - (window->height/2));
	double length = sqrt(dirx * dirx + diry * diry);
	dirx /= length;
	diry /= length;

	float bulletx = playerx + (get_player_size_in_tile()/2);
	float bullety = playery + (get_player_size_in_tile()/2);
	float bullet_end_point_x = bulletx+dirx*bullet_range;
	float bullet_end_point_y = bullety+diry*bullet_range;

	for (int i = 0; i < max_bullets; i++) {
		bullet b = bullets[i];
		if (b.active) continue;

		bullets[i] = (bullet){true, bulletx, bullety, hh, bullet_end_point_x, bullet_end_point_y};
		break;
	}
}

bool onSegment(vec2f p, vec2f q, vec2f r)
{
    if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) &&
        q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y))
       return true;
  
    return false;
}

int orientation(vec2f p, vec2f q, vec2f r)
{
    int val = (q.y - p.y) * (r.x - q.x) -
              (q.x - p.x) * (r.y - q.y);
  
    if (val == 0) return 0;  // collinear
  
    return (val > 0)? 1: 2; // clock or counterclock wise
}

bool lines_intersect(vec2f p1, vec2f q1, vec2f p2, vec2f q2)
{
    int o1 = orientation(p1, q1, p2);
    int o2 = orientation(p1, q1, q2);
    int o3 = orientation(p2, q2, p1);
    int o4 = orientation(p2, q2, q1);
  
    // General case
    if (o1 != o2 && o3 != o4)
        return true;
  
    // Special Cases
    // p1, q1 and p2 are collinear and p2 lies on segment p1q1
    if (o1 == 0 && onSegment(p1, p2, q1)) return true;
  
    // p1, q1 and q2 are collinear and q2 lies on segment p1q1
    if (o2 == 0 && onSegment(p1, q2, q1)) return true;
  
    // p2, q2 and p1 are collinear and p1 lies on segment p2q2
    if (o3 == 0 && onSegment(p2, p1, q2)) return true;
  
     // p2, q2 and q1 are collinear and q1 lies on segment p2q2
    if (o4 == 0 && onSegment(p2, q1, q2)) return true;
  
    return false; // Doesn't fall in any of the above cases
}

bool check_if_bullet_collided_with_object(bullet b, platform_window* window) {
	map_info info = get_map_info(window);
	float size = get_bullet_size_in_tile(window);

	for (int i = 0; i < max_objects; i++) {
		object o = objects[i];
		if (!o.active) continue;
		if (b.position.x <= o.position.x + o.size.x && b.position.x + size >= o.position.x && 
			b.position.y <= o.position.y + o.size.y && b.position.y >= o.position.y &&
			b.position.z <= o.h + o.size.z && b.position.z >= o.h) {
			return true;
		}
	}

	return false;
}

bool check_if_bullet_collided_with_zombie(bullet b, platform_window* window, bool kill_if_collided) {
	map_info info = get_map_info(window);
	float size = get_bullet_size_in_tile(window);

	for (int i = 0; i < max_zombies; i++) {
		zombie o = zombies[i];
		if (!o.alive) continue;

		vec2f bstart = (vec2f){b.position.x, b.position.y};
		vec2f bend = (vec2f){b.endx, b.endy};

		if (b.position.z <= o.position.z + o.size.z && b.position.z >= o.position.z) {
			if (lines_intersect(bstart, bend, (vec2f){o.position.x, o.position.y+o.size.y}, (vec2f){o.position.x+o.size.x, o.position.y+o.size.y}) || // bottom
				lines_intersect(bstart, bend, (vec2f){o.position.x, o.position.y}, (vec2f){o.position.x+o.size.x, o.position.y}) || // top
				lines_intersect(bstart, bend, (vec2f){o.position.x, o.position.y}, (vec2f){o.position.x, o.position.y+o.size.y}) || // left
				lines_intersect(bstart, bend, (vec2f){o.position.x+o.size.x, o.position.y}, (vec2f){o.position.x+o.size.x, o.position.y+o.size.y})) // right
			{
				if (kill_if_collided) {
					zombies[i].alive = false;
				}
				return true;
			}		
		}
	}

	return false;
}

void draw_bullets(platform_window* window) {
	float size = get_bullet_size(window);
	map_info info = get_map_info(window);

	for (int i = 0; i < max_bullets; i++) {
		bullet b = bullets[i];
		if (!b.active) continue;

		bullets[i].alive_time += update_delta;
		if (bullets[i].alive_time > 0.03f) bullets[i].active = false;

		if (is_in_bounds(window, b.position.x, b.position.y)) {
			tile t = get_tile_under_coords(window, b.position.x, b.position.y);

			float h = get_height_of_tile_under_coords(window, bullets[i].position.x, bullets[i].position.y);
			if (h >= b.position.z) {
				bullets[i].active = false; // hit the ground.
			}
		}
		else {
			bullets[i].active = false;
		}

		//if (check_if_bullet_collided_with_object(b, window)) {
		//	bullets[i].active = false;
		//}
		if (check_if_bullet_collided_with_zombie(b, window, true)) {
			bullets[i].active = false;
		}	
		if (!b.active) continue;

		float bullet_render_x = b.position.x*info.tile_width + (b.position.y*info.px_incline);
		float bullet_render_y = b.position.y*info.tile_height - (b.position.z*info.px_raised_per_h);

		float bullet_render_x_end = b.endx*info.tile_width + (b.endy*info.px_incline);
		float bullet_render_y_end = b.endy*info.tile_height - (b.position.z*info.px_raised_per_h);

		renderer->render_line(bullet_render_x, bullet_render_y, bullet_render_x_end, bullet_render_y_end, 2, rgb(0,255,100));
		//renderer->render_rectangle(bullet_render_x - (size/2), bullet_render_y - (size/2), size, size, rgb(0,50,220));
	}
}