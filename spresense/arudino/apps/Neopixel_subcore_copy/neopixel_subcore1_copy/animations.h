/* NeoPixelを光らせるためのアニメーション（毎フレームごとの色）を定義 */

#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#define NUM_PIXELS 24
#define NUM_FRAMES 48
#define NUM_COLORS 3

extern const uint8_t anime01[NUM_COLORS][NUM_FRAMES]; // 非警戒モード/会話モードの時に赤外線センサが反応したときのアニメーション，ゆっくり点灯
extern const uint8_t anime02[NUM_COLORS][NUM_FRAMES]; // 警戒モードの時に赤外線センサが反応したときのアニメーション，ゆっくり点灯
extern const uint8_t anime03[NUM_COLORS][NUM_FRAMES]; // 非警戒モード/会話モードの時に話している間
extern const uint8_t anime04[NUM_COLORS][NUM_FRAMES]; // 警戒モードの時に話している間
extern const uint8_t anime05[NUM_COLORS][NUM_FRAMES]; // 非警戒モード/会話モードの時に聞いている間
extern const uint8_t anime06[NUM_COLORS][NUM_FRAMES]; // 警戒モードの時に聞いている間
extern const uint8_t anime07[NUM_COLORS][NUM_FRAMES]; // 考えている間（サーバ処理中）
extern const uint8_t anime08[NUM_COLORS][NUM_FRAMES]; // 侵入者を検知したときのアニメーション
extern const uint8_t anime09[NUM_COLORS][NUM_FRAMES]; // 非警戒モード/会話モードから警戒モードへの遷移
extern const uint8_t anime10[NUM_COLORS][NUM_FRAMES]; // 警戒モードから非警戒モードへの遷移
extern const uint8_t anime11[NUM_COLORS][NUM_FRAMES]; // ゆっくり消灯するとき（会話モードで赤外線検知後，タイムアウトしたとき）

#endif //ANIMATIONS_H