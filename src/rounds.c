
bool current_round_is_done()
{
	return _current_round.zombies == 0 && number_of_zombies_active() == 0;
}

void start_next_round()
{
	_current_round.round_nr++;
	_current_round.zombies = 6;
	_current_round.state = ROUND_SWITCHING;
	_current_round.round_timer = 0.0f;

	log_infox("Next round: %d", _current_round.round_nr);
}

int zombies_left_in_current_round()
{
	if (_current_round.state != ROUND_SPAWNING) return 0;
	return _current_round.zombies;
}

void update_round_server()
{
	_current_round.round_timer += SERVER_TICK_RATE;
	if (_current_round.state == ROUND_SWITCHING) {
		if (_current_round.round_timer >= ROUND_SWITCH_TIME) {
			_current_round.state = ROUND_SPAWNING;
		}
	}

	if (current_round_is_done()) {
		start_next_round();
	}
}