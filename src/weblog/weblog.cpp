#include "weblog.h"
#include "loghtml.h"

#include "weblog\loghtml.h"
WebLog::WebLog()
{
    _log_lines.push_back("");
};

void WebLog::init(AsyncWebServer &server)
{
    pSerial = NULL;

    server.on("/log", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "text/html", loghtml); });

    eventserver.onConnect([this](AsyncEventSourceClient *client)
                          {
            for (unsigned long size = _log_lines.size()-1, i = 0; i<size; i++)
                client->send(_log_lines[i].c_str(),NULL,millis(),1000); });

    server.addHandler(&eventserver);
}
void WebLog::init(Stream &serial)
{
    pSerial = &serial;
};
void WebLog::init(AsyncWebServer &server, Stream &serial)
{
    pSerial = &serial;
    server.on("/log", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(200, "text/html", loghtml); });

    eventserver.onConnect([this](AsyncEventSourceClient *client)
                          {
            for (unsigned long size = _log_lines.size()-1, i = 0; i<size; i++)
                client->send(_log_lines[i].c_str(),NULL,millis(),1000); });

    server.addHandler(&eventserver);
}

void WebLog::print(const char *text)
{
    if (pSerial)
    {
        pSerial->print(text);
    }

    for (int i = 0; text[i] != '\0'; i++)
    {
        if (text[i] == '\n')
        {
            eventserver.send(_log_lines.back().c_str(), NULL, millis());
            _log_lines.push_back("");
        }
        else
        {
            _log_lines.back() += text[i];
        }
    }
}

void WebLog::print(const std::string text)
{
    print(text.c_str());
}

void WebLog::print(const String text)
{
    print(text.c_str());
}

void WebLog::print(const IPAddress &ip)
{
    print(ip.toString());
}

void WebLog::print(const unsigned short int value)
{
    print(std::to_string(value));
}

void WebLog::println()
{
    print("\n");
}

void WebLog::println(const char *text)
{
    print(text);
    println();
}

void WebLog::println(const std::string text)
{
    print(text);
    println();
}

void WebLog::println(const String text)
{
    print(text);
    println();
}

void WebLog::println(const IPAddress &ip)
{
    print(ip.toString());
    println();
}

void WebLog::println(const unsigned short int value)
{
    print(std::to_string(value));
    println();
}
