#ifndef __INC_SERVER_H
#define __INC_SERVER_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

class HttpServer {

    AsyncWebServer webserver =  AsyncWebServer(80);

    public:
    HttpServer();
    void begin();
};

#endif