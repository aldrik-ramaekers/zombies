#include "../include/bullets.h"

void shoot(platform_window* window, u32 id, float dirx, float diry) {
	player* p = get_player_by_id(id);
	if (!p) {
		log_info("User with unknown id shot");
	}
	gun g = get_gun_by_type(p->guntype);
	float time_between_bullets = 1.0f/g.shots_per_second;

	if (p->sec_since_last_shot < time_between_bullets) {
		return;
	}
	p->sec_since_last_shot = 0.0f;

	int bullets_to_shoot = g.bullets_per_shot;
	if (bullets_to_shoot > p->ammo_in_mag) bullets_to_shoot = p->ammo_in_mag;
	p->ammo_in_mag -= bullets_to_shoot;
	if (p->ammo_in_mag == 0) {
		int amount_to_reload = g.magazine_size;
		if (amount_to_reload > p->total_ammo) amount_to_reload = p->total_ammo;
		p->total_ammo -= amount_to_reload;
		p->ammo_in_mag = amount_to_reload;
	}

	for (int i = 0; i < bullets_to_shoot; i++)
	{
		map_info info = get_map_info(window);
		float bullet_range = 100.0f;

		float hh = get_height_of_tile_under_coords(window, p->playerx, p->playery);
		dirx += ((float)rand()/(float)(RAND_MAX/g.bullet_spread)-(g.bullet_spread/2));
		diry += ((float)rand()/(float)(RAND_MAX/g.bullet_spread)-(g.bullet_spread/2));

		float bulletx = p->gunx;
		float bullety = p->guny;
		float bullet_end_point_x = bulletx+dirx*bullet_range;
		float bullet_end_point_y = bullety+diry*bullet_range;

		for (int i = 0; i < max_bullets; i++) {
			bullet b = bullets[i];
			if (b.active) continue;

			bullets[i] = (bullet){p->id, true, bulletx, bullety, hh + 0.5, bullet_end_point_x, bullet_end_point_y};
			break;
		}
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

static bool check_if_bullet_collided_with_ground(bullet *b, platform_window* window) {
	map_info info = get_map_info(window);
	float dirx = (b->endx - b->position.x);
	float diry = (b->endy - b->position.y);
	float length = sqrt(dirx * dirx + diry * diry);
	dirx /= length;
	diry /= length;
	double nr_tiles_to_check = length*4; // check 4 points per tile.

	for (int i = 1; i < nr_tiles_to_check; i++) {
		float xtocheck = b->position.x + (dirx*i/4);
		float ytocheck = b->position.y + (diry*i/4);
		if (!is_in_bounds(xtocheck, ytocheck)) break;
		tile tile = get_tile_under_coords(window, xtocheck, ytocheck);

		float h = get_height_of_tile_under_coords(window, xtocheck, ytocheck);
		if (b->position.z <= h) {
			b->endx = xtocheck;
			b->endy = ytocheck;
			return true;
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
		player *p = get_player_by_id(b.player_id);
		bullets[i].position.x = p->gunx;
		bullets[i].position.y = p->guny;
		bullets[i].position.z = p->gun_height;
		
		if (check_if_bullet_collided_with_ground(&b, window)) {
			bullets[i].endy = b.endy;
			bullets[i].endx = b.endx;
			b = bullets[i];
		}

		if (check_if_bullet_collided_with_object(&b, window)) {
			bullets[i].endy = b.endy;
			bullets[i].endx = b.endx;
			b = bullets[i];
		}
		
		if (check_if_bullet_collided_with_zombie(b, window, true)) {
			bullets[i].active = false;
		}

		bullets[i].alive_time += update_delta;
		bullets[i].active = false;

		if ((int)bullets[i].position.y < (int)bullets[i].endy) { BULLET_RENDER_DEPTH((int)bullets[i].position.y); }
		else { BULLET_RENDER_DEPTH((int)bullets[i].endy); }

		float bullet_render_x = b.position.x*info.tile_width + (b.position.y*info.px_incline);
		float bullet_render_y = b.position.y*info.tile_height - (b.position.z*info.px_raised_per_h);

		float bullet_render_x_end = b.endx*info.tile_width + (b.endy*info.px_incline);
		float bullet_render_y_end = b.endy*info.tile_height - (b.position.z*info.px_raised_per_h);

		renderer->render_line(bullet_render_x, bullet_render_y, bullet_render_x_end, bullet_render_y_end, 2, rgb(0,255,100));
	}
}