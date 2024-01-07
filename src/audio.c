#include "../include/audio.h"

void add_throwable_audio_event_to_queue(audio_event_type event, throwable_type throwable, u32 playerid, vec3f position) {
	for (int i = 0; i < max_audio_events; i++) {
		if (audio_events[i].active) continue;

		audio_events[i] = (audio_event){.active = true, .throwable = throwable, .obj = OBJECT_NONE, .zombie = ZOMBIE_TYPE_NONE, .playerid = playerid, .position = position, .type = event};
		return;
	}
	log_info("Audio queue full");
}

void add_zombie_audio_event_to_queue(audio_event_type event, zombie_type zombie, vec3f position) {
	for (int i = 0; i < max_audio_events; i++) {
		if (audio_events[i].active) continue;

		audio_events[i] = (audio_event){.active = true, .throwable = THROWABLE_NONE, .obj = OBJECT_NONE, .zombie = zombie, .playerid = -1, .position = position, .type = event};
		return;
	}
	log_info("Audio queue full");
}

void add_object_audio_event_to_queue(audio_event_type event, object_type obj, u32 playerid, vec3f position) {
	for (int i = 0; i < max_audio_events; i++) {
		if (audio_events[i].active) continue;

		audio_events[i] = (audio_event){.active = true, .throwable = THROWABLE_NONE, .obj = obj, .zombie = ZOMBIE_TYPE_NONE, .playerid = playerid, .position = position, .type = event};
		return;
	}
	log_info("Audio queue full");
}

void add_audio_event_to_queue(audio_event_type event, u32 playerid, vec3f position) {
	add_object_audio_event_to_queue(event, OBJECT_NONE, playerid, position);
}

void add_ui_audio_event_to_queue(audio_event_type event) {
	add_object_audio_event_to_queue(event, OBJECT_NONE, -1, global_audio_source_position);
}

static Mix_Chunk* get_sample_from_audio_event(audio_event event, u32 playerid) {
	player* p = get_player_by_id(playerid);

	switch (event.type)
	{
		case EVENT_FOOTSTEP: {
			if (event.zombie != ZOMBIE_TYPE_NONE) {
				switch(event.zombie) {
					case ZOMBIE_TYPE_ENRAGED: return wav_step_enraged;
					default: return wav_error;
				}		
			}
			else {
				return wav_step;
			}
		} 
		case EVENT_CHARACTER_TYPE: return wav_character;
		case EVENT_ROUND_CHANGE: return wav_round_change;
		case EVENT_COLLECT: return wav_collect;
		case EVENT_BOUNCE_THROWABLE: return wav_throwable_bounce;
		case EVENT_FIRE: return wav_fire;
		case EVENT_EXPLODE_THROWABLE: {
			switch(event.throwable) {
				case THROWABLE_GRENADE: return wav_grenade_explode;
				case THROWABLE_MOLOTOV: return wav_molotov_explode;
				default: return wav_error;
			}
		}
		case EVENT_SHOOT: {
			if (!p) { log_infox("Unknown player audio event %d", playerid); return wav_error; }
			switch (p->guntype)
			{
				case GUN_MP5: return wav_shoot_mp5;
				case GUN_NOVA: return wav_shoot_nova;
				default: return wav_error;
			}
		}
		case EVENT_RELOAD: {
			switch (p->guntype)
			{
				case GUN_MP5: return wav_reload_mp5;
				case GUN_NOVA: return wav_reload_nova;
				default: return wav_error;
			}
		}

		case EVENT_ZOMBIEROAR: {
			switch (event.zombie)
			{
				case ZOMBIE_TYPE_ENRAGED: return wav_roar_enraged;
				default: return wav_error;
			}
		}
		case EVENT_IMPACT: {
			if (event.zombie != ZOMBIE_TYPE_NONE) {
				switch (event.zombie)
				{
					case ZOMBIE_TYPE_NORMAL: return wav_impact_zombie;
					default: return wav_error;
				}
			}
			else if (event.obj != OBJECT_NONE)  {
				switch (event.obj)
				{
					case OBJECT_PLANTBOX1: return wav_impact_wood;
					case OBJECT_METAL_WALL:
					case OBJECT_METAL_WALL2: return wav_impact_metal;
					default: return wav_error;
				}
			}
		}
		case EVENT_ZOMBIESCREECH: {
			int random_screech_index = rand() % NUM_SCREECHES;
			return wav_screech[random_screech_index];
		}
		case EVENT_PLAYERHURT: {
			int random_hurt_index = rand() % NUM_PLAYER_HURT;
			return wav_player_hurt[random_hurt_index];
			return wav_error;
		}

		default: return wav_error;
	}
}

void play_sounds_in_queue() {
	for (int i = 0; i < max_audio_events; i++) {
		if (!audio_events[i].active) continue;

		Mix_Chunk* sample = get_sample_from_audio_event(audio_events[i], audio_events[i].playerid);
		/*if (sample == wav_error) {
			log_infox("Missing sample for type: %d | zombie: %d | object: %d | throwable: %d", 
			audio_events[i].type, audio_events[i].obj, audio_events[i].zombie, audio_events[i].throwable);
		}*/

		play_positioned_sound(0, sample, audio_events[i].position, 20);
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

void play_music(Mix_Music* music) {
	Mix_FadeInMusic(music, -1, 2000);
	Mix_VolumeMusic(MIX_MAX_VOLUME/6);
}

void play_positioned_sound(int channel, Mix_Chunk* wav, vec3f pos, float max_audible_dist) {
	player* p = get_player_by_id(player_id);
	if (!p) return;

	// calculate volume
	int tiles_between_throwable_and_player = distance_between_3f((vec3f){.x = p->playerx, .y = p->playery, .z = p->height}, pos);
	if (vec3f_equals(pos, global_audio_source_position)) tiles_between_throwable_and_player = 0; // Global audio source.

	float volume = (tiles_between_throwable_and_player / max_audible_dist);
	if (volume > 1.0f) volume = 1.0f;
	if (volume < 0.0f) volume = 0.0f;

	// calculate angle
	/*
	float dirx = (throwables[i].position.x - p->playerx);
	float diry = (throwables[i].position.y - p->playery);
	float rads = atan2(diry, dirx) * 180.0f/M_PI;
	if (rads < 0) rads = 360 + rads;
	rads += 90;
	if (rads > 360) rads -= 360;
	*/

	int c = Mix_PlayChannel(-1, wav, 0);
	if (c == -1) {
		log_info("Audio not playable because no channels are free");
	}
	else {
		Mix_SetPosition(c, 0, (int)(volume*255));
	}
}
