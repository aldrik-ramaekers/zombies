#include "../include/bullets.h"

void shoot(platform_window* window, u32 id, float dirx, float diry) {
	player* p = get_player_by_id(id);
	if (!p) {
		log_info("User with unknown id shot");
	}
	if (p->interact_state != INTERACT_IDLE) {
		return;
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

	if (bullets_to_shoot > 0) {
		add_audio_event_to_queue(EVENT_SHOOT, p->id, (vec3f){.x = p->playerx, .y = p->playery, .z = p->height});
	}
	
	for (int i = 0; i < bullets_to_shoot; i++)
	{
		map_info info = get_map_info(window);
		float bullet_range = 100.0f;

		float hh = get_height_of_tile_under_coords(p->playerx, p->playery);
		float rads = atan2(dirx, diry);
		float target1 = rads - (g.bullet_spread/2.0f);
		float target2 = rads + (g.bullet_spread/2.0f);

		float target_rand = 0.0f;
		{
			float random = ((float) rand()) / (float) RAND_MAX;
			float diff = target2 - target1;
			float r = random * diff;
			target_rand = target1 + r;
		}

		float bulletx = p->gunx;
		float bullety = p->guny;
		float bullet_end_point_x = bulletx+(sin(target_rand))*bullet_range;
		float bullet_end_point_y = bullety+(cos(target_rand))*bullet_range;

		for (int i = 0; i < max_bullets; i++) {
			bullet b = bullets[i];
			if (b.active) continue;
			bullets[i] = (bullet){p->id, true, bulletx, bullety, hh + 0.5,.endx = bullet_end_point_x,.endy = bullet_end_point_y, .damage = g.damage};
			break;
		}
	}

	if (p->ammo_in_mag == 0) {
		add_audio_event_to_queue(EVENT_RELOAD, p->id, (vec3f){.x = p->playerx, .y = p->playery, .z = p->height});
		p->interact_state = INTERACT_RELOADING;
		p->sec_since_interact_state_change = 0;
		return;
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

object_type check_if_bullet_collided_with_object(bullet* b, platform_window* window) {
	map_info info = get_map_info(window);
	float size = get_bullet_size_in_tile(window);
	
	vec2f bstart = (vec2f){b->position.x, b->position.y};
	vec2f bend = (vec2f){b->endx, b->endy};

	object_type result = OBJECT_NONE;
	float dist_of_closest_intersect = __FLT_MAX__;

	for (int i = 0; i < MAX_OBJECTS; i++) {
		object o = loaded_map.objects[i];
		if (!o.active) continue;
		if (b->position.z <= o.position.z + o.size.z && b->position.z >= o.position.z) {
			box obj_box = get_box_of_square((vec3f){o.position.x, o.position.y, o.position.z}, o.size);
			vec2f intersect_point;
			if (check_if_bullet_collided_with_section(&dist_of_closest_intersect, bstart, bend, obj_box.bl_d, obj_box.br_d, &intersect_point)) {
				result = o.type;
			}
			if (check_if_bullet_collided_with_section(&dist_of_closest_intersect, bstart, bend, obj_box.tl_d, obj_box.tr_d, &intersect_point)) {
				result = o.type;
			}
			if (check_if_bullet_collided_with_section(&dist_of_closest_intersect, bstart, bend, obj_box.tl_d, obj_box.bl_d, &intersect_point)) {
				result = o.type;
			}
			if (check_if_bullet_collided_with_section(&dist_of_closest_intersect, bstart, bend, obj_box.tr_d, obj_box.br_d, &intersect_point)) {
				result = o.type;
			}

			if (result != OBJECT_NONE) {
				b->endy = intersect_point.y;
				b->endx = intersect_point.x;
			}
		}
	}

	return result;
}

bool check_if_bullet_collided_with_zombie(bullet* b, platform_window* window, player *p) {
	if (b->damage == 0) return false;
	
	map_info info = get_map_info(window);
	float size = get_bullet_size_in_tile(window);

	bool result = false;
	float dist_of_closest_intersect = __FLT_MAX__;
	int index_of_closest_zombie = -1;
	vec2f intersect_point_of_closest_zombie;
	
	for (int i = 0; i < SERVER_MAX_ZOMBIES; i++) {
		zombie o = zombies[i];
		if (!o.alive) continue;

		vec2f bstart = (vec2f){b->position.x, b->position.y};
		vec2f bend = (vec2f){b->endx, b->endy};

		if (b->position.z <= o.position.z + o.size.z && b->position.z >= o.position.z) {
			vec2f intersect_point;
			box obj_box = get_box_of_square((vec3f){o.position.x, o.position.y, o.position.z}, o.size);
			bool this_zombie_collided = false;
			if (check_if_bullet_collided_with_section(&dist_of_closest_intersect, bstart, bend, obj_box.bl_d, obj_box.br_d, &intersect_point)) {
				this_zombie_collided = true;
				index_of_closest_zombie = i;
			}
			if (check_if_bullet_collided_with_section(&dist_of_closest_intersect, bstart, bend, obj_box.tl_d, obj_box.tr_d, &intersect_point)) {
				this_zombie_collided = true;
				index_of_closest_zombie = i;
			}
			if (check_if_bullet_collided_with_section(&dist_of_closest_intersect, bstart, bend, obj_box.tl_d, obj_box.bl_d, &intersect_point)) {
				this_zombie_collided = true;
				index_of_closest_zombie = i;
			}
			if (check_if_bullet_collided_with_section(&dist_of_closest_intersect, bstart, bend, obj_box.tr_d, obj_box.br_d, &intersect_point)) {
				this_zombie_collided = true;
				index_of_closest_zombie = i;
			}

			if (this_zombie_collided) {
				result = true;
				intersect_point_of_closest_zombie = intersect_point;
			}
		}
	}

	if (result) {
		b->endy = intersect_point_of_closest_zombie.y;
		b->endx = intersect_point_of_closest_zombie.x;
		
		if (hit_zombie(index_of_closest_zombie, b->damage)) {
			p->kills++;
		}
		b->damage = 0;
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
		tile tile = get_tile_under_coords(xtocheck, ytocheck);

		float h = get_height_of_tile_under_coords(xtocheck, ytocheck);
		if (b->position.z <= h) {
			b->endx = xtocheck;
			b->endy = ytocheck;
			return true;
		}
	}
	return false;
}

void clear_bullets() {
	for (int i = 0; i < max_bullets; i++) {
		if (!bullets[i].active) continue;
		bullets[i].alive_time += update_delta;
		if (bullets[i].alive_time >= 0.05f) {
			bullets[i].active = false;
		}
	}
}

void update_bullets_server(platform_window* window) {
	for (int i = 0; i < max_bullets; i++) {
		bullet b = bullets[i];
		if (!b.active) continue;
		player *p = get_player_by_id(b.player_id);
		if (!p) continue; // Bullet from inactive player?
		bullets[i].position.x = p->gunx;
		bullets[i].position.y = p->guny;
		bullets[i].position.z = p->gun_height;
		
		if (check_if_bullet_collided_with_ground(&b, window)) {
			bullets[i].endy = b.endy;
			bullets[i].endx = b.endx;
			b = bullets[i];
		}

		object_type obj_collision = check_if_bullet_collided_with_object(&b, window);
		if (obj_collision != OBJECT_NONE) {
			bullets[i].endy = b.endy;
			bullets[i].endx = b.endx;
			b = bullets[i];
			
			add_object_audio_event_to_queue(EVENT_IMPACT, obj_collision, p->id, (vec3f){.x = p->playerx, .y = p->playery, .z = p->height});
		}
		
		if (check_if_bullet_collided_with_zombie(&b, window, p)) {
			bullets[i].endy = b.endy;
			bullets[i].endx = b.endx;
			bullets[i].damage = b.damage;
			b = bullets[i];

			add_points_to_player(p, POINTS_PER_HIT);
			add_zombie_audio_event_to_queue(EVENT_IMPACT, ZOMBIE_TYPE_NORMAL, (vec3f){.x = p->playerx, .y = p->playery, .z = p->height});
		}
	}
}

void draw_bullets(platform_window* window) {
	float size = get_bullet_size(window);
	map_info info = get_map_info(window);

	for (int i = 0; i < max_bullets; i++) {
		bullet b = bullets[i];
		if (!b.active) continue;

		if (b.player_id == player_id) {
			player *p = get_player_by_id(b.player_id);
			b.position.x = p->gunx;
			b.position.y = p->guny;
			b.position.z = p->gun_height;
		}

		BULLET_RENDER_DEPTH((int)bullets[i].position.y);

		float bullet_render_x = b.position.x*info.tile_width + (b.position.y*info.px_incline);
		float bullet_render_y = b.position.y*info.tile_height - (b.position.z*info.px_raised_per_h);

		float bullet_render_x_end = b.endx*info.tile_width + (b.endy*info.px_incline);
		float bullet_render_y_end = b.endy*info.tile_height - (b.position.z*info.px_raised_per_h);

		renderer->render_line(bullet_render_x, bullet_render_y, bullet_render_x_end, bullet_render_y_end, 1, rgba(255, 221, 97, 100));
	}
}