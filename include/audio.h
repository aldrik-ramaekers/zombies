#ifndef INCLUDE_AUDIO_
#define INCLUDE_AUDIO_

#include <projectbase/project_base.h>

#define CHANNEL_THROWABLES 0
#define CHANNEL_SHOOTING 1

typedef enum t_audio_event_type {
	EVENT_SHOOT,
	EVENT_RELOAD,
	EVENT_BOUNCE_THROWABLE,
} audio_event_type;

typedef struct t_audio_event {
	bool active;
	audio_event_type type;
	u32 playerid;
	vec3f position;
} audio_event;

#define MAX_AUDIO_EVENTS 20
audio_event audio_events[MAX_AUDIO_EVENTS] = {0};
int max_audio_events = MAX_AUDIO_EVENTS;

void add_audio_event_to_queue(audio_event_type event, u32 playerid, vec3f position);
void play_sounds_in_queue();
void clear_sounds_in_queue();
void play_sound(int channel, Mix_Chunk* wav);
void play_positioned_sound(int channel, Mix_Chunk* wav, vec3f pos, float max_audible_dist);

#endif