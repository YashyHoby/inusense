#include <HttpGs2200.h>
#include <TelitWiFi.h>
#include <Arduino.h>
#include <SDHCI.h>
#include <File.h>
#include "config.h"

#define  CONSOLE_BAUDRATE  115200

SDClass SD;  /**< SDClass object */ 

typedef enum{
	POST=0,
	GET
} DEMO_STATUS_E;

DEMO_STATUS_E httpStat = POST;

const uint16_t SEND_BUF_SIZE = 4096;

// char audioDataBin[SEND_BUF_SIZE];
// char audioDataStr[2*SEND_BUF_SIZE+1];
// char sendData[2*SEND_BUF_SIZE+1];

const uint16_t RECEIVE_PACKET_SIZE = 1500;
uint8_t Receive_Data[RECEIVE_PACKET_SIZE] = {0};

TelitWiFi gs2200;
TWIFI_Params gsparams;
HttpGs2200 theHttpGs2200(&gs2200);
HTTPGS2200_HostParams hostParams;

int sendFileToServer();

void parse_httpresponse(char *message)
{
	char *p;
	
	if ((p=strstr(message, "200 OK\r\n")) != NULL) {
		//ConsolePrintf("Response : %s\r\n", p+8);
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
  httpStat = POST;
}

int sendFileToServer(char* filePath){
	bool result = false;
  char dataBin[SEND_BUF_SIZE];
  char dataStr[2*SEND_BUF_SIZE+1];

  Serial.print("loading file...");
  File fileObj = SD.open(filePath, FILE_READ);
  
  Serial.print("filename > ");
  if (fileObj) {
    Serial.println(fileObj.name());

    //send
    unsigned long fileSize = fileObj.size();
    Serial.print("file size:");
    Serial.print(fileSize);
    Serial.println(" B");
    int maxSendTimes = (fileSize/sizeof(dataBin)) + 1;//postで送信する回数
    fileObj.seek(0);//set pointer to zero

    for(int send_i = 0;send_i<maxSendTimes;send_i++){
      Serial.print("loading & converting Data...");
      //ファイルから固定長データを順番に読み込み
      int end = sizeof(dataBin);
      // Serial.println("");
      if(send_i == maxSendTimes-1){
        for(int i=0;fileObj.available();i++){//最後のフレームよみこみ
          dataBin[i] = fileObj.read();
          end = i;
        }
        end += 1;
        httpStat = GET;
      }else{
        for(int i=0;i<sizeof(dataBin);i++){
          dataBin[i] = fileObj.read();
        }
      }
      theHttpGs2200.config(HTTP_HEADER_TRANSFER_ENCODING, "chunked");
      //create post data.
      
      //バイナリを文字(ASCII)に変換．(データサイズは倍になるが仕方ない...)
      for(int i=0;i<sizeof(dataBin);i++){
        int d1 = dataBin[i] >> 4;//0x??の上位桁.4bit右シフトして取り出す．
        int d2 = dataBin[i] & 15;//0x??の下位桁.ビットマスク(0b00001111)の積で取り出す．
        itoa(d1,&dataStr[i*2],16);
        itoa(d2,&dataStr[(i*2)+1],16);
      }
      dataStr[end*2] = '\0';//add Null string
      Serial.println("DONE!");

      Serial.print("Sending data ");
      Serial.print(send_i+1);
      Serial.print("/");
      Serial.print(maxSendTimes);
      Serial.print("...");
      result = theHttpGs2200.post(HTTP_POST_PATH, dataStr);
      Serial.println("DONE!");

      if (false == result) {
        break;
      }

      do {
        result = theHttpGs2200.receive(5000);
        if (result) {
          theHttpGs2200.read_data(Receive_Data, RECEIVE_PACKET_SIZE);
          // ConsolePrintf("%s", (char *)(Receive_Data));
        } else {
          // AT+HTTPSEND command is done
          // ConsolePrintf( "\r\n");
        }
      } while (result);

      result = theHttpGs2200.end();
      // delay(100);
      // httpStat = GET;
      // break;
    }
    /* Close the file */
    fileObj.close();
    //--------終了通知--------
    result = theHttpGs2200.post(HTTP_POST_PATH, "end");
    if (false == result) {
      // break;
      return -1;
    }

    do {
      result = theHttpGs2200.receive(5000);
      if (result) {
        theHttpGs2200.read_data(Receive_Data, RECEIVE_PACKET_SIZE);
        // ConsolePrintf("%s", (char *)(Receive_Data));
      } else {
        // AT+HTTPSEND command is done
        // ConsolePrintf( "\r\n");
      }
    } while (result);
    result = theHttpGs2200.end();
    return 0;
  } else {
    /* If the file didn't open, print an error */
    Serial.println("error opening 001.wav");
    delay(500);
    return -1;
  }  
}

void loop() {
	bool result = false;
	// static int count = 0;
  Serial.println("hoge");
  delay(1000);
  if(httpStat == POST){
    Serial.println("post");
    // File file = SD.open("voice_to_send/001.wav",FILE_READ);
    // sendFileToServer(file);
    sendFileToServer("voice_to_send/001.wav");
    Serial.println("post DONE!!");
    httpStat = GET;
  }else if(httpStat == GET){
    Serial.println("GET");
  }
  // switch(httpStat){
  //   case POST:
  //     httpStat = GET;
  //     break;

  //   case GET:
  //     theHttpGs2200.config(HTTP_HEADER_TRANSFER_ENCODING, "identity");

  //     result = theHttpGs2200.get(HTTP_GET_PATH);
  //     if (true == result) {
  //       theHttpGs2200.read_data(Receive_Data, RECEIVE_PACKET_SIZE);
  //       parse_httpresponse((char *)(Receive_Data));
  //     } else {
  //       ConsoleLog( "?? Unexpected HTTP Response ??" );
  //     }

  //     do {
  //       result = theHttpGs2200.receive(5000);
  //       if (result) {
  //         theHttpGs2200.read_data(Receive_Data, RECEIVE_PACKET_SIZE);
  //         ConsolePrintf("%s", (char *)(Receive_Data));
  //       } else {
  //         // AT+HTTPSEND command is done
  //         ConsolePrintf( "\r\n");
  //       }
  //     } while (result);
  //     result = theHttpGs2200.end();
  //     delay(100000);
  //     break;
  //   default:
  //     break;
  // }
}
