#ifndef __INC_WEBLOG_H
#define __INC_WEBLOG_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

class WebLog
{
    std::vector<std::string> _log_lines;
    AsyncEventSource eventserver = AsyncEventSource("/logevent");
    Stream *pSerial;

public:
    WebLog();
    void init(AsyncWebServer &server);
    void init(Stream &serial);
    void init(AsyncWebServer &server, Stream &serial);
    void print(const char *text);
    void print(const std::string text);
    void print(const String text);
    void print(const IPAddress &ip);
    void print(const unsigned short int value);
    void println();
    void println(const char *text);
    void println(const std::string text);
    void println(const String text);
    void println(const IPAddress &ip);
    void println(const unsigned short int value);
    template <typename... Args>
    void printf(const char *format, Args... args)
    {
        int size_s = std::snprintf(nullptr, 0, format, args...) + 1;
        char buf[size_s];
        std::sprintf(buf, format, args...);
        print(buf);
    }
};
#endif
