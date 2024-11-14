#include "HttpClient.h"

TelitWiFi gs2200;
TWIFI_Params gsparams;
CustomHttpGs theCustomHttpGs(&gs2200);
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
    theCustomHttpGs.begin(&hostParams);

    ConsoleLog("Start HTTP Client");

    theCustomHttpGs.config(HTTP_HEADER_AUTHORIZATION, "Basic dGVzdDp0ZXN0MTIz");
    theCustomHttpGs.config(HTTP_HEADER_TRANSFER_ENCODING, "chunked");
    theCustomHttpGs.config(HTTP_HEADER_CONTENT_TYPE, "application/x-www-form-urlencoded");
    theCustomHttpGs.config(HTTP_HEADER_HOST, HTTP_SRVR_IP);

    digitalWrite(LED0, HIGH);
    httpStat = POST;
}

void handleHttpPost(const char* filePath) {
    //const char* filePath = "audio/Morning_10s.mp3";
    File myFile = theSD.open(filePath, FILE_READ);
    theCustomHttpGs.sendFile(myFile);
    httpStat = GET;
    myFile.close();
}

/*
void handleHttpGet(const char* saveFileName) {
    Serial.print("get audio from server");
    const uint16_t RECEIVE_PACKET_SIZE = 1500;
    uint8_t Receive_Data[RECEIVE_PACKET_SIZE] = {0};
    bool result = false;

    if (theSD.exists(saveFileName))
    {
        Serial.print("Remove existing file [");
        Serial.print(saveFileName);
        Serial.println("].");
        theSD.remove(saveFileName);
    }

    // SDカードにファイルを作成
    myFile = theSD.open(saveFileName, FILE_WRITE);

    if (!myFile) {
        Serial.println("File open error");
        return;
    }

    theCustomHttpGs.config(HTTP_HEADER_TRANSFER_ENCODING, "identity");
    result = theCustomHttpGs.get(HTTP_GET_PATH);
    Serial.println(result);
    if (!result) {
        Serial.println("HTTP GET request failed.");
        myFile.close();
        return;
    }

    do {
        Serial.println("unchi");
        result = theCustomHttpGs.receive(5000);
        Serial.println(result);
        if (result) {
            theCustomHttpGs.read_data(Receive_Data, RECEIVE_PACKET_SIZE);
            ConsolePrintf("%s", (char *)(Receive_Data));
        } else {
            ConsolePrintf("\r\n");
        }
    } while (result);
    Serial.println("dd");
    myFile.close();//
    theCustomHttpGs.end();
}*/


void downloadAudioFile(const char* saveFileName) {
    const uint16_t RECEIVE_PACKET_SIZE = 1500;
    uint8_t buffer[RECEIVE_PACKET_SIZE];
    bool result = false;

    if (theSD.exists(saveFileName))
    {
        Serial.print("Remove existing file [");
        Serial.print(saveFileName);
        Serial.println("].");
        theSD.remove(saveFileName);
    }

    // SDカードにファイルを作成
    File myFile = theSD.open(saveFileName, FILE_WRITE);
    if (!myFile) {
        Serial.println("File open error");
    }

    result = theCustomHttpGs.get(HTTP_GET_PATH);

    if (!result) {
        Serial.println("Failed to send GET request");
        theCustomHttpGs.end();
        myFile.close();
        return;
    }


    Serial.println("HTTP GET request sent");

    

    do {
        result = theCustomHttpGs.receive(2000);  // タイムアウトを設定
        if (result) {
            theCustomHttpGs.read_data(buffer, RECEIVE_PACKET_SIZE);  // 受信したデータをバッファに格納
            myFile.write(buffer, RECEIVE_PACKET_SIZE);  // バッファ内容をSDカードに書き込み
            /* // バイナリで出力
            for (int i = 0; i < RECEIVE_PACKET_SIZE; i++) {
                Serial.print(buffer[i], HEX);
                Serial.print(" ");
            } */
            Serial.println("loading..");
            usleep(40000);
        }
        
    } while (result);


    Serial.println("adaf");
    theCustomHttpGs.end();
    Serial.println("dddddd");
    myFile.close();
}