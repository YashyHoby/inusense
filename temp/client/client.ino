
#include "GlobalVariables.h"
#include "AudioRecorderPlayer.h"
#include "HttpClient.h"
#include "TriggerWordRecognition.h"


#define RECORD_FILE_NAME "record.mp3"
#define RECEIVE_FILE_NAME "unchi.txt"
#define PLAY_FILE_NAME "audio/Morning_10s.mp3"
#define  CONSOLE_BAUDRATE  115200

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


  initialize_audio();
  initialization_TWR();
  //initialize_http();
}

void loop()
{
  int triggerWord_index = -1;
  Serial.println("Ready?");
  //delay(20000);
  //rec_audio_mp3(RECORD_FILE_NAME);
  //delay(10000);
  //Serial.println("next!");
  httpStat = GET;
  switch (httpStat) {
      case POST:
          //handleHttpPost(RECORD_FILE_NAME);
          break;
      case GET:
          //downloadAudioFile_timeout(RECEIVE_FILE_NAME);
          //downloadTextFile(RECEIVE_FILE_NAME);
          //downloadAudioFile(RECEIVE_FILE_NAME);
          break;
      default:
          break;
  }

  Serial.println("trigger detection");
  triggerWord_index = triggerWordRecognition();

  Serial.println("next");
  //start_player("sony.mp3");
}


void test_recAndPlay_mp3()
{
  start_recorder(RECORD_FILE_NAME);
  delay(10000);
  Serial.println("next!");
  start_player(PLAY_FILE_NAME);
  Serial.println("end!");
  delay(50000);
}