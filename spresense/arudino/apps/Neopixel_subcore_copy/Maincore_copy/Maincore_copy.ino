#ifdef SUBCORE
#error "Core selection is wrong!!"
#endif

#include <MP.h>
#define MEMSIZE 8        // Meridimの共有メモリのサイズ（8bit）
int16_t *meridian_addr;  // 共有メモリのアドレス用グローバル変数

#define subcore1 1

void setup() {
  MP.begin(subcore1); // サブコア開始．サブコアでタイマ割込まわしてるせいでSerial.print()使えへんごめんなさい．
  delay(1000);

  Serial.begin(115200);
  delay(1000);

  /* 共有メモリの確保 */
  uint16_t *addr = (uint16_t *)MP.AllocSharedMemory(MEMSIZE);
  if(!addr) {
    Serial.println("Error: out of memory");
    return;
  }

  Serial.printf("SharedMemory Address: @%08lx\n", (uint32_t)addr);  // 共有メモリのアドレスを表示
  meridian_addr = &addr[0]; // 共有メモリの開始アドレスをグローバル変数に維持する

  /* 共有メモリのイニシャライズ */
  for (int i=0; i<MEMSIZE; i++) {
    addr[i]=0x0000;
  }

  /* サブコアに共有メモリのアドレスを送信 */
  int8_t msgid = 10;
  MP.Send(msgid, addr, subcore1);

  // /* 共有メモリのチェック */
  // for (int i = 0; i < MEMSIZE; i++) {
  //   if (addr[i] != 0x0000) {
  //     Serial.printf("Error: @%08lx\n", (uint32_t)&addr[i]);
  //     while (1);
  //   }
  // }
  // Serial.println("SharedMemory Check: OK!");
}

void loop()
{
  setAnimation(1);  // Start animation No.1
  delay(2000);
  setAnimation(0);  // turn OFF neopixel
  delay(2000);
}

/* set Neopixel animation (0-11). animations detail is discribed in <Neopixel_subcore/animations.h> */
/* if you want to change animation scene, plz chenge matrix data defined in <Neopixel_subcore/animations.cpp>*/
/* NeoPixelを光らせるための関数，アニメーションが1-11の11個用意されてます．（animetions.cpp内に定義） */
/* 全部2s間で，ループするものとしないものがあります． */
// 0: 消灯
// 1: 非警戒モード/会話モードの時に赤外線センサが反応したときのアニメーション
// 2: 警戒モードの時に赤外線センサが反応したときのアニメーション
// 3: 非警戒モード/会話モードの時に話している間（ループ）
// 4: 警戒モードの時に話している間（ループ）
// 5: 非警戒モード/会話モードの時に聞いている間（ループ）
// 6: 警戒モードの時に聞いている間（ループ）
// 7: 考えている間（サーバ処理中）（ループ）
// 8: 侵入者を検知したときのアニメーション（ループ）
// 9: 非警戒モード/会話モードから警戒モードへの遷移
// 10: 警戒モードから非警戒モードへの遷移
// 11: ゆっくり消灯するとき（会話モードで赤外線検知後，タイムアウトしたとき）
void setAnimation(uint8_t animeNum)
{
  if (0 <= animeNum <= 11)
  {
    meridian_addr[0]=animeNum;
  }
}
