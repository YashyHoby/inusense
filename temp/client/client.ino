#include <HttpGs2200.h>
#include <TelitWiFi.h>
#include <SDHCI.h>  // SpresenseのSDカードライブラリ
#include "config.h"

#define CONSOLE_BAUDRATE 115200
#define MP3_FILE_PATH "/mnt/audio/Moning_10s.mp3"  // SDカード上のMP3ファイルのパス

SDClass SD;

TelitWiFi gs2200;
TWIFI_Params gsparams;
HttpGs2200 theHttpGs2200(&gs2200);
HTTPGS2200_HostParams hostParams;

void setup() {
    pinMode(LED0, OUTPUT);
    digitalWrite(LED0, LOW);
    Serial.begin(CONSOLE_BAUDRATE);

    // SDカードの初期化
    if (!SD.begin()) {
        ConsoleLog("SDカードの初期化に失敗しました");
        while (1);
    }

    Init_GS2200_SPI_type(iS110B_TypeC);
    gsparams.mode = ATCMD_MODE_STATION;
    gsparams.psave = ATCMD_PSAVE_DEFAULT;
    if (gs2200.begin(gsparams)) {
        ConsoleLog("GS2200の初期化に失敗しました");
        while (1);
    }

    if (gs2200.activate_station(AP_SSID, PASSPHRASE)) {
        ConsoleLog("WiFiの接続に失敗しました");
        while (1);
    }

    hostParams.host = (char *)HTTP_SRVR_IP;
    hostParams.port = (char *)HTTP_PORT;
    theHttpGs2200.begin(&hostParams);
}

void sendMp3File() {
    File mp3File = SD.open(MP3_FILE_PATH, FILE_READ);  // SDHCIを使ったファイルのオープン
    if (!mp3File) {
        ConsoleLog("MP3ファイルのオープンに失敗しました");
        return;
    }

    theHttpGs2200.config(HTTP_HEADER_CONTENT_TYPE, "audio/mpeg");

    if (theHttpGs2200.post(HTTP_POST_PATH, NULL)) {
        while (mp3File.available()) {
            uint8_t buffer[512];
            size_t bytesRead = mp3File.read(buffer, sizeof(buffer));
            theHttpGs2200.read_data(buffer, bytesRead);  // データ送信
        }
        theHttpGs2200.end();
    }
    mp3File.close();
}

void loop() {
    sendMp3File();
    delay(10000);  // 10秒ごとに送信を試行
}
