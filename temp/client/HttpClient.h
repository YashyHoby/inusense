#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include "GlobalVariables.h"
#include <HttpGs2200.h>
#include <TelitWiFi.h>
#include "CustomHttpGs.h"
#include "config.h"

extern TelitWiFi gs2200;
extern TWIFI_Params gsparams;
extern CustomHttpGs theCustomHttpGs;
extern HTTPGS2200_HostParams hostParams;

typedef enum {
    POST = 0,
    GET
} DEMO_STATUS_E;

extern DEMO_STATUS_E httpStat;

void parse_httpresponse(char *message);
void initialize_http();
void handleHttpPost(const char* filePath);
void handleHttpGet(const char* saveFilePath);
void downloadAudioFile_timeout(const char* saveFilePath);
void downloadAudioFile_byteRead(const char* saveFilePath);
void downloadTextFile(const char* saveFileName);
void close_recorder();

#endif /* HTTPCLIENT_H */