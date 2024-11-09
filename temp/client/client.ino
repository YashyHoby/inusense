#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>

#include "config.h"        // その他の設定用ファイル（IPアドレスなど）

const char* ssid = AP_SSID;
const char* password = PASSPHRASE;
const char* serverIp = HTTP_SRVR_IP; // PCのPythonサーバーのIPアドレス
const int serverPort = HTTP_PORT;          // Pythonサーバーで待機しているポート番号

WiFiClient client;

void setup() {
    Serial.begin(115200);
    while (!Serial) {}

    // WiFiに接続
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
}

void loop() {
    if (client.connect(serverIp, serverPort)) {
        Serial.println("Connected to server");

        // MP3ファイルの送信
        sendMP3File("/mnt/audio/Moning_10s.mp3");

        client.stop(); // 接続を終了
        delay(5000);   // 次の送信まで待機
    } else {
        Serial.println("Failed to connect to server");
    }
}

void sendMP3File(const char* filePath) {
    File mp3File = SPIFFS.open(filePath, "r");
    if (!mp3File) {
        Serial.println("Failed to open file");
        return;
    }

    // ファイルサイズを取得し、送信
    size_t fileSize = mp3File.size();
    client.write((uint8_t*)&fileSize, sizeof(fileSize)); // 先頭にファイルサイズを送信
    delay(10);

    // ファイルデータを送信
    uint8_t buffer[1024];
    while (mp3File.available()) {
        size_t bytesRead = mp3File.read(buffer, sizeof(buffer));
        client.write(buffer, bytesRead);
    }
    mp3File.close();
    Serial.println("File sent successfully");
}
