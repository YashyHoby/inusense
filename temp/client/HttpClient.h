#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include "GlobalVariables.h"
#include <HttpGs2200.h>
#include <TelitWiFi.h>
#include "HttpFileSender.h"
#include "config.h"

extern TelitWiFi gs2200;
extern TWIFI_Params gsparams;
extern HttpFileSender httpFileSender;
extern HTTPGS2200_HostParams hostParams;

typedef enum {
    POST = 0,
    GET
} DEMO_STATUS_E;

extern DEMO_STATUS_E httpStat;

void parse_httpresponse(char *message);
void initialize_http();
void handleHttpPost(const char* filePath);
void handleHttpGet();

#endif /* HTTPCLIENT_H */