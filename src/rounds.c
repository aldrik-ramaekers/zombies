
bool current_round_is_done()
{
	return _current_round.zombies == 0 && number_of_zombies_active() == 0;
}

static u32 get_number_of_zombies_in_round(u32 round_nr)
{
	// 10 = 25
	// 20 = 66
	// 50 = 250
	// 100 = 640
	// 200 = 1670
	return 8 + pow(round_nr, 1.4f);
}

void start_next_round()
{
	_current_round.round_nr++;
	_current_round.zombies = get_number_of_zombies_in_round(_current_round.round_nr);
	_current_round.state = ROUND_SWITCHING;
	_current_round.round_timer = 0.0f;

	// Fade in at start of game.
	if (_current_round.round_nr == 1) {
		_current_round.fade_in_timer = FADE_IN_DURATION;
		_current_round.fade_out_timer = 0.0f;
	}

	add_ui_audio_event_to_queue(EVENT_ROUND_CHANGE);

	log_infox("Next round: %d", _current_round.round_nr);
}

int zombies_left_in_current_round()
{
	if (_current_round.state != ROUND_SPAWNING) return 0;
	return _current_round.zombies;
}

static float get_round_text_opacity() {
	float opacity = 2.0f - ((_current_round.round_timer / ROUND_SWITCH_TIME)*2);
	if (opacity > 1.0f) opacity = 1.0f;
	return opacity;
}

static int round_text_len = 0;
void draw_round(platform_window* window) {
	return;
	if (_current_round.round_nr == 0) return;

	char round_text[30];
	int window_center_x = _global_camera.x + window->width / 2;	
	sprintf(round_text, "ROUND %d", _current_round.round_nr);
	round_text_len = strlen(round_text);
	int text_w = renderer->calculate_text_width(fnt_24, round_text);
	int final_text_y = _global_camera.y + 20;

	if (_current_round.state == ROUND_SPAWNING) {
		renderer->render_text(fnt_24, window_center_x - (text_w/2)+1, final_text_y+1, round_text, rgba(0,0,0,120));
		renderer->render_text(fnt_24, window_center_x - (text_w/2), final_text_y, round_text, rgb(189, 39, 19));

		char time_text[30];
		sprintf(time_text, "%.0fs", _current_round.round_timer);
		if (_current_round.round_timer >= 60) {
			int min = (int)(_current_round.round_timer/60.0f);
			int sec = (int)((int)_current_round.round_timer%60);
			sprintf(time_text, "%dm %ds", min, sec);
		}
		int time_text_w = renderer->calculate_text_width(fnt_20, time_text);
		final_text_y = _global_camera.y + 24 + fnt_24->px_h;
		renderer->render_text(fnt_20, window_center_x - (time_text_w/2)+1, final_text_y+1, time_text, rgba(0,0,0,120));
		renderer->render_text(fnt_20, window_center_x - (time_text_w/2), final_text_y, time_text, rgb(189, 39, 19));
	}
	else if (_current_round.state == ROUND_SWITCHING) {
		float delay_per_char = 0.1f;
		int characters_visible_count = _current_round.round_timer/delay_per_char;
		if (characters_visible_count > round_text_len) characters_visible_count = round_text_len;
		round_text[characters_visible_count] = 0;
		font* round_text_fnt = get_font(window, 2.0f);

		text_w = renderer->calculate_text_width(round_text_fnt, round_text);
		final_text_y = _global_camera.y + window->height/4.0f;
		int box_pad = 15;
		int box_x = window_center_x - (text_w/2)+1 - box_pad;
		int box_y = final_text_y - box_pad;
		int box_w = text_w + box_pad*2;
		int box_h = round_text_fnt->px_h + box_pad*2;
		renderer->render_rectangle(box_x, box_y, box_w, box_h, rgba(255,0,0,100));
		renderer->render_text(round_text_fnt, window_center_x - (text_w/2), final_text_y, round_text, rgba(255,255,255,255));
	}
}

void restart_game()
{
	// Reset to round 1.
	_current_round.round_nr = 0;
	start_next_round();

	clear_zombies();

	// Respawn players.
	for (int i = 0; i < MAX_PLAYERS; i++) {
		if (!players[i].active) continue;
		players[i].sec_since_last_shot = 10.0f;
		players[i].sec_since_last_damage_taken = 10.0f;
		players[i].playerx = 40;
		players[i].playery = 3;
		players[i].move_state = PLAYER_MOVE_STATE_IDLE;
		players[i].interact_state = INTERACT_IDLE;
		players[i].guntype = GUN_MP5;
		players[i].height = 0.0f;
		players[i].health = 500;
		players[i].max_health = 500;
		players[i].kills = 0;
		players[i].direction = DIRECTION_DOWN;
		players[i].throwables.grenades = 3; 
		players[i].throwables.molotovs = 1;
		players[i].points = 800;
		players[i].sec_since_last_step = 0.0f;

		gun g = get_gun_by_type(players[i].guntype);
		players[i].total_ammo = g.max_ammunition;
		players[i].ammo_in_mag = g.magazine_size;
	}
}

void update_round_server()
{
	static int visible_previously_count = 0;

	if (!every_player_died()) {
		_current_round.fade_in_timer -= SERVER_TICK_RATE;
		if (_current_round.fade_in_timer <= 0.0f)
			_current_round.fade_in_timer = 0.0f;
	}
	else {
		_current_round.fade_out_timer += SERVER_TICK_RATE;
		if (_current_round.fade_out_timer >= FADE_OUT_DELAY_DURATION) {
			_current_round.fade_in_timer += SERVER_TICK_RATE;
			
			if (_current_round.fade_in_timer >= FADE_IN_DURATION) {
				_current_round.fade_in_timer = FADE_IN_DURATION;
				restart_game();
			}
		}
	}

	_current_round.round_timer += SERVER_TICK_RATE;
	if (_current_round.state == ROUND_SWITCHING) {
		if (_current_round.round_timer >= ROUND_SWITCH_TIME) {
			_current_round.state = ROUND_SPAWNING;
			_current_round.round_timer = 0.0f;	
			visible_previously_count = 0;	
		}

		float delay_per_char = 0.1f;
		int characters_visible_count = _current_round.round_timer/delay_per_char;
		if (characters_visible_count > round_text_len) characters_visible_count = round_text_len;
		if (visible_previously_count < characters_visible_count) {
			add_ui_audio_event_to_queue(EVENT_CHARACTER_TYPE);
			visible_previously_count = characters_visible_count;
		}
	}

	if (current_round_is_done()) {
		start_next_round();
	}
}