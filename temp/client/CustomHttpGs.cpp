#include "CustomHttpGs.h"

/*
親クラスのpost()関数は送信するデータとして文字列のみを想定した仕様になっている．(NULL文字'\0'(0x00)が途中に含まれるデータを最後まで送信できない．)
そのため，ファイルのバイナリデータのhex表記を文字列に変換し送信する関数sendFile()を追加した．
ファイルは固定長のバッファ(fileDataBin)に読み込まれ，変換されバッファ(fileDataStr)に格納した後複数回に分けて送信される．
なぜか最初の4096Bのフレームが送信されない問題...(10/30)
*/
void CustomHttpGs::sendFile(File file){
	bool result = false;
  const uint16_t SEND_BUF_SIZE = 4096;
  static char fileDataBin[SEND_BUF_SIZE];
  static char fileDataStr[2*SEND_BUF_SIZE+1];

  if (file) {
    // Serial.println("001.wav:");
    unsigned long fileSize = file.size();
    // Serial.print("file size:");
    // Serial.print(fileSize);
    // Serial.println(" B");

    int maxSendTimes = (fileSize/sizeof(fileDataBin)) + 1;//postで送信する回数

    file.seek(0);//set pointer to zero    
    for(int send_i = 0;send_i<maxSendTimes;send_i++){
      // Serial.print("Sending data ");
      // Serial.print(send_i+1);
      // Serial.print("/");
      // Serial.print(maxSendTimes);
      // Serial.println();
      //ファイルから固定長データを順番に読み込み
      int end = sizeof(fileDataBin);
      if(send_i == maxSendTimes-1){
        for(int i=0;file.available();i++){//最後のフレームよみこみ
          fileDataBin[i] = file.read();
          end = i;
        }
        end += 1;
        // httpStat = GET;
      }else{
        for(unsigned int i=0;i<sizeof(fileDataBin);i++){
          fileDataBin[i] = file.read();
        }
      }
      //POST
      this->config(HTTP_HEADER_TRANSFER_ENCODING, "chunked");
      
      //バイナリデータのhex表記(0x??)を文字(ASCII)に変換．(データサイズは倍になるが仕方ない...)
      for(unsigned int i=0;i<sizeof(fileDataBin);i++){
        int d1 = fileDataBin[i] >> 4;//0x??の上位桁.4bit右シフトして取り出す．
        int d2 = fileDataBin[i] & 15;//0x??の下位桁.ビットマスク(0b00001111)の積で取り出す．
        itoa(d1,&fileDataStr[i*2],16);
        itoa(d2,&fileDataStr[(i*2)+1],16);
      }
      fileDataStr[end*2] = '\0';//add Null string

      result = this->post(HTTP_POST_PATH, fileDataStr);
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
    //終了通知．
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
    Serial.println("error opening file");
  }
}