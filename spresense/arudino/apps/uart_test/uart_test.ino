void setup() {
  // USBシリアル（デバッグ用）
  Serial.begin(9600);
  while (!Serial) {
    // USBシリアル接続を待機
  }
  Serial.println("Spresense UART Communication Example");

  // ハードウェアUART (Serial2) の初期化
  Serial2.begin(9600); // ボーレートを9600bpsに設定

  delay(1000);

  Serial.println("plz send 1 to start talking");
  while(1){
    if (Serial.available()) {
      String receivedData = Serial.readString();
      receivedData.trim(); // 空白や改行を削除
      if (receivedData == "1") {
        break;
      }
    }
  }
  
}

void loop() {
  Serial2.print("ohayo-.\r");
  Serial.println("send OHAYOU to Aquestalk LSI");

  /* 再生が終わるまで待機 */
  while(1){
    if (Serial2.available()) {
      String receivedData = Serial2.readString(); // 受信データを文字列として読み取る
      //receivedData.trim(); // 空白や改行を削除
      if (receivedData == ">") {
        Serial.println("sound end & Aquestalk LSI ready");
        break;
      } else if (receivedData == "*") {
        Serial.println("ATP3011F4 busy");
      } else {
        Serial.print("unknown data received via UART1: ");
        Serial.println(receivedData);
      }
    }
    delay(1000);
    Serial.println("now talking...");
    Serial2.print("\r");  // 状態を確認[CR]
  }

  delay(1000); // 1秒待機

}
