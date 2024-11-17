
#include "GlobalVariables.h"
#include "AudioRecorderPlayer.h"
#include "HttpClient.h"
#include "TriggerWordRecognition.h"


#define RECORD_FILE_NAME "record.mp3"
#define RECEIVE_FILE_NAME "unchi.txt"
#define PLAY_FILE_NAME "audio/Morning_10s.mp3"
#define  CONSOLE_BAUDRATE  115200

#define IRPIN A0  // 赤外線入力ピン


const char label[3][8] = {"itteki", "tadaima", "nene"}; // ラベル用テキスト

void setup()
{
  Serial.begin(CONSOLE_BAUDRATE);
  while (!Serial)
  {
    ; // シリアルポートの接続を待機
  }

  /* Initialize SD */
  while (!theSD.begin())
  {
    /* wait until SD card is mounted. */
    Serial.println("Insert SD card.");
  }

  // ハードウェアUART (Serial2) の初期化
  Serial2.begin(9600); // ボーレートを9600bpsに設定
  delay(1000);

  initialize_audio();
  initialization_TWR();
  //initialize_http();

  Serial2.print("sextutoaxtupukanryo-.\r");
  delay(2000);
  httpStat = POST;
  Serial.println("Setup complete");
  speakText("sextutoaxtupukanryo-");
}

void loop()
{ 
  Serial.println("start loop");
  delay(1000);

  //test_recAndPlay_mp3();
  normal_mode();
}

// 赤外線
void wait_person_detection(){
  while(analogRead(IRPIN) > 300); // 人物が検知されるまで待機
  Serial.println("person detected");
}

void person_detection(){
  // 人がいたらtrue
  if(analogRead(IRPIN) > 300){
    return true;
  }else{
    return false;
  }
}

// 録音再生テスト
void test_recAndPlay_mp3(){
  speakText("rokuon.\r");
  start_recorder(RECORD_FILE_NAME);
  delay(10000);
  speakText("saisei.\r");
  start_player(RECORD_FILE_NAME);
  speakText("owari.\r");
  delay(50000);
}

// 通常モード
void normal_mode(){
  Serial.println("normal mode");
  wait_person_detection();
  speakText("ohayo-");
  
  unsigned long startTime = millis();
  while(true){
    int triggerWord_index = triggerWordRecognition();
    // トリガー分岐
    if(triggerWord_index == 0){
      // 行ってきます
      alert_mode();
      startTime = millis(); // リセット
    }else if(triggerWord_index == 1){
      // ただいま
      speakText("waai");
      startTime = millis(); // リセット
    }else if(triggerWord_index == 2){
      // ねえねえ
      interaction_mode();
      startTime = millis(); // リセット
    }

    // 人がいるかどうか
    if(person_detection){
      startTime = millis(); // リセット
    }

    // 経過時間
    unsigned long elapsedTime = millis() - startTime;
    if(elapsedTime > 10000){
      break;
    }
  }
  speakText("matane");
  delay(5000);
}

// 警戒モード
void alert_mode(){
  Serial.println("alert mode");
  // 人が去るまで待機させたい
  // delay(20000); 
  speakText("ixtuteraxtusyai");
  wait_person_detection();

  while(true){
    speakText("okaeri-");
    int triggerWord_index = triggerWordRecognition();
    // トリガー分岐
    if(triggerWord_index == 1){
      // ただいま
    }else{
      // line通知
      Serial2.print("e.\r");
      delay(1000); 
    }
  }
}

// 会話モード
void interaction_mode(){
  Serial.println("interaction mode");
  speakText("naani");
  start_recorder(RECORD_FILE_NAME);
  delay(5000);

  // 録音データ送信

  // テキスト受信
  char answer = "";
  speakText(answer);
}

void speakText(char* text){
  unsigned long timeout = millis() + 5000; // 5秒のタイムアウトを設定
  while (true) {
    if (millis() > timeout) {
      //Serial.println("Timeout waiting for LSI to be ready.");
      return; // タイムアウト時に終了
    }

    if (Serial2.available()) {
      String receivedData = Serial2.readStringUntil('\n'); // 改行まで読み取る
      receivedData.trim(); // 空白や改行を削除
      if (receivedData == ">") {
        Serial.println("Sound end & Aquestalk LSI ready");
        break;
      } else if (receivedData == "*") {
        Serial.println("ATP3011F4 busy");
      } else {
        Serial.print("Unknown data received via UART: ");
        Serial.println(receivedData);
      }
    }
    delay(10); // 過度のCPU負荷を避けるため少し待機
  }

  String message = String(text) + '.\r';
  Serial2.print(message); // テキストを送信
  //Serial2.print('.\r'); // 必要に応じてCRを追加
}