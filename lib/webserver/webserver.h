#ifndef WEBSERVER_H
#define WEBSERVER_H
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

void indexHandler(AsyncWebServerRequest * request);


#endif