#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SD.h>

const char* ssid = "Unchi";
const char* password = "ahoaho13";
const char* serverUrl = "http://192.168.166.66:3000/upload";

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("WiFiに接続中...");
    }
    Serial.println("WiFiに接続しました");

    if (!SD.begin()) {
        Serial.println("SDカードのマウントに失敗しました");
        return;
    }
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(serverUrl);
        http.addHeader("Content-Type", "audio/mpeg");

        File mp3File = SD.open("audio/Morning_10s.mp3", FILE_READ);
        if (mp3File) {
            int fileSize = mp3File.size();
            uint8_t* fileBuffer = new uint8_t[fileSize];
            mp3File.read(fileBuffer, fileSize);
            mp3File.close();

            int httpResponseCode = http.POST(fileBuffer, fileSize);
            if (httpResponseCode > 0) {
                Serial.printf("ファイルを送信しました。レスポンスコード: %d\n", httpResponseCode);
            } else {
                Serial.printf("送信エラー: %s\n", http.errorToString(httpResponseCode).c_str());
            }

            delete[] fileBuffer;
        } else {
            Serial.println("MP3ファイルの読み込みに失敗しました");
        }

        http.end();
    }
    delay(10000);
}
