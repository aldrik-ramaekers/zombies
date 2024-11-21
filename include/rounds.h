#ifndef INCLUDE_ROUNDS
#define INCLUDE_ROUNDS

#include "../project-base/src/project_base.h"

#define ROUND_SWITCH_TIME 6.0f

typedef enum t_round_state {
	ROUND_SPAWNING,
	ROUND_SWITCHING,
} round_state;

#define FADE_IN_DURATION 3.0f
#define FADE_OUT_DELAY_DURATION 2.0f

typedef struct t_zombie_round {
	u32 round_nr;
	u32 zombies;
	round_state state;
	float round_timer;
	float fade_in_timer;
	float fade_out_timer;
} zombie_round;

zombie_round _current_round = {.round_nr = 0, .zombies = 0, .state = ROUND_SWITCHING, FADE_IN_DURATION, FADE_OUT_DELAY_DURATION};

bool current_round_is_done();
void start_next_round();
int zombies_left_in_current_round();
void update_round_server();
void draw_round();

#endif