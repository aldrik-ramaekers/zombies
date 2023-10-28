#ifndef INCLUDE_AUDIO_
#define INCLUDE_AUDIO_

#include <projectbase/project_base.h>

#define CHANNEL_THROWABLES 0

void play_sound(int channel, Mix_Chunk* wav);
void play_positioned_sound(int channel, Mix_Chunk* wav, vec3f pos, float max_audible_dist);

#endif