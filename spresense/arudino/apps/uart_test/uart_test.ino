
void setup() {
  pinMode(2, INPUT);  // LSIが発音中か確認するピン（発音中はLOWになる）

  
  Serial.begin(9600);   // USBシリアル
  Serial2.begin(9600);  // ハードウェアUART (Serial2)

  delay(1000);
}

void loop() {
  /* PCからUSBシリアル経由で文字列を取得 */
  String response;
  Serial.println("Send something");
  while(1){
    if (Serial.available()) {
      response = Serial.readString();
      response.trim(); // 空白や改行を削除
      Serial.println(response); // 同じ文字を返答
      break;
    }
  }

  /* 文字列の長さを確認，「,」「.」まで，もしくは127バイトまで取り出しながら送信 */
  int maxLength = 125;
  int strLength = response.length();  // responseの長さを取得
  while(strLength > 0) {
    String subResponse;

    /* 最初に登場する「,」あるいは「.」を探してそこまで送信する */
    int commaIndex = response.indexOf(","); 
    int dotIndex = response.indexOf(".");
    if (commaIndex != -1 && commaIndex < maxLength) {
      subResponse = response.substring(0, commaIndex+1);
      response = response.substring(commaIndex+1);
    } else if (dotIndex != -1 && dotIndex < maxLength) {
      subResponse = response.substring(0, dotIndex+1);
      response = response.substring(dotIndex+1);
    }

    /* 「,」も「.」も見つからなかった場合，あるいはあってもそこまでに127文字を超えてしまう場合 */
    /* maxLength以前で最も最後に出てくる母音を探してそこまで送信 */
    else if (strLength > maxLength) {
      int vowelsIndex = maxLength;
      /* 127文字目手前の母音を探す */
      const char vowels[] = {'a', 'e', 'i', 'o', 'u'};
      for (int i = maxLength - 1; i >= 0; i--) {            // 終了位置から逆方向に検索
        char currentChar = toLowerCase(response.charAt(i)); // 一応小文字に変換して比較
        for (int j = 0; j < 5; j++) {                       // 母音リストをチェック
          if (currentChar == vowels[j]) {
            vowelsIndex = i;                                // 最初に見つけた位置（最後に現れる母音の位置）をvowelsIndexに代入して終了
            break;
          }
        }
        if (vowelsIndex == i) break;  // ネストを抜けるやり方がわかりませんでした．
      }

      subResponse = response.substring(0, vowelsIndex+1); // 送信する分の文字列を取り出してsubResponseに格納
      response = response.substring(vowelsIndex+1);       // 取り出した文字列を削除して詰める
    } else {
      subResponse = response.substring(0, strLength);   // 最後の文字列
      response = response.substring(strLength);
    }

    Serial2.print(subResponse); // 最初の文字列を送信
    Serial2.print("\r");        // 再生

    /* 送信が終わるまで待機 */
    delay(200);
    while(1){
      if (digitalRead(2) == 1){
        break;
      } else {
        Serial.println("busy");
        delay(100);
      } 
    }

    strLength = response.length();    // responseの長さを再度取得
  }

  // delay(100);
}
