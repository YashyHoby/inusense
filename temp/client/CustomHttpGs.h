/*
HttpGs2200クラスを継承し，ファイル送信する関数を追加したクラス．


*/

#ifndef CUSTOMHTTPGS_h
#define CUSTOMHTTPGS_h
#include "GlobalVariables.h"
#include <HttpGs2200.h>
#include <TelitWiFi.h>
#include "config.h"

class CustomHttpGs:public HttpGs2200
{
  private:
    static const uint16_t RECEIVE_PACKET_SIZE = 1500;
    uint8_t Receive_Data[RECEIVE_PACKET_SIZE] = {0};
  public:
    CustomHttpGs(TelitWiFi* wifi):HttpGs2200(wifi){};
    void sendFile(File file);
    //int readData(uint8_t* data, int length);
};
#endif