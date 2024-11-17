#ifndef _AUDIOAI_H_
#define _AUDIOAI_H_

#include <Arduino.h>
#include <Audio.h>
#include <FFT.h>
#include <SDHCI.h>
#include <float.h>
#include <DNNRT.h>

#define FFT_LEN 512

extern FFTClass<AS_CHANNEL_STEREO, FFT_LEN> FFT;
extern AudioClass* theAudio;
extern DNNRT dnnrt;

extern bool isProcessing;

void initAudioAI();
int runAudioAI(float valueThreshold);
void averageSmooth(float* dst);

#endif //_AUDIOAI_H_