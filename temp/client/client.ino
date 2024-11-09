#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "Your_SSID";
const char* password = "Your_Password";
const char* serverUrl = "http://<PythonサーバーのIPアドレス>:<ポート番号>/upload";

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
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
                Serial.printf("File sent, response: %d\n", httpResponseCode);
            } else {
                Serial.printf("Error: %s\n", http.errorToString(httpResponseCode).c_str());
            }

            delete[] fileBuffer;
        } else {
            Serial.println("Failed to open file for reading");
        }

        http.end();
    }
    delay(10000);
}
