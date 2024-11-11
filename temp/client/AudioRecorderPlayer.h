#ifndef AUDIORECORDERPLAYER_H
#define AUDIORECORDERPLAYER_H

#include "GlobalVariables.h"
#include <Audio.h>


#define audioLevel -160

extern AudioClass *theAudio;
extern bool ErrEnd;


/* Recording time[second] */
static const int32_t recoding_time = 4;

/* Recording bit rate
 * Set in bps.
 * Note: 96kbps fixed
 */
static const int32_t recoding_bitrate = 96000;

/* Bytes per second */
static const int32_t recoding_byte_per_second = (recoding_bitrate / 8);

/* Total recording size */
static const int32_t recoding_size = recoding_byte_per_second * recoding_time;

void audio_attention_cb(const ErrorAttentionParam *atprm);
void initialize_audio();
void rec_audio_mp3(const char* recFileName);
void play_audio_mp3(const char* recFileName);

#endif /* AUDIORECORDERPLAYER_H */
