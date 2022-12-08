#include "include/bullets.h"

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

		bullets[i] = (bullet){true, bulletx, bullety, hh + 0.5, bullet_end_point_x, bullet_end_point_y};
		break;
	}
}

bool check_if_bullet_collided_with_section(float* dist_of_closest_intersect, vec2f bstart, vec2f bend, vec2f l1, vec2f l2, vec2f* intersect_point_buf) {
	if (lines_intersect(bstart, bend, l1, l2)) {
		vec2f intersect_point = get_intersection_point(bstart, bend, l1, l2);

		float dirx = (bstart.x - intersect_point.x);
		float diry = (bstart.y - intersect_point.y);
		double length_of_shot = sqrt(dirx * dirx + diry * diry);

		if (length_of_shot > *dist_of_closest_intersect) {
			return false;
		}
		*dist_of_closest_intersect = length_of_shot;
		*intersect_point_buf = intersect_point;
		return true;
	}
	return false;
}

bool check_if_bullet_collided_with_object(bullet* b, platform_window* window) {
	map_info info = get_map_info(window);
	float size = get_bullet_size_in_tile(window);
	
	vec2f bstart = (vec2f){b->position.x, b->position.y};
	vec2f bend = (vec2f){b->endx, b->endy};

	bool result = false;
	float dist_of_closest_intersect = __FLT_MAX__;

	for (int i = 0; i < max_objects; i++) {
		object o = objects[i];
		if (!o.active) continue;
		if (b->position.z <= o.h + o.size.z && b->position.z >= o.h) {
			box obj_box = get_box_of_square(window, (vec3f){o.position.x, o.position.y, o.h}, o.size);
			vec2f intersect_point;
			if (check_if_bullet_collided_with_section(&dist_of_closest_intersect, bstart, bend, obj_box.bl_b, obj_box.br_b, &intersect_point)) {
				result = true;
			}
			if (check_if_bullet_collided_with_section(&dist_of_closest_intersect, bstart, bend, obj_box.tl_b, obj_box.tr_b, &intersect_point)) {
				result = true;
			}
			if (check_if_bullet_collided_with_section(&dist_of_closest_intersect, bstart, bend, obj_box.tl_b, obj_box.bl_b, &intersect_point)) {
				result = true;
			}
			if (check_if_bullet_collided_with_section(&dist_of_closest_intersect, bstart, bend, obj_box.tr_b, obj_box.br_b, &intersect_point)) {
				result = true;
			}

			if (result) {
				b->endy = intersect_point.y;
				b->endx = intersect_point.x;
			}
		}
	}

	return result;
}

bool check_if_bullet_collided_with_zombie(bullet b, platform_window* window, bool kill_if_collided) {
	map_info info = get_map_info(window);
	float size = get_bullet_size_in_tile(window);

	bool result = false;
	float dist_of_closest_intersect = __FLT_MAX__;
	int index_of_closest_zombie = -1;
	
	for (int i = 0; i < max_zombies; i++) {
		zombie o = zombies[i];
		if (!o.alive) continue;

		vec2f bstart = (vec2f){b.position.x, b.position.y};
		vec2f bend = (vec2f){b.endx, b.endy};

		if (b.position.z <= o.position.z + o.size.z && b.position.z >= o.position.z) {
			vec2f intersect_point;
			box obj_box = get_box_of_square(window, (vec3f){o.position.x, o.position.y, o.position.z}, o.size);
			bool this_zombie_collided = false;
			if (check_if_bullet_collided_with_section(&dist_of_closest_intersect, bstart, bend, obj_box.bl_b, obj_box.br_b, &intersect_point)) {
				this_zombie_collided = true;
				index_of_closest_zombie = i;
			}
			if (check_if_bullet_collided_with_section(&dist_of_closest_intersect, bstart, bend, obj_box.tl_b, obj_box.tr_b, &intersect_point)) {
				this_zombie_collided = true;
				index_of_closest_zombie = i;
			}
			if (check_if_bullet_collided_with_section(&dist_of_closest_intersect, bstart, bend, obj_box.tl_b, obj_box.bl_b, &intersect_point)) {
				this_zombie_collided = true;
				index_of_closest_zombie = i;
			}
			if (check_if_bullet_collided_with_section(&dist_of_closest_intersect, bstart, bend, obj_box.tr_b, obj_box.br_b, &intersect_point)) {
				this_zombie_collided = true;
				index_of_closest_zombie = i;
			}

			if (this_zombie_collided) {
				result = true;
			}
		}
	}

	if (kill_if_collided && result) {
		zombies[index_of_closest_zombie].alive = false;
		return result;
	}

	return result;
}

void draw_bullets(platform_window* window) {
	float size = get_bullet_size(window);
	map_info info = get_map_info(window);

	for (int i = 0; i < max_bullets; i++) {
		bullet b = bullets[i];
		if (!b.active) continue;

		/*
		if (is_in_bounds(window, b.position.x, b.position.y)) {
			tile t = get_tile_under_coords(window, b.position.x, b.position.y);

			float h = get_height_of_tile_under_coords(window, bullets[i].position.x, bullets[i].position.y);
			if (h >= b.position.z) {
				bullets[i].active = false; // hit the ground.
			}
		}
		else {
			bullets[i].active = false;
		}*/

		if (check_if_bullet_collided_with_object(&b, window)) {
			bullets[i].endy = b.endy;
			bullets[i].endx = b.endx;
			b = bullets[i];
		}
		
		if (check_if_bullet_collided_with_zombie(b, window, true)) {
			bullets[i].active = false;
		}

		bullets[i].alive_time += update_delta;
		if (bullets[i].alive_time > 0.03f) bullets[i].active = false;

		if (!b.active) continue;

		BULLET_RENDER_DEPTH(b.position.z);

		float bullet_render_x = b.position.x*info.tile_width + (b.position.y*info.px_incline);
		float bullet_render_y = b.position.y*info.tile_height - (b.position.z*info.px_raised_per_h);

		float bullet_render_x_end = b.endx*info.tile_width + (b.endy*info.px_incline);
		float bullet_render_y_end = b.endy*info.tile_height - (b.position.z*info.px_raised_per_h);

		renderer->render_line(bullet_render_x, bullet_render_y, bullet_render_x_end, bullet_render_y_end, 2, rgb(0,255,100));
	}
}