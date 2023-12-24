
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

		text_w = renderer->calculate_text_width(fnt_32, round_text);
		final_text_y = _global_camera.y + window->height/4.0f;
		int box_pad = 10;
		int box_x = window_center_x - (text_w/2)+1 - box_pad;
		int box_y = final_text_y - box_pad;
		int box_w = text_w + box_pad*2;
		int box_h = fnt_32->px_h + box_pad*2;
		renderer->render_rectangle(box_x, box_y, box_w, box_h, rgba(255,0,0,100));
		renderer->render_text(fnt_32, window_center_x - (text_w/2), final_text_y, round_text, rgba(255,255,255,255));
	}
}

void update_round_server()
{
	static int visible_previously_count = 0;

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