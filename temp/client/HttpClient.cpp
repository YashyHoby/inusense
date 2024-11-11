#include "HttpClient.h"

TelitWiFi gs2200;
TWIFI_Params gsparams;
HttpFileSender httpFileSender(&gs2200);
HTTPGS2200_HostParams hostParams;
DEMO_STATUS_E httpStat;

void parse_httpresponse(char *message) {
    char *p;
    if ((p = strstr(message, "200 OK\r\n")) != NULL) {
        // 必要に応じてレスポンスの処理を追加
    }
}

void initialize_http() {
    Init_GS2200_SPI_type(iS110B_TypeC);

    gsparams.mode = ATCMD_MODE_STATION;
    gsparams.psave = ATCMD_PSAVE_DEFAULT;
    if (gs2200.begin(gsparams)) {
        ConsoleLog("GS2200 Initialization Failed");
        while (1);
    }

    if (gs2200.activate_station(AP_SSID, PASSPHRASE)) {
        ConsoleLog("Association Failed");
        while (1);
    }

    hostParams.host = (char *)HTTP_SRVR_IP;
    hostParams.port = (char *)HTTP_PORT;
    httpFileSender.begin(&hostParams);

    ConsoleLog("Start HTTP Client");

    httpFileSender.config(HTTP_HEADER_AUTHORIZATION, "Basic dGVzdDp0ZXN0MTIz");
    httpFileSender.config(HTTP_HEADER_TRANSFER_ENCODING, "chunked");
    httpFileSender.config(HTTP_HEADER_CONTENT_TYPE, "application/x-www-form-urlencoded");
    httpFileSender.config(HTTP_HEADER_HOST, HTTP_SRVR_IP);

    digitalWrite(LED0, HIGH);
    httpStat = POST;
}

void handleHttpPost(const char* filePath) {
    //const char* filePath = "audio/Morning_10s.mp3";
    myFile = theSD.open(filePath, FILE_READ);
    httpFileSender.sendFile(myFile);
    httpStat = GET;
}

void handleHttpGet() {
    const uint16_t RECEIVE_PACKET_SIZE = 1500;
    uint8_t Receive_Data[RECEIVE_PACKET_SIZE] = {0};
    bool result = false;

    httpFileSender.config(HTTP_HEADER_TRANSFER_ENCODING, "identity");
    result = httpFileSender.get(HTTP_GET_PATH);
    if (result) {
        httpFileSender.read_data(Receive_Data, RECEIVE_PACKET_SIZE);
        parse_httpresponse((char *)(Receive_Data));
    } else {
        ConsoleLog("Unexpected HTTP Response");
    }

    do {
        result = httpFileSender.receive(5000);
        if (result) {
            httpFileSender.read_data(Receive_Data, RECEIVE_PACKET_SIZE);
            ConsolePrintf("%s", (char *)(Receive_Data));
        } else {
            ConsolePrintf("\r\n");
        }
    } while (result);

    httpFileSender.end();
    delay(100000);
}