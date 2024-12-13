
#include "GlobalVariables.h"
#include "AudioRecorderPlayer.h"
#include "HttpClient.h"
#include "TriggerWordRecognition.h"

#define RECORD_FILE_NAME "question.mp3"
#define SEND_FILE_NAME "test.mp3" // デバック用
#define RECEIVE_FILE_NAME "answer.txt"
#define PLAY_FILE_NAME "audio/Morning_10s.mp3"
#define CONSOLE_BAUDRATE  115200

#define ACTION_AUDIO "action.mp3"
#define ALERT_AUDIO "alert.mp3"

#define IRPIN A0  // 赤外線入力ピン

const char label[3][8] = {"itteki", "tadaima", "nene"}; // ラベル用テキスト

void setup()
{
  Serial.begin(CONSOLE_BAUDRATE);
  while (!Serial)
  {
    ; // シリアルポート接続待機
  }

  /* SDカード初期化 */
  while (!theSD.begin())
  {
    Serial.println("Insert SD card.");
  }

  // ハードウェアUART (Serial2) の初期化
  pinMode(2, INPUT);  // LSIが発音中か確認するピン（発音中はLOWになる）
  Serial2.begin(9600); // ハードウェアUART (Serial2)

  initialize_http();
  initialize_audio();
  initialization_TWR();
  

  delay(2000);
  Serial.println("Setup complete");
  Serial2.print("sextutoaxtupukanryo-.\r");
}

void loop()
{ 
  Serial.println("start main loop");

  //trigger_itteki();
  // trigger_tadaima();
  trigger_nene();
  //trigger_tadaima_fushinsya();
}


// 録音再生テスト
void test_recAndPlay_mp3(){
  start_recorder(RECORD_FILE_NAME);
  delay(10000);
  start_player(RECORD_FILE_NAME);
  delay(50000);
}



// 長文テキストを分割して再生
void speakText(String input){

  /* 万一のときのために「、。」を「,.」に変換する */
  String input2response; // 変換後の文字列を格納
  int strLength = input.length();  // inputの長さを取得
  for (int i = 0; i < strLength; i++) {
    // UTF-8の1文字目を取得
    char c = input.charAt(i);
    if (c == 0xE3 && i + 2 < strLength) { 
      // UTF-8マルチバイト文字（0xE3: 全角の「、」「。」の1バイト目）
      char c1 = input.charAt(i + 1); // 2バイト目
      char c2 = input.charAt(i + 2); // 3バイト目
      if (c1 == 0x80 && c2 == 0x81) { // 全角「、」（U+3001）
        input2response += ','; // 半角「,」に変換
        i += 2; // 次の文字へ（3バイト分スキップ）
        continue;
      } else if (c1 == 0x80 && c2 == 0x82) { // 全角「。」（U+3002）
        input2response += '.'; // 半角「.」に変換
        i += 2; // 次の文字へ（3バイト分スキップ）
        continue;
      }
    }
    // マルチバイト文字でない場合、そのまま追加
    input2response += c;
  }

  /* まんいちのときのために「'」を取り除く */
  input2response.replace("'",""); // アポストロフィを削除

  /* haをwaに変換する */
  String response2response = ""; // 結果文字列を格納
  strLength = input2response.length();
  for (int i = 0; i < strLength; i++) {
    if (i + 1 < strLength && input2response.substring(i, i + 2) == "ha") {
      // 次の文字が空白、','、または'.'の場合
      if (i + 2 == strLength || input2response.charAt(i + 2) == ' ' || input2response.charAt(i + 2) == ',' || input2response.charAt(i + 2) == '.') {
        response2response += "wa"; // "ha"を"wa"に変換
        i++; // "ha"を処理したので次の文字にスキップ
        continue;
      }
    }
    // 条件に該当しない場合はそのまま追加
    response2response += input2response.charAt(i);
  }

  String response = ""; // 結果の文字列を格納
  strLength = response2response.length();
  for (int i = 0; i < strLength; i++) {
    if (response2response.charAt(i) != ' ') { // 空白でない文字だけを追加
      response += response2response.charAt(i);
    }
  }

  /* 文字列の長さを確認，「,」「.」まで，もしくは127バイトまで取り出しながら送信 */
  int maxLength = 125;
  strLength = response.length();
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

    Serial.println(subResponse);
    Serial2.print(subResponse); // 最初の文字列を送信
    Serial2.print("\r");        // 再生

    /* 送信が終わるまで待機 */
    delay(200);
    while(1){
      if (digitalRead(2) == 1){
        break;
      } else {
        //Serial.println("busy");
        delay(100);
      } 
    }

    strLength = response.length();    // responseの長さを再度取得
  }
}

void IR_test(){
  while(true){
    Serial.println(analogRead(IRPIN));
    delay(500);
  }
}

// 赤外線
void wait_person_detection(){
  Serial.println("start person detection");
  while(true){
    if(analogRead(IRPIN) > 300){
      break;
    }
  } // 人物が検知されるまで待機
  Serial.println("person detected");
}

void conversation(){
  Serial.println("start conversation");

  while(true){
    // start_player(ACTION_AUDIO);
    start_recorder(RECORD_FILE_NAME);

    delay(100);
    // オーディオファイルを送信
    Serial.println("send audio to server");
    post_audioFile(RECORD_FILE_NAME);

    String response = get_text();
    if (response == "end"){
      break;
    }
    speakText(response);
  }

  Serial2.print("baibai.\r");
  Serial.println("end conversation");

}


void trigger_itteki(){
  // 行ってきます識別
  while(true){
    int triggerWord_index = triggerWordRecognition();

    if(triggerWord_index == 0){
      break;
    }
  }
  //delay(2000);
  Serial2.print("ixtuteraxtusyai.\r");
}

void trigger_tadaima(){
  delay(2000);
  wait_person_detection();

  Serial2.print("okaeri-.\r");
  // ただいま識別
  while(true){
    int triggerWord_index = triggerWordRecognition();

    if(triggerWord_index == 1){
      break;
    }
  }

  Serial2.print("waai\r");
}

void trigger_nene(){
  // ねえねえ識別
  while(true){
    int triggerWord_index = triggerWordRecognition();

    if(triggerWord_index == 2){
      break;
    }
  }
  delay(2000);

  Serial2.print("naani?\r");

  // またね，バイバイ等で会話の終わりを識別
  conversation();
}

void trigger_tadaima_fushinsya(){
  delay(2000);
  wait_person_detection();

  Serial2.print("okaeri-.\r");

  // ただいまではない応答
  delay(5000);

  Serial2.print("e\r");

  delay(500);

  post_requestLINE();
}