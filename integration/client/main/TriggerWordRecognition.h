#ifndef TRIGGERWOEDRECOGNITION
#define TRIGGERWOEDRECOGNITION

#include "GlobalVariables.h"
#include <FFT.h>
#include <float.h>
#include <DNNRT.h>

#define FFT_LEN 512

extern FFTClass<AS_CHANNEL_STEREO, FFT_LEN> FFT;
extern DNNRT dnnrt;

void averageSmooth(float* dst);
int triggerWordRecognition();
void initialization_TWR();
void soundRecognition();

#endif //TRIGGERWOEDRECOGNITION


