#include <HttpGs2200.h>
#include <TelitWiFi.h>
#include <Arduino.h>
#include <SDHCI.h>
#include <File.h>
#include "config.h"

#define  CONSOLE_BAUDRATE  115200

SDClass SD;  /**< SDClass object */ 

File audioFile;

typedef enum{
	POST=0,
	GET
} DEMO_STATUS_E;

DEMO_STATUS_E httpStat;

const uint16_t SEND_BUF_SIZE = 100;

char sendData[SEND_BUF_SIZE];
char* audioDataBin;
// char* audioDataStr;
char audioDataStr[SEND_BUF_SIZE];

const uint16_t RECEIVE_PACKET_SIZE = 1500;
uint8_t Receive_Data[RECEIVE_PACKET_SIZE] = {0};

TelitWiFi gs2200;
TWIFI_Params gsparams;
HttpGs2200 theHttpGs2200(&gs2200);
HTTPGS2200_HostParams hostParams;

void parse_httpresponse(char *message)
{
	char *p;
	
	if ((p=strstr(message, "200 OK\r\n")) != NULL) {
		ConsolePrintf("Response : %s\r\n", p+8);
	}
}

void setup() {

	/* initialize digital pin LED_BUILTIN as an output. */
	pinMode(LED0, OUTPUT);
	digitalWrite(LED0, LOW);   // turn the LED off (LOW is the voltage level)
	Serial.begin(CONSOLE_BAUDRATE); // talk to PC
  while (!Serial) {
    ; /* wait for serial port to connect. Needed for native USB port only */
  }

    /* Initialize SD */
  Serial.print("Insert SD card.");
  while (!SD.begin()) {
    ; /* wait until SD card is mounted. */
  }
	/* Initialize SPI access of GS2200 */
	Init_GS2200_SPI_type(iS110B_TypeC);

	/* Initialize AT Command Library Buffer */
	gsparams.mode = ATCMD_MODE_STATION;
	gsparams.psave = ATCMD_PSAVE_DEFAULT;
	if (gs2200.begin(gsparams)) {
		ConsoleLog("GS2200 Initilization Fails");
		while (1);
	}

	/* GS2200 Association to AP */
	if (gs2200.activate_station(AP_SSID, PASSPHRASE)) {
		ConsoleLog("Association Fails");
		while (1);
	}

	hostParams.host = (char *)HTTP_SRVR_IP;
	hostParams.port = (char *)HTTP_PORT;
	theHttpGs2200.begin(&hostParams);

	ConsoleLog("Start HTTP Client");

	/* Set HTTP Headers */
	theHttpGs2200.config(HTTP_HEADER_AUTHORIZATION, "Basic dGVzdDp0ZXN0MTIz");
	theHttpGs2200.config(HTTP_HEADER_TRANSFER_ENCODING, "chunked");
	theHttpGs2200.config(HTTP_HEADER_CONTENT_TYPE, "application/x-www-form-urlencoded");
	theHttpGs2200.config(HTTP_HEADER_HOST, HTTP_SRVR_IP);

	digitalWrite(LED0, HIGH); // turn on LED

  audioFile = SD.open("voice_to_send/001.wav", FILE_READ);
  if (audioFile) {
    Serial.println("001.wav:");
    // int bufSize = sizeof(audioFile);
    unsigned long fileSize = audioFile.size();
    Serial.print("file size:");
    Serial.print(fileSize);
    Serial.println(" B");

    audioDataBin = (char*)calloc(fileSize,sizeof(char));
    
    //copy to array
    for(int i=0;i<fileSize;i++){
      audioDataBin[i] = audioFile.read();
    }
    audioFile.seek(0);//set pointer to zero

    //show array
    // for(int i=0;i<fileSize;i++){
    //   Serial.print(audioDataBin[i],HEX);
    //   Serial.print(",");
    // }
    // Serial.println();
    
    /* Read from the file until there's nothing else in it */
    // while (audioFile.available()) {
    //   // Serial.write(audioFile.read());
    //   Serial.print(audioFile.read(),HEX);
    //   Serial.print(",");
    // }
    // Serial.println();
    /* Close the file */
    audioFile.close();
  } else {
    /* If the file didn't open, print an error */
    Serial.println("error opening 001.wav");
  }

}

// バイナリデータを16進数文字列に変換する関数
// void binaryToHex(const char* binaryData, size_t dataLength, char* hexData) {
//     for (size_t i = 0; i < dataLength; i++) {
//         // 各バイトを16進数形式で2桁の文字列に変換
//         sprintf(&hexData[i * 2], "%02X", (unsigned char)binaryData[i]);
//         // itoa(binaryData[i],10);
//     }
// }

// the loop function runs over and over again forever
void loop() {
	httpStat = POST;
	bool result = false;
	// static int count = 0;

	while (1) {
		switch (httpStat) {
		case POST:
			theHttpGs2200.config(HTTP_HEADER_TRANSFER_ENCODING, "chunked");
			//create post data.
			// snprintf(sendData, sizeof(sendData), "data=%d", count);
			// snprintf(sendData, sizeof(sendData), "%d", audioDataBin[i]);//ここ，sendDataに100Byteずつコピーする実装にしたい
      
      //バイナリデータを文字列に変換(データサイズは倍になるが仕方ない...)
      // audioDataStr = (char*)calloc(sizeof(audioDataBin)*2+1,sizeof(char));//配列確保(+1は終端Null文字)
      // audioDataStr = (char*)calloc(50+1,sizeof(char));//配列確保(+1は終端Null文字)

      //Show Bin Array(first 10)
      Serial.print("BIN:");
      for(int i=0;i<10;i++){
        Serial.print(audioDataBin[i],HEX);
        Serial.print(",");
      }
      Serial.println();
      
      //binaryToHex(audioDataBin, sizeof(audioDataBin), audioDataStr);//データ変換&コピー

      //Show Str Array(first 10)
      Serial.print("ITA:");
      
      //バイナリを文字(ASCII)に変換．
      for(int i=0;i<101;i++){
        int d1 = audioDataBin[i] >> 4;//0x??の上位桁.4bit右シフトして取り出す．
        int d2 = audioDataBin[i] & 15;//0x??の下位桁.ビットマスク(0b00001111)の積で取り出す．
        Serial.print(d1);
        Serial.print("-");
        itoa(d1,&audioDataStr[i*2],16);
        Serial.print(d2);
        itoa(d2,&audioDataStr[(i*2)+1],16);
        Serial.print("->");
        Serial.print(audioDataStr[i*2],HEX);
        Serial.print(audioDataStr[i*2+1],HEX);
        Serial.print(",");
      }
      Serial.println();
      audioDataStr[sizeof(audioDataStr)] = '\0';//add Null string

      //Show String(ASCII) Array(first 10)
      Serial.print("STR:");
      for(int i=0;i<20;i++){
        Serial.print(audioDataStr[i],HEX);
        Serial.print(",");
      }
      Serial.println();

      memcpy(sendData,audioDataStr,SEND_BUF_SIZE);//送信バッファにコピー
      //送信バッファ表示
      Serial.print("sendData:");
      for(int i=0;i<SEND_BUF_SIZE;i++){
        Serial.print(sendData[i],HEX);
        Serial.print(",");
      }
      Serial.println();
			result = theHttpGs2200.post(HTTP_POST_PATH, sendData);
			//result = theHttpGs2200.post(HTTP_POST_PATH, audioDataBin);//ちょくでわたすのはむりだった(Byteだと0x00が登場した時点でNull文字と判断されてpostが終了される．strlen()でデータサイズを調べているライブラリの仕様がカス．)
			if (false == result) {
				break;
			}

			do {
				result = theHttpGs2200.receive(5000);
				if (result) {
					theHttpGs2200.read_data(Receive_Data, RECEIVE_PACKET_SIZE);
					ConsolePrintf("%s", (char *)(Receive_Data));
				} else {
					// AT+HTTPSEND command is done
					ConsolePrintf( "\r\n");
				}
			} while (result);

			result = theHttpGs2200.end();

			delay(1000);
			// count+=100;
			httpStat = GET;
			break;
			
		case GET:
			theHttpGs2200.config(HTTP_HEADER_TRANSFER_ENCODING, "identity");

			result = theHttpGs2200.get(HTTP_GET_PATH);
			if (true == result) {
				theHttpGs2200.read_data(Receive_Data, RECEIVE_PACKET_SIZE);
				parse_httpresponse((char *)(Receive_Data));
			} else {
				ConsoleLog( "?? Unexpected HTTP Response ??" );
			}

 			do {
				result = theHttpGs2200.receive(5000);
				if (result) {
					theHttpGs2200.read_data(Receive_Data, RECEIVE_PACKET_SIZE);
					ConsolePrintf("%s", (char *)(Receive_Data));
				} else {
					// AT+HTTPSEND command is done
					ConsolePrintf( "\r\n");
				}
			} while (result);

			result = theHttpGs2200.end();

			delay(20000);
			httpStat = POST;
			break;
		default:
			break;
		}
	}
  free(audioDataBin);
}
