#include <WiFiS110.h>
#include <HTTPClient.h>
#include "config.h"
#include "server_config.h"

const char* ssid = AP_SSID;
const char* password = PASSPHRASE;
const char* server = HTTP_SRVR_IP; // server_config.hで定義
const int port = HTTP_PORT;

WiFiClient wifiClient;
HTTPClient http;

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    // Wi-Fi接続
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("WiFi connected");

    // サーバーとの接続確認
    Serial.print("Connected to server: ");
    Serial.println(server);
}

void loop() {
    sendMP3Data();
    delay(5000); // データ送信間隔
}

void sendMP3Data() {
    // HTTP接続
    if (http.begin(wifiClient, String("http://") + server + ":" + port + "/upload")) {
        http.addHeader("Content-Type", "audio/mpeg");

        // MP3データの読み込み（例: SDカードから）
        File mp3File = SD.open("/audio/Moning_10s.mp3", FILE_READ);
        if (!mp3File) {
            Serial.println("Failed to open MP3 file");
            return;
        }

        int fileSize = mp3File.size();
        uint8_t buffer[512];
        
        // HTTP POSTを開始
        int httpResponseCode = http.sendRequest("POST", NULL, fileSize);
        if (httpResponseCode > 0) {
            while (mp3File.available()) {
                size_t len = mp3File.read(buffer, sizeof(buffer));
                http.write(buffer, len); // サーバにデータを送信
            }
            Serial.println("MP3 data sent successfully");
        } else {
            Serial.print("Error on sending POST: ");
            Serial.println(httpResponseCode);
        }

        http.end();
        mp3File.close();
    } else {
        Serial.println("Unable to connect to server");
    }
}
