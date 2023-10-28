#include "../include/audio.h"

void play_sound(int channel, Mix_Chunk* wav) {
	Mix_PlayChannel(channel, wav, 0);
}

void play_positioned_sound(int channel, Mix_Chunk* wav, vec3f pos, float max_audible_dist) {
	player* p = get_player_by_id(player_id);
	if (!p) return;

	// calculate volume
	int tiles_between_throwable_and_player = distance_between_3f((vec3f){.x = p->playerx, .y = p->playery, .z = p->height}, pos);
	float volume = (tiles_between_throwable_and_player / max_audible_dist);
	if (volume > 1.0f) volume = 1.0f;

	// calculate angle
	/*
	float dirx = (throwables[i].position.x - p->playerx);
	float diry = (throwables[i].position.y - p->playery);
	float rads = atan2(diry, dirx) * 180.0f/M_PI;
	if (rads < 0) rads = 360 + rads;
	rads += 90;
	if (rads > 360) rads -= 360;
	*/

	Mix_SetPosition(0, 0, volume*255);
	Mix_PlayChannel(channel, wav, 0);
}
