#include "AudioRecorderPlayer.h"

AudioClass *theAudio;
bool ErrEnd = false;

/**
 * @brief Audio attention callback
 *
 * When audio internal error occurs, this function will be called back.
 */
void audio_attention_cb(const ErrorAttentionParam *atprm)
{
  Serial.println("Attention!");

  if (atprm->error_code >= AS_ATTENTION_CODE_WARNING)
  {
    ErrEnd = true;
  }
}

void initialize_audio()
{
  theAudio = AudioClass::getInstance();
  theAudio->begin(audio_attention_cb);
  theAudio->setRenderingClockMode(AS_CLKMODE_NORMAL);
  Serial.println("Initialization Audio Library");
  //theAudio->setReadyMode();
}

void start_recorder(const char* recFileName)
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
  if (theSD.exists(recFileName))
  {
    Serial.print("Remove existing file [");
    Serial.print(recFileName);
    Serial.println("].");
    theSD.remove(recFileName);
  }

  File myFile = theSD.open(recFileName, FILE_WRITE);
  /* Verify file open */
  if (!myFile)
  {
    Serial.println("File open error");
    return;
  }

  Serial.print("Open! [");
  Serial.print(recFileName);
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
    if (err != AUDIOLIB_ECODE_OK || ErrEnd)
    {
      Serial.print("File End! =");
      Serial.println(err);
      theAudio->stopRecorder();
      break;
    }
    usleep(10000);
  }

  theAudio->closeOutputFile(myFile);
  myFile.close();
  theAudio->setReadyMode();
  Serial.println("End Recording");
}

void start_player(const char* recFileName){
  bool isErr = false;
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
    isErr = true;
  }

  /* Open file placed on SD card */
  File myFile = theSD.open(recFileName, FILE_READ);
  if (!myFile)
  {
    Serial.print("File open error\n");
    isErr = true;
  }else{
    Serial.print("Open! [");
    Serial.print(recFileName);
    Serial.println("]");
  }
  /* Send first frames to be decoded */
  err = theAudio->writeFrames(AudioClass::Player0, myFile);
  if ((err != AUDIOLIB_ECODE_OK) && (err != AUDIOLIB_ECODE_FILEEND))
    {
      Serial.print("File Read Error! =");
      Serial.print(err);
      isErr = true;
    }


  if (!isErr)
  {
    /* Main volume set */
    //theAudio->setVolume(audioLevel);
    theAudio->startPlayer(AudioClass::Player0);
    Serial.println("Play!");
    while (1)
    {
      //Serial.println("loop!!");

      /* Send new frames to decode in a loop until file ends */
      int err = theAudio->writeFrames(AudioClass::Player0, myFile);

      /*  Tell when player file ends */
      if (err == AUDIOLIB_ECODE_FILEEND)
        {
          Serial.println("Main player File End!");
          theAudio->stopPlayer(AudioClass::Player0, AS_STOPPLAYER_NORMAL);
          break;
        }

      /* Show error code from player and stop */
      if (err)
        {
          Serial.println("Main player error code:");
          Serial.println(err);
          theAudio->stopPlayer(AudioClass::Player0, AS_STOPPLAYER_NORMAL);
          break;
        }

      if (ErrEnd)
        {
          Serial.println("Error End");
          theAudio->stopPlayer(AudioClass::Player0, AS_STOPPLAYER_NORMAL);
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
    
    theAudio->setReadyMode();
    Serial.println("End playback");
  }
  myFile.close();
}

void close_recorder(File myfile)
{
  
}

void close_player(File myfile)
{
}