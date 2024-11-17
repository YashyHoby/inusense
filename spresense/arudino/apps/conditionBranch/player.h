#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <SDHCI.h>
#include <Audio.h>
// #include <unistd.h>

#define audioLevel -320

extern AudioClass* theAudio;
extern SDClass SD;

extern bool isProcessing;

void play_audio_mp3(const char* fileName);

#endif