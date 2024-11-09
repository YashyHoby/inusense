#include <WiFiS110.h>
#include <WiFiClient.h>
#include <SDHCI.h>
#include <File.h>

// Wi-Fi接続情報
const char* ssid = "Unchi";
const char* password = "ahoaho13";

// サーバ情報
const char* serverIP = "192.168.166.66"; // PCのIPアドレス
const int serverPort = 3000; // サーバのポート番号

WiFiClient client;

void setup() {
  Serial.begin(115200);

  // Wi-Fi接続
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Wi-Fiに接続中...");
  }
  Serial.println("Wi-Fiに接続しました。");

  // SDカードの初期化
  if (!SD.begin()) {
    Serial.println("SDカードの初期化に失敗しました。");
    return;
  }
}

void loop() {
  // MP3ファイルのパス
  const char* mp3FilePath = "/mnt/sd0/MP3/test.mp3";
  File mp3File = SD.open(mp3FilePath);

  if (!mp3File) {
    Serial.println("MP3ファイルのオープンに失敗しました。");
    return;
  }

  // サーバに接続
  if (client.connect(serverIP, serverPort)) {
    Serial.println("サーバに接続しました。");

    // MP3データの送信
    while (mp3File.available()) {
      byte buffer[1024];
      int bytesRead = mp3File.read(buffer, sizeof(buffer));
      client.write(buffer, bytesRead);
    }

    Serial.println("MP3ファイルの送信が完了しました。");
    client.stop();
  } else {
    Serial.println("サーバへの接続に失敗しました。");
  }

  mp3File.close();
  delay(10000); // 必要に応じて調整
}
