#include "../include/audio.h"

void add_audio_event_to_queue(audio_event_type event, u32 playerid, vec3f position) {
	for (int i = 0; i < max_audio_events; i++) {
		if (audio_events[i].active) continue;

		audio_events[i] = (audio_event){.active = true, .playerid = playerid, .position = position, .type = event};
		return;
	}
}

static int get_channel_from_audio_event_type(audio_event_type event) {
	switch (event)
	{
		case EVENT_BOUNCE_THROWABLE: return CHANNEL_THROWABLES;
		case EVENT_SHOOT: return CHANNEL_SHOOTING;
		case EVENT_RELOAD: return CHANNEL_SHOOTING;

		default: return 0;
	}
}

static Mix_Chunk* get_sample_from_audio_event_type(audio_event_type event, u32 playerid) {
	player* p = get_player_by_id(playerid);
	if (!p) return 0;

	switch (event)
	{
		case EVENT_BOUNCE_THROWABLE: return wav_throwable_bounce;
		case EVENT_SHOOT: {
			switch (p->guntype)
			{
				case GUN_MP5: return wav_shoot_mp5;		
				default:
					break;
			}
		}
		case EVENT_RELOAD: {
			switch (p->guntype)
			{
				case GUN_MP5: return wav_reload_mp5;		
				default:
					break;
			}
		}

		default:
			break;
	}
}

void play_sounds_in_queue() {
	for (int i = 0; i < max_audio_events; i++) {
		if (!audio_events[i].active) continue;

		play_positioned_sound(
			get_channel_from_audio_event_type(audio_events[i].type), 
			get_sample_from_audio_event_type(audio_events[i].type, audio_events[i].playerid), 
			audio_events[i].position, 20);
	}
}

void clear_sounds_in_queue() {
	for (int i = 0; i < max_audio_events; i++) {
		audio_events[i].active = false;
	}
}

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
