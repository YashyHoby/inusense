
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



  httpStat = POST;
  Serial.println("Setup complete");
  Serial2.print("sextutoaxtupukanryo-.\r");
}

void loop()
{ 
  Serial.println("start loop");

  // switch (httpStat) {  
  //     case POST:
  //         //handleHttpPost(RECORD_FILE_NAME);
  //         break;
  //     case GET:
  //         //downloadAudioFile_timeout(RECEIVE_FILE_NAME);
  //         //downloadTextFile(RECEIVE_FILE_NAME);
  //         //downloadAudioFile(RECEIVE_FILE_NAME);
  //         break;
  //     default:
  //         break;
  // }

  Serial.println("trigger detection");
  int triggerWord_index = -1;
  triggerWord_index = triggerWordRecognition();

  Serial.println("next");
  //start_player("sony.mp3");
}

void wait_person_detection(){
  while(analogRead(IRPIN) > 300); // 人物が検知されるまで待機
}

void test_recAndPlay_mp3(){
  start_recorder(RECORD_FILE_NAME);
  delay(10000);
  Serial.println("next!");
  start_player(PLAY_FILE_NAME);
  Serial.println("end!");
  delay(50000);
}

void normal_mode(){
  wait_person_detection();
  int triggerWord_index = triggerWordRecognition();
  Serial2.print("ohayo-.\r");
  while(true){
    triggerWord_index = triggerWordRecognition();
    // トリガー分岐
    if(triggerWord_index == 0){

    }else if(triggerWord_index == 1){

    }else if(triggerWord_index == 2){
      
    }
  }
}

void alert_mode(){
  wait_person_detection();
  Serial2.print("okaeri-.\r");
}

void interaction_mode(){
}