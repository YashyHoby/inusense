#if (SUBCORE != 1)
#error "Core selection is wrong!!"
#endif

#include <Adafruit_NeoPixel_Spresense.h>
#include <MP.h>
#include "animations.h"

#define PIN 6             // D06 pin for neopixel pwm
#define INTERVAL_US 41666 // タイマー割込みのインターバル：41.6ms（24Hz）

#define MEMSIZE 8        // Meridimの共有メモリのサイズ（8bit）
int16_t *meridian_addr;  // 共有メモリのアドレス用グローバル変数

uint8_t t = 0;  // アニメーションのフレーム管理用，startAnimation()内でアニメーション開始時にt=0にされる
uint8_t tempAnimation[NUM_COLORS][NUM_FRAMES] = {{0}};  // アニメーションコピー用

uint8_t lastAnimeNum;
bool isloopOrNot = false; // ループアニメーションのときtrueにするやつ (0:non-loop, 1:loop)
bool isDone = false;      // 非ループアニメーションの時に立ち上がるとアニメーションストップ

Adafruit_NeoPixel_Spresense neopixel(NUM_PIXELS, PIN);

void setup()
{
  /* Initialize neopixel strip */
	neopixel.begin();                 // Initialize neopixel object
  neopixel.show();                  // Turn OFF all pixels ASAP (but it doesn't work. why?)
  neopixel.setBrightness(255);      // 明るさを255に設定（max：255）
  delay(1000);                      // pwmが安定するまでちょっと待つ
  neopixel.fill(0, 0, NUM_PIXELS);  // Retry to turn OFF all pixels
  neopixel.show();

  int8_t msgid;   //  共有メモリID
  uint16_t *addr; //

  MP.begin();

  /* Receive from MainCore */
  /* メインコアから共有メモリのアドレスを受信 */
  MP.Recv(&msgid, &addr);
  meridian_addr = &addr[0]; // 共有メモリの開始アドレスをグローバル変数に維持する
}

/* loop内ではひたすら共有メモリを監視，変更があったらアニメーションをtempAnimationにコピーする */
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
        isloopOrNot = false;
        memcpy(tempAnimation, anime01, sizeof(anime01));
        startAnimation();
        break;
      case 2:
        isloopOrNot = false;
        memcpy(tempAnimation, anime02, sizeof(anime02));
        startAnimation();
        break;
      case 3:
        isloopOrNot = true;
        memcpy(tempAnimation, anime03, sizeof(anime03));
        startAnimation();
        break;
      case 4:
        isloopOrNot = true;
        memcpy(tempAnimation, anime04, sizeof(anime04));
        startAnimation();
        break;
      case 5:
        isloopOrNot = true;
        memcpy(tempAnimation, anime05, sizeof(anime05));
        startAnimation();
        break;
      case 6:
        isloopOrNot = true;
        memcpy(tempAnimation, anime06, sizeof(anime06));
        startAnimation();
        break;
      case 7:
        isloopOrNot = true;
        memcpy(tempAnimation, anime07, sizeof(anime07));
        startAnimation();
        break;
      case 8:
        isloopOrNot = true;
        memcpy(tempAnimation, anime08, sizeof(anime08));
        startAnimation();
        break;
      case 9:
        isloopOrNot = false;
        memcpy(tempAnimation, anime09, sizeof(anime09));
        startAnimation();
        break;
      case 10:
        isloopOrNot = false;
        memcpy(tempAnimation, anime10, sizeof(anime10));
        startAnimation();
        break;
      case 11:
        isloopOrNot = false;
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
/* タイマー割込み用の関数，約24Hzで割込み */
unsigned int neopixelInterrupt()
{
  if (!isDone) {
    neopixel.fill(neopixel.Color(tempAnimation[0][t], tempAnimation[1][t], tempAnimation[2][t]), 0, NUM_PIXELS);
    neopixel.show();
    t++;
  }
  if (t>=NUM_FRAMES) {
    t=0;
    if (!isloopOrNot)
    {
      isDone = true;
    }
  }
  return INTERVAL_US;
}

/* Turn OFF all of pixels and Stop pwm data transmission */
/* Neopixelを消灯，pwmデータ送信を停止 */
void clearPixels()
{
  detachTimerInterrupt();           // Stop timer interrupt
  delay(100);
  neopixel.fill(0, 0, NUM_PIXELS);  // Turn OFF all pixels
  neopixel.show();
  memset(tempAnimation, 0, sizeof(tempAnimation));
}

/* Play the animation from the first point */
/* 最初からアニメーションを開始 */
void startAnimation()
{
  t = 0;
  attachTimerInterrupt(neopixelInterrupt, INTERVAL_US); // start timer interrupt
}