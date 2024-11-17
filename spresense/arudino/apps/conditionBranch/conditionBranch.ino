#ifdef SUBCORE
#error "Core selection is wrong!!"
#endif

#include <Audio.h>
#include <FFT.h>
#include <SDHCI.h>
#include <float.h>
#include <DNNRT.h>
#include "audioAI.h"
#include "player.h"

#define NNB_FILE "model.nnb"
#define VALUE_TH 0.95 // AIの確からしさの閾値

#define IRPIN A0  // 赤外線入力ピン

FFTClass<AS_CHANNEL_STEREO, FFT_LEN> FFT; // ステレオ、512サンプルでFFTを初期化
AudioClass* theAudio = AudioClass::getInstance();
SDClass SD;
DNNRT dnnrt;

bool isProcessing = false;  // 処理中フラグ
const char label[3][8] = {"itteki", "tadaima", "nene"}; // ラベル用テキスト

void setup() {
  Serial.begin(115200);
  // delay(30000);   // 赤外線センサが安定するまで30秒待つ
  delay(5000);       // 開発用に5秒だけにしとく

  /* SDカードの挿入を待つ */
  while (!SD.begin()) {Serial.println("insert SD card");}  
  File nnbfile = SD.open(NNB_FILE);
  if (!nnbfile) {
    Serial.println(String(NNB_FILE) + " not found");
    return;
  }

  /* DNNRTを学習済みデータで開始 */
  int ret = dnnrt.begin(nnbfile);
  if (ret < 0) {
    Serial.println("DNNRT initialization error");
    return;
  }

  FFT.begin(WindowHamming, AS_CHANNEL_STEREO, (FFT_LEN/2)); // ハミング窓、ステレオ、オーバーラップ50%

  theAudio->begin();
  
  nnbfile.close();

  Serial.println("Start loop");
}

void loop() {
  initAudioAI();
  int index = -1; // 認識結果
  /* 認識されるまで runAudioAI() 回す */
  while(1) {
    if (!isProcessing) index = runAudioAI(VALUE_TH);
    if (index != -1) break;
  }
  Serial.println(String(label[index]));
  theAudio->setReadyMode();

  while(analogRead(IRPIN) > 300); // 人物が検知されるまで待機
  play_audio_mp3("sony.mp3");
}

/*
void loop() {
  //　非警戒モードからスタート
  while (analogRead(IRPIN) > 500)　delay(100);  // 人物が検知されるまで待機

  ここで音声1「おはよう」等再生
  
  経過時間基準1 = mills();  // millis()は電源ONされてからの経過時間をミリ秒単位で返します(uint64_t)
  while (1) {
    ここで音声識別
    if (ワードが「いってきます」) 
    {
      while (1) {
        // 警戒モード
        while (analogRead(IRPIN) > 500) delay(100); // 人物が検知されるまで待機

        ここで音声2「おかえり」再生

        経過時間基準2 = mills();
        ここで音声識別
        if (ワードが「ただいま」) {
          break;
        } else {
          // 10秒経過判定
          uint64_t 経過時間 = mills();
          if (経過時間-経過時間基準2 >= 10000) {
            サーバにLINE通知指示
            continue;
          }
        }
      }
    }
    else if (ワードが「あのね」) 
    {
      // 会話モード
      ここで音声3「なあに」再生
      ここで音声録音（おなじように10sで切る？，）
      ここでサーバに音声送信
      ここでサーバから音声取得
      音声再生
    }
    else
    {
      uint64_t 経過時間 = mills();
      if (経過時間-経過時間基準1 >= 10000) {
        break;
      } else {
        ここでサーバの通信継続指示
        continue;
      }
    }
  }
}
*/


