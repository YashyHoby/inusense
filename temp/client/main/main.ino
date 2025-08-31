
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

  // ハードウェアUART (Serial2) の初期化
  Serial2.begin(9600); // ボーレートを9600bpsに設定
  sleep(2);

  initialize_http();
  initialize_audio();
  

  delay(5000);
  Serial.println("Setup complete");
  Serial2.print("sextutoaxtupukanryo-.\r");
}

void loop()
{ 
  Serial.println("start main loop");
  delay(5000);

  // line通知リクエスト
  //Serial.println("send LINE request");
  //post_requestLINE();
  Serial2.print("reko-dosuta-to.\r");
  start_recorder(RECORD_FILE_NAME);

  // オーディオファイルを送信
  Serial2.print("soushin.\r");
  post_audioFile(RECORD_FILE_NAME);
  
  String response = get_text();
  Serial2.print(response);
  delay(5000);

  // 録音と再生
  //test_recAndPlay_mp3();
  Serial2.print("owari.\r");
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