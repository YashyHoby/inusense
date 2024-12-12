#ifndef AUDIORECORDERPLAYER_H
#define AUDIORECORDERPLAYER_H

#include "GlobalVariables.h"
//#include <Audio.h>


#define audioLevel -160

//extern AudioClass *theAudio;
extern bool ErrEnd;

void audio_attention_cb(const ErrorAttentionParam *atprm);
void initialize_audio();
void start_recorder(const char* recFileName);
void start_player(const char* recFileName);
void close_recorder(File myfile);
void close_player(File myfile);

#endif /* AUDIORECORDERPLAYER_H */
