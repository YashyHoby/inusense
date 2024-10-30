#include "HttpFileSender.h"

void HttpFileSender::sendFile(File file){
	bool result = false;
  const uint16_t SEND_BUF_SIZE = 8192;
  static char audioDataBin[SEND_BUF_SIZE];
  static char audioDataStr[2*SEND_BUF_SIZE+1];

  if (file) {
    // Serial.println("001.wav:");
    unsigned long fileSize = file.size();
    // Serial.print("file size:");
    // Serial.print(fileSize);
    // Serial.println(" B");

    int maxSendTimes = (fileSize/sizeof(audioDataBin)) + 1;//postで送信する回数

    file.seek(0);//set pointer to zero    
    for(int send_i = 0;send_i<maxSendTimes;send_i++){
      // Serial.print("Sending data ");
      // Serial.print(send_i+1);
      // Serial.print("/");
      // Serial.print(maxSendTimes);
      // Serial.println();
      //ファイルから固定長データを順番に読み込み
      int end = sizeof(audioDataBin);
      if(send_i == maxSendTimes-1){
        for(int i=0;file.available();i++){//最後のフレームよみこみ
          audioDataBin[i] = file.read();
          end = i;
        }
        end += 1;
        // httpStat = GET;
      }else{
        for(unsigned int i=0;i<sizeof(audioDataBin);i++){
          audioDataBin[i] = file.read();
        }
      }
      //POST
      this->config(HTTP_HEADER_TRANSFER_ENCODING, "chunked");
      
      //バイナリデータのhex表記(0x??)を文字(ASCII)に変換．(データサイズは倍になるが仕方ない...)
      for(unsigned int i=0;i<sizeof(audioDataBin);i++){
        int d1 = audioDataBin[i] >> 4;//0x??の上位桁.4bit右シフトして取り出す．
        int d2 = audioDataBin[i] & 15;//0x??の下位桁.ビットマスク(0b00001111)の積で取り出す．
        itoa(d1,&audioDataStr[i*2],16);
        itoa(d2,&audioDataStr[(i*2)+1],16);
      }
      audioDataStr[end*2] = '\0';//add Null string

      result = this->post(HTTP_POST_PATH, audioDataStr);
      if (false == result) {
        break;
      }

      do {
        result = this->receive(5000);
        if (result) {
          this->read_data(this->Receive_Data, this->RECEIVE_PACKET_SIZE);
          // ConsolePrintf("%s", (char *)(this->Receive_Data));
        } else {
          // AT+HTTPSEND command is done
          // ConsolePrintf( "\r\n");
        }
      } while (result);

      result = this->end();
    }
    file.close();
    //終了通知
    result = this->post(HTTP_POST_PATH, "end");
    if (false == result) {
      // break;
      ;
    }

    do {
      result = this->receive(5000);
      if (result) {
        this->read_data(this->Receive_Data, this->RECEIVE_PACKET_SIZE);
        // ConsolePrintf("%s", (char *)(this->Receive_Data));
      } else {
        // AT+HTTPSEND command is done
        // ConsolePrintf( "\r\n");
      }
    } while (result);

    result = this->end();
  } else {
    /* If the file didn't open, print an error */
    Serial.println("error opening 001.wav");
  }
}