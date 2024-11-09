#include <HttpGs2200.h>
#include <TelitWiFi.h>
#include <Arduino.h>
#include <SDHCI.h>
#include <File.h>
#include "HttpFileSender.h"
#include "config.h"

#define CONSOLE_BAUDRATE 115200

SDClass SD;
File file;

const uint16_t RECEIVE_PACKET_SIZE = 4096; // パケットサイズを増加
uint8_t Receive_Data[RECEIVE_PACKET_SIZE] = {0};

TelitWiFi gs2200;
TWIFI_Params gsparams;
HttpFileSender HttpFileSender(&gs2200);
HTTPGS2200_HostParams hostParams;

void setup() {
    pinMode(LED0, OUTPUT);
    digitalWrite(LED0, LOW);
    Serial.begin(CONSOLE_BAUDRATE);
    while (!Serial) {
        ; // シリアルポートの接続待ち
    }

    // SDカードの初期化
    while (!SD.begin()) {
        ; // SDカードのマウント待ち
    }

    // GS2200の初期化
    Init_GS2200_SPI_type(iS110B_TypeC);
    gsparams.mode = ATCMD_MODE_STATION;
    gsparams.psave = ATCMD_PSAVE_DEFAULT;
    if (gs2200.begin(gsparams)) {
        Serial.println("GS2200の初期化に失敗しました");
        while (1);
    }

    // アクセスポイントへの接続
    if (gs2200.activate_station(AP_SSID, PASSPHRASE)) {
        Serial.println("アクセスポイントへの接続に失敗しました");
        while (1);
    }

    hostParams.host = (char *)HTTP_SRVR_IP;
    hostParams.port = (char *)HTTP_PORT;
    HttpFileSender.begin(&hostParams);

    Serial.println("HTTPクライアントを開始します");

    // HTTPヘッダーの設定
    HttpFileSender.config(HTTP_HEADER_AUTHORIZATION, "Basic dGVzdDp0ZXN0MTIz");
    HttpFileSender.config(HTTP_HEADER_CONTENT_TYPE, "application/x-www-form-urlencoded");
    HttpFileSender.config(HTTP_HEADER_HOST, HTTP_SRVR_IP);

    digitalWrite(LED0, HIGH);
}

void loop() {
    static bool isFileSent = false;
    if (!isFileSent) {
        const char* filePath = "audio/Morning_10s.mp3";
        file = SD.open(filePath, FILE_READ);
        if (!file) {
            Serial.println("ファイルのオープンに失敗しました");
            return;
        }

        // ファイルサイズの取得
        size_t fileSize = file.size();
        uint8_t* fileBuffer = (uint8_t*)malloc(fileSize);
        if (!fileBuffer) {
            Serial.println("メモリの確保に失敗しました");
            file.close();
            return;
        }

        // ファイル全体をバッファに読み込み
        file.read(fileBuffer, fileSize);
        file.close();

        // コンテンツ長を指定して送信
        HttpFileSender.config(HTTP_HEADER_CONTENT_LENGTH, String(fileSize).c_str());
        bool result = HttpFileSender.post(HTTP_POST_PATH, fileBuffer, fileSize);
        free(fileBuffer);

        if (result) {
            size_t len = HttpFileSender.read_data(Receive_Data, RECEIVE_PACKET_SIZE);
            Serial.write(Receive_Data, len);
        } else {
            Serial.println("HTTP POSTリクエストに失敗しました");
        }

        isFileSent = true;
    }

    // 必要に応じて他の処理をここに追加

    delay(1000); // 1秒待機
}
