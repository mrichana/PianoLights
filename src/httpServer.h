#ifndef __INC_SERVER_H
#define __INC_SERVER_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

class HttpServer {

    AsyncWebServer webserver =  AsyncWebServer(80);
    AsyncEventSource events = AsyncEventSource("/events");

    public:
    HttpServer();
    void begin();
    void sendEvent(const char*);
    void sendJSON();
};

extern HttpServer server;

#endif