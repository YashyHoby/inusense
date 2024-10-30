#include <HttpGs2200.h>
#include <TelitWiFi.h>
#include <Arduino.h>
#include <SDHCI.h>
#include <File.h>
#include "HttpFileSender.h"
#include "config.h"

#define  CONSOLE_BAUDRATE  115200

SDClass SD;  /**< SDClass object */ 

File file;

typedef enum{
	POST=0,
	GET
} DEMO_STATUS_E;

DEMO_STATUS_E httpStat;

const uint16_t RECEIVE_PACKET_SIZE = 1500;
uint8_t Receive_Data[RECEIVE_PACKET_SIZE] = {0};

TelitWiFi gs2200;
TWIFI_Params gsparams;
HttpFileSender HttpFileSender(&gs2200);
HTTPGS2200_HostParams hostParams;

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
  HttpFileSender.begin(&hostParams);

	ConsoleLog("Start HTTP Client");
	/* Set HTTP Headers */

  HttpFileSender.config(HTTP_HEADER_AUTHORIZATION, "Basic dGVzdDp0ZXN0MTIz");
  HttpFileSender.config(HTTP_HEADER_TRANSFER_ENCODING, "chunked");
  HttpFileSender.config(HTTP_HEADER_CONTENT_TYPE, "application/x-www-form-urlencoded");
  HttpFileSender.config(HTTP_HEADER_HOST, HTTP_SRVR_IP);

	digitalWrite(LED0, HIGH); // turn on LED
  httpStat = POST;
}

void loop() {
	bool result = false;
  char* filePath = "data_to_send/nyan.jpg";
  switch(httpStat){
    case POST:
      file = SD.open(filePath, FILE_READ);
      HttpFileSender.sendFile(file);
      httpStat = GET;
      break;
    case GET:
      HttpFileSender.config(HTTP_HEADER_TRANSFER_ENCODING, "identity");
      result = HttpFileSender.get(HTTP_GET_PATH);
      if (true == result) {
        HttpFileSender.read_data(Receive_Data, RECEIVE_PACKET_SIZE);
        parse_httpresponse((char *)(Receive_Data));
      } else {
        ConsoleLog( "?? Unexpected HTTP Response ??" );
      }

      do {
        result = HttpFileSender.receive(5000);
        if (result) {
          HttpFileSender.read_data(Receive_Data, RECEIVE_PACKET_SIZE);
          ConsolePrintf("%s", (char *)(Receive_Data));
        } else {
          // AT+HTTPSEND command is done
          ConsolePrintf( "\r\n");
        }
      } while (result);

      result = HttpFileSender.end();
      delay(100000);
      break;
    default:
      break;
  }
}
