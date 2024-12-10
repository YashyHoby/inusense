
#if (SUBCORE != 1)
#error "Core selection is wrong!!"
#endif

#include <Adafruit_NeoPixel_Spresense.h>
#include <MP.h>
#include "animations.h"

#define PIN 6             // D06 pin for neopixel pwm
#define INTERVAL_US 41666 // Timer Interrupt Interval: 41.6ms (24Hz)

#define MEMSIZE 8        // Meridim array maximum setting (8bit)
int16_t *meridian_addr;  // Shared memory address for meridian

uint8_t t = 0;
uint8_t tempAnimation[NUM_COLORS][NUM_FRAMES] = {{0}};  // Temporal matrix of neopixel animation

uint8_t lastAnimeNum = 0;
bool loopOrNotFlg = false;  // loop or non-loop flag (0:non-loop, 1:loop)
bool loopAnimeFlg = false;  // if it's true, non-loop animation stops

Adafruit_NeoPixel_Spresense neopixel(NUM_PIXELS, PIN);

void setup()
{
  /* Initialize neopixel strip */
	neopixel.begin();                 // Initialize neopixel object
  neopixel.show();                  // Turn OFF all pixels ASAP (but it doesn't work. why?)
  neopixel.setBrightness(255);      // Set BRIGHTNESS to 255 (max = 255)
  delay(1000);                      // Wait for pwm stable
  neopixel.fill(0, 0, NUM_PIXELS);  // Retry to turn OFF all pixels
  neopixel.show();

  int8_t msgid;
  uint16_t *addr;

  MP.begin();

  /* Receive from MainCore */
  MP.Recv(&msgid, &addr);
  meridian_addr = &addr[0]; // Globally maintain the start address of shared memory
}

void loop()
{
  uint8_t animeNum = (uint8_t)meridian_addr[0];
  if (animeNum != lastAnimeNum)
  {
    switch (animeNum)
    {
      case 0:
        clearPixels();
        break;
      case 1:
        loopOrNotFlg = false;
        memcpy(tempAnimation, anime01, sizeof(anime01));
        startAnimation();
        break;
      case 2:
        loopOrNotFlg = false;
        memcpy(tempAnimation, anime02, sizeof(anime02));
        startAnimation();
        break;
      case 3:
        loopOrNotFlg = true;
        memcpy(tempAnimation, anime03, sizeof(anime03));
        startAnimation();
        break;
      case 4:
        loopOrNotFlg = true;
        memcpy(tempAnimation, anime04, sizeof(anime04));
        startAnimation();
        break;
      case 5:
        loopOrNotFlg = true;
        memcpy(tempAnimation, anime05, sizeof(anime05));
        startAnimation();
        break;
      case 6:
        loopOrNotFlg = true;
        memcpy(tempAnimation, anime06, sizeof(anime06));
        startAnimation();
        break;
      case 7:
        loopOrNotFlg = true;
        memcpy(tempAnimation, anime07, sizeof(anime07));
        startAnimation();
        break;
      case 8:
        loopOrNotFlg = true;
        memcpy(tempAnimation, anime08, sizeof(anime08));
        startAnimation();
        break;
      case 9:
        loopOrNotFlg = false;
        memcpy(tempAnimation, anime09, sizeof(anime09));
        startAnimation();
        break;
      case 10:
        loopOrNotFlg = false;
        memcpy(tempAnimation, anime10, sizeof(anime10));
        startAnimation();
        break;
      case 11:
        loopOrNotFlg = false;
        memcpy(tempAnimation, anime11, sizeof(anime11));
        startAnimation();
        break;
      default:
        break;
    }
    lastAnimeNum = animeNum;
  }
}

/* Timer interrupt function. Just send data via pwm at 24FPS. */
unsigned int neopixelInterrupt()
{
  if (!loopAnimeFlg)
  {
    neopixel.fill(neopixel.Color(tempAnimation[0][t], tempAnimation[1][t], tempAnimation[2][t]), 0, NUM_PIXELS);
    neopixel.show();
    t++;
  }
  if (t>=NUM_FRAMES)
  {
    t=0;
    if (!loopOrNotFlg)
    {
      loopAnimeFlg = true;
    }
  }
  return INTERVAL_US;
}

/* Turn OFF all of pixels and Stop pwm data transmission */
void clearPixels()
{
  detachTimerInterrupt();           // Stop timer interrupt
  delay(100);
  neopixel.fill(0, 0, NUM_PIXELS);  // Turn OFF all pixels
  neopixel.show();
  memset(tempAnimation, 0, sizeof(tempAnimation));
}

/* Play the animation from the first point */
void startAnimation()
{
  t = 0;
  attachTimerInterrupt(neopixelInterrupt, INTERVAL_US); // start timer interrupt
}
