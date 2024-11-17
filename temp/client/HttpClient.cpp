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
    File myFile = theSD.open(saveFileName, FILE_WRITE);

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
}

// bool HttpGs2200::receive(uint64_t timeout) を利用
void downloadAudioFile_timeout(const char* saveFileName) {
    const int RECEIVE_PACKET_SIZE = 10;
    uint8_t buffer[RECEIVE_PACKET_SIZE];
    int result = 0;

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

    theCustomHttpGs.config(HTTP_HEADER_TRANSFER_ENCODING, "identity");
    result = theCustomHttpGs.get(HTTP_GET_PATH);

    if (!result) {
        Serial.println("Failed to send GET request");
        theCustomHttpGs.end();
        myFile.close();
        return;
    }


    Serial.println("HTTP GET request sent");


    while (true){
        result = theCustomHttpGs.receive(5000);

        if (result) {
            theCustomHttpGs.read_data(buffer, RECEIVE_PACKET_SIZE);  // 受信したデータをバッファに格納
            myFile.write(buffer, RECEIVE_PACKET_SIZE);  // バッファ内容をSDカードに書き込み
            // バイナリで出力
            for (int i = 0; i < RECEIVE_PACKET_SIZE; i++) {
                Serial.print(buffer[i], HEX);
                Serial.print(" ");
            }
            Serial.println("loading..");
        } else {
            Serial.println("End");
            break;
        }
        
    }


    Serial.println("adaf");
    theCustomHttpGs.end();
    Serial.println("dddddd");
    myFile.close();
}

// bool HttpGs2200::receive(uint64_t timeout) を利用
void LINE_req() {
    const int RECEIVE_PACKET_SIZE = 10;
    uint8_t buffer[RECEIVE_PACKET_SIZE];
    int result = 0;

    theCustomHttpGs.config(HTTP_HEADER_TRANSFER_ENCODING, "identity");
    result = theCustomHttpGs.get(HTTP_LINE_PATH);

    if (!result) {
        Serial.println("Failed to send GET request");
        theCustomHttpGs.end();
        //myFile.close();
        return;
    }


    Serial.println("HTTP GET request sent");

    Serial.println("adaf");
    theCustomHttpGs.end();
    Serial.println("dddddd");
}



// int HttpGs2200::receive(uint8_t* data, int length) を利用
void downloadAudioFile_byteRead(const char* saveFileName) {
    const int RECEIVE_PACKET_SIZE = 1500;
    uint8_t buffer[RECEIVE_PACKET_SIZE];
    int bytesRead;
    bool result;

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
        return;
    }

    // HTTP GETリクエストの送信
    result = theCustomHttpGs.get(HTTP_GET_PATH);
    if (!result) {
        Serial.println("Failed to send GET request");
        theCustomHttpGs.end();
        myFile.close();
        return;
    }
    Serial.println("HTTP GET request sent");

    // データの受信ループ
    while (true) {
        bytesRead = theCustomHttpGs.receive(buffer, RECEIVE_PACKET_SIZE);
        if (bytesRead < 0) {
            Serial.println("Error: Failed to receive data.");
            break;
        } else if (bytesRead == 0) {
            // データの受信が完了した場合
            Serial.println("All data received.");
            break;
        } else {
            // 受信したデータをファイルに書き込み
            myFile.write(buffer, bytesRead);
        }
        usleep(10000);
    }

    // 終了処理
    theCustomHttpGs.end();
    myFile.close();
}


//　テキストデータ受信
void downloadTextFile(const char* saveFileName) {
    const int RECEIVE_PACKET_SIZE = 10;
    uint8_t buffer[RECEIVE_PACKET_SIZE];
    int result = 0;

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

    theCustomHttpGs.config(HTTP_HEADER_TRANSFER_ENCODING, "identity");
    result = theCustomHttpGs.get(HTTP_GET_PATH);

    if (!result) {
        Serial.println("Failed to send GET request");
        theCustomHttpGs.end();
        myFile.close();
        return;
    }


    Serial.println("HTTP GET request sent");


    while (true){
        result = theCustomHttpGs.receive(20000);
        
        if (result) {
            theCustomHttpGs.read_data(buffer, RECEIVE_PACKET_SIZE);  // 受信したデータをバッファに格納
            myFile.write(buffer, RECEIVE_PACKET_SIZE);  // バッファ内容をSDカードに書き込み
            // バイナリで出力
            for (int i = 0; i < RECEIVE_PACKET_SIZE; i++) {
                Serial.print((char)buffer[i], HEX);
                Serial.print(" ");
            }
            Serial.println("loading..");
        } else {
            Serial.println("End");
            break;
        }
        
    }


    theCustomHttpGs.end();
    myFile.close();
}
