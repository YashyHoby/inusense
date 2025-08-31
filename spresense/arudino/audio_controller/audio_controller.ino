#include <SDHCI.h>
#include <Audio.h>

#define RECORD_FILE_NAME "arrr.mp3"
#define audioLevel -160

SDClass theSD;
AudioClass *theAudio;

File myFile;

bool ErrEnd = false;

/**
 * @brief Audio attention callback
 *
 * When audio internal error occurs, this function will be called back.
 */
static void audio_attention_cb(const ErrorAttentionParam *atprm)
{
  Serial.println("Attention!");

  if (atprm->error_code >= AS_ATTENTION_CODE_WARNING)
  {
    ErrEnd = true;
  }
}

/* Recording time[second] */
static const int32_t recoding_time = 4;

/* Recording bit rate
 * Set in bps.
 * Note: 96kbps fixed
 */
static const int32_t recoding_bitrate = 96000;

/* Bytes per second */
static const int32_t recoding_byte_per_second = (recoding_bitrate / 8);

/* Total recording size */
static const int32_t recoding_size = recoding_byte_per_second * recoding_time;

void setup()
{
  Serial.begin(115200);
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

  theAudio = AudioClass::getInstance();
  theAudio->begin(audio_attention_cb);
  Serial.println("Initialization Audio Library");

  //theAudio->setRenderingClockMode(AS_CLKMODE_NORMAL);

  

}

void rec_audio_mp3()
{
  /* Select input device as analog microphone */
  theAudio->setRecorderMode(AS_SETRECDR_STS_INPUTDEVICE_MIC);
  
  /*
   * Initialize filetype to stereo mp3 with 48 Kb/s sampling rate
   * Search for MP3 codec in "/mnt/sd0/BIN" directory
   */
  theAudio->initRecorder(AS_CODECTYPE_MP3, "/mnt/sd0/BIN", AS_SAMPLINGRATE_48000, AS_CHANNEL_STEREO);
  Serial.println("Init Recorder!");

  /* Open file for data write on SD card */
  if (theSD.exists(RECORD_FILE_NAME))
  {
    Serial.print("Remove existing file [");
    Serial.print(RECORD_FILE_NAME);
    Serial.println("].");
    theSD.remove(RECORD_FILE_NAME);
  }

  myFile = theSD.open(RECORD_FILE_NAME, FILE_WRITE);
  /* Verify file open */
  if (!myFile)
  {
    Serial.println("File open error");
    exit(1);
  }

  Serial.print("Open! [");
  Serial.print(RECORD_FILE_NAME);
  Serial.println("]");
  theAudio->startRecorder();
  Serial.println("Recording Start!");

  while (1)
  {
    err_t err;
    /* recording end condition */
    if (theAudio->getRecordingSize() > recoding_size)
    {
      theAudio->stopRecorder();
      sleep(1);
      err = theAudio->readFrames(myFile);

      break;
    }

    /* Read frames to record in file */
    err = theAudio->readFrames(myFile);

    if (err != AUDIOLIB_ECODE_OK)
    {
      Serial.print("File End! =");
      Serial.println(err);
      

      break;
    }

    if (ErrEnd)
    {
      Serial.println("Error End");

      break;
    }
    usleep(10000);
  }

  theAudio->closeOutputFile(myFile);
  myFile.close();
  theAudio->setReadyMode();

  Serial.println("End Recording");
}

void play_audio_mp3(){
  theAudio->setPlayerMode(AS_SETPLAYER_OUTPUTDEVICE_SPHP, AS_SP_DRV_MODE_LINEOUT);
  /*
   * Set main player to decode stereo mp3. Stream sample rate is set to "auto detect"
   * Search for MP3 decoder in "/mnt/sd0/BIN" directory
   */
  err_t err = theAudio->initPlayer(AudioClass::Player0, AS_CODECTYPE_MP3, "/mnt/sd0/BIN", AS_SAMPLINGRATE_AUTO, AS_CHANNEL_STEREO);

  /* Verify player initialize */
  if (err != AUDIOLIB_ECODE_OK)
    {
      Serial.print("Player0 initialize error\n");
      exit(1);
    }

  /* Open file placed on SD card */
  myFile = theSD.open(RECORD_FILE_NAME);

  /* Verify file open */
  if (!myFile)
    {
      Serial.print("File open error\n");
      exit(1);
    }
  Serial.print("Open! [");
  Serial.print(myFile);
  Serial.println("]");

  /* Send first frames to be decoded */
  err = theAudio->writeFrames(AudioClass::Player0, myFile);

  if ((err != AUDIOLIB_ECODE_OK) && (err != AUDIOLIB_ECODE_FILEEND))
    {
      Serial.print("File Read Error! =");
      Serial.print(err);
      myFile.close();
      exit(1);
    }

  Serial.println("Play!");

  /* Main volume set */
  theAudio->setVolume(audioLevel);
  theAudio->startPlayer(AudioClass::Player0);

  while (1)
  {
    Serial.println("loop!!");

    /* Send new frames to decode in a loop until file ends */
    int err = theAudio->writeFrames(AudioClass::Player0, myFile);

    /*  Tell when player file ends */
    if (err == AUDIOLIB_ECODE_FILEEND)
      {
        Serial.println("Main player File End!");
      }

    /* Show error code from player and stop */
    if (err)
      {
        Serial.println("Main player error code:");
        Serial.println(err);
        break;
      }

    if (ErrEnd)
      {
        Serial.println("Error End");
        break;
      }

    /* This sleep is adjusted by the time to read the audio stream file.
    * Please adjust in according with the processing contents
    * being processed at the same time by Application.
    *
    * The usleep() function suspends execution of the calling thread for usec
    * microseconds. But the timer resolution depends on the OS system tick time
    * which is 10 milliseconds (10,000 microseconds) by default. Therefore,
    * it will sleep for a longer time than the time requested here.
    */
    usleep(40000);
  }
  theAudio->stopPlayer(AudioClass::Player0);
  myFile.close();
  theAudio->setReadyMode();
}




void loop()
{
  rec_audio_mp3();
  delay(10000);
  Serial.println("next!");
  play_audio_mp3();
  delay(100000);
  theAudio->end();
}
//  usleep(10000);