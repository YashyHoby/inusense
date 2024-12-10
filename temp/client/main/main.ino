
#include "GlobalVariables.h"
#include "AudioRecorderPlayer.h"
#include "HttpClient.h"


#define RECORD_FILE_NAME "question.mp3"
#define SEND_FILE_NAME "test.mp3" // デバック用
#define RECEIVE_FILE_NAME "answer.txt"
#define PLAY_FILE_NAME "audio/Morning_10s.mp3"
#define  CONSOLE_BAUDRATE  115200

#define IRPIN A0  // 赤外線入力ピン



void setup()
{
  Serial.begin(CONSOLE_BAUDRATE);
  while (!Serial)
  {
    ; // シリアルポート接続待機
  }

  /* SDカード初期化 */
  while (!theSD.begin())
  {
    Serial.println("Insert SD card.");
  }

  initialize_http();
  initialize_audio();
  

  delay(5000);
  Serial.println("Setup complete");
}

void loop()
{ 
  Serial.println("start main loop");
  delay(1000);

  // line通知リクエスト
  Serial.println("send LINE request");
  post_requestLINE();

  // オーディオファイルを送信
  post_audioFile(SEND_FILE_NAME);

  // 録音と再生
  //test_recAndPlay_mp3();

  Serial.println("end main loop");
  while(1){
      // 停止
  }
}


// 録音再生テスト
void test_recAndPlay_mp3(){
  start_recorder(RECORD_FILE_NAME);
  delay(10000);
  start_player(RECORD_FILE_NAME);
  delay(50000);
}