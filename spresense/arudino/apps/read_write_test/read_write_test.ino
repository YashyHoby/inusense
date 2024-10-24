/**
 * @file read_write.ino
 * @author Sony Semiconductor Solutions Corporation
 * @brief SD card read/write sample application.
 */

#include <Arduino.h>
#include <SDHCI.h>
#include <File.h>
#include "config.h"

SDClass SD;  /**< SDClass object */ 

//File myFile; /**< File object */ 
File audioFile;

/**
 * @brief Write to the file and read from the file.
 * 
 * @details The file is located on the SD card.
 */
void setup() {

  /* Open serial communications and wait for port to open */
  Serial.begin(115200);
  while (!Serial) {
    ; /* wait for serial port to connect. Needed for native USB port only */
  }

  /* Initialize SD */
  Serial.print("Insert SD card.");
  while (!SD.begin()) {
    ; /* wait until SD card is mounted. */
  }

  /* Create a new directory */
  //SD.mkdir("dir/");

  /* Open the file. Note that only one file can be open at a time,
     so you have to close this one before opening another. */
  //myFile = SD.open("dir/test.txt", FILE_WRITE);
  audioFile = SD.open("voice_to_send/001.wav", FILE_READ);

  /* If the file opened okay, write to it */
  // if (myFile) {
  //   Serial.print("Writing to test.txt...");
  //   myFile.println("testing 1, 2, 3.");
  //   /* Close the file */
  //   myFile.close();
  //   Serial.println("done.");
  // } else {
  //   /* If the file didn't open, print an error */
  //   Serial.println("error opening test.txt");
  // }

  /* Re-open the file for reading */
  audioFile = SD.open("voice_to_send/001.wav");
  char* audioData;
  
  if (audioFile) {
    Serial.println("001.wav:");
    // int bufSize = sizeof(audioFile);
    unsigned long fileSize = audioFile.size();
    Serial.print("size:");
    Serial.print(fileSize);
    Serial.println(" byte");

    audioData = (char*)calloc(fileSize,sizeof(char));
    
    //copy to array
    for(int i=0;i<fileSize;i++){
      audioData[i] = audioFile.read();
    }
    audioFile.seek(0);//set pointer to zero

    //show array
    for(int i=0;i<fileSize;i++){
      Serial.print(audioData[i],HEX);
      Serial.print(",");
    }
    Serial.println();
    
    /* Read from the file until there's nothing else in it */
    while (audioFile.available()) {
      // Serial.write(audioFile.read());
      Serial.print(audioFile.read(),HEX);
      Serial.print(",");
    }
    Serial.println();
    /* Close the file */
    audioFile.close();
    free(audioData);
  } else {
    /* If the file didn't open, print an error */
    Serial.println("error opening 001.wav");
  }
}

/**
 * @brief Run repeatedly.
 * 
 * @details Does not do anything.
 */
void loop() {

}
