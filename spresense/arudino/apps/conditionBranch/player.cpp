#include "player.h"

bool errEnd = false;

void play_audio_mp3(const char* fileName){
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
  File myFile = SD.open(fileName);

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
        // Serial.println("Main player error code:");
        // Serial.println(err);
        break;
      }

    if (errEnd)
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
    //usleep(40000);
  }
  theAudio->stopPlayer(AudioClass::Player0);
  myFile.close();
  theAudio->setReadyMode();
}