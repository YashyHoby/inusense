
#include "GlobalVariables.h"
#include "AudioRecorderPlayer.h"
#include "HttpClient.h"


#define RECORD_FILE_NAME "record.mp3"
#define RECEIVE_FILE_NAME "aa.mp3"
#define PLAY_FILE_NAME "audio/Morning_10s.mp3"
#define  CONSOLE_BAUDRATE  115200

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

  initialize_http();
}

void loop()
{
  Serial.println("Ready?");
  //delay(20000);
  //rec_audio_mp3(RECORD_FILE_NAME);
  //delay(10000);
  //Serial.println("next!");
  httpStat = GET;
  switch (httpStat) {
      case POST:
          handleHttpPost(RECORD_FILE_NAME);
          break;
      case GET:
          downloadAudioFile(RECEIVE_FILE_NAME);
          //downloadAudioFile(RECEIVE_FILE_NAME);
          break;
      default:
          break;
  }
  Serial.println("next");
  play_audio_mp3(PLAY_FILE_NAME);
  exit(1);
}


void test_recAndPlay_mp3()
{
  rec_audio_mp3(RECORD_FILE_NAME);
  delay(10000);
  Serial.println("next!");
  play_audio_mp3(PLAY_FILE_NAME);
  Serial.println("end!");
  delay(50000);
}