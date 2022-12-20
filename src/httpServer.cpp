#include "httpServer.h"
#include "debug.h"
#include "options.h"

//#include <AsyncElegantOTA.h>
// #define SPIFFSEnable

#ifdef SPIFFSEnable
#include <SPIFFS.h>
#else
#include <pgmspace.h>

#include "page\favicon.h"
#include "page\index.html.h"
#include "page\main.js.h"
#include "page\polyfills.js.h"
#include "page\runtime.js.h"
#include "page\styles.css.h"
#endif

extern bool tryToConnectToPiano;
extern byte tryToChangePattern;

const byte ERROR = 255;

HttpServer::HttpServer()
{
#ifdef SPIFFSEnable
  SPIFFS.begin(false, "/spiffs", 64);
#endif

  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, POST, PUT");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Content-Type");

  webserver.on("/getDebug", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send(200, "text/json", String(httpDebug::getString())); });

  webserver.on("/getJSON", HTTP_GET, [](AsyncWebServerRequest *request)
               {
              httpDebug::print("getJSON: ");
              httpDebug::println(options.json());
              request->send(200, "text/json", options.json()); });

  webserver.on("/startMidi", HTTP_POST, [](AsyncWebServerRequest *request)
               {
              httpDebug::println("startMidi");
              tryToConnectToPiano = true;
              return request->send(200, "text/json", options.json()); });
  webserver.on("/setPattern", HTTP_POST, [](AsyncWebServerRequest *request)
               { 
              httpDebug::print("setPattern :");
              String message = String((int)ERROR);
              if (request->hasParam("pattern")) {
                message = request->getParam("pattern")->value();
              } else {
                httpDebug::println("ERROR");
                return request->send(400, "text/plain", "No pattern id");
              }
              byte id = message.toInt();
              tryToChangePattern = id;

              httpDebug::println(String(id).c_str());

              request->send(200, "text/json", options.json()); });
  webserver.on("/setSparkle", HTTP_POST, [](AsyncWebServerRequest *request)
               { 
              httpDebug::print("setSparkle: ");
              String message = "error";
              if (request->hasParam("sparkle")) {
                message = request->getParam("sparkle")->value();
              } else {
                httpDebug::println("ERROR");
                return request->send(400, "text/plain", "No value");
              }
              message.toLowerCase(); 
              options.setSparkle(message=="true");

              httpDebug::println(message.c_str());

              request->send(200, "text/json", options.json()); });
  webserver.on("/setBrightness", HTTP_POST, [](AsyncWebServerRequest *request)
               {
              httpDebug::print("setBrightness: ");
              String message = String((int)ERROR);
              
              if (request->hasParam("brightness")) {
                message = request->getParam("brightness")->value();
              } else {
                httpDebug::println("ERROR");
                request->send(400, "text/plain", "No value");
              }
              byte brightness = (byte)message.toInt();
              options.setBrightness(brightness);
              
              httpDebug::println(brightness);

              request->send(200, "text/json", options.json()); });
  webserver.on("/setColor", HTTP_POST, [](AsyncWebServerRequest *request)
               {
              httpDebug::print("setColor: ");
              
              String os = String((int)ERROR);

              String rs = "255";
              String gs = "255";
              String bs = "255";
              if (request->hasParam("o") && request->hasParam("r") && request->hasParam("g") && request->hasParam("b")) {
                os = request->getParam("o")->value();
                rs = request->getParam("r")->value();
                gs = request->getParam("g")->value();
                bs = request->getParam("b")->value();
              } else {
                httpDebug::println("ERROR");
                return request->send(400, "text/plain", "No value");
              }

              int o = (int)os.toInt();

              int r = (int)rs.toInt(); 
              int g = (int)gs.toInt(); 
              int b = (int)bs.toInt(); 

              options.setColor( o, r, g, b);

              request->send(200, "text/json", options.json()); });
#ifdef SPIFFSEnable
  webserver.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
#else
  webserver.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request)
               {
    AsyncWebServerResponse* response = request->beginResponse_P(200, "text/html", index_html, 1306);
         response->addHeader("Content-Encoding", "gzip");
         request->send(response); });

  webserver.on("/main.js", HTTP_GET, [](AsyncWebServerRequest *request)
               {
    AsyncWebServerResponse* response = request->beginResponse_P(200, "application/javascript", main_js,132885); //132885
         response->addHeader("Content-Encoding", "gzip");
         request->send(response); });

  webserver.on("/polyfills.js", HTTP_GET, [](AsyncWebServerRequest *request)
               {
    AsyncWebServerResponse* response = request->beginResponse_P(200, "application/javascript", polyfills_js, 11986);
         response->addHeader("Content-Encoding", "gzip");
         request->send(response); });

  webserver.on("/runtime.js", HTTP_GET, [](AsyncWebServerRequest *request)
               {
    AsyncWebServerResponse* response = request->beginResponse_P(200, "application/javascript", runtime_js, 646);
         response->addHeader("Content-Encoding", "gzip");
         request->send(response); });

  webserver.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request)
               {
    AsyncWebServerResponse* response = request->beginResponse_P(200, "text/css", styles_css, 17069);
         response->addHeader("Content-Encoding", "gzip");
         request->send(response); });

  webserver.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request)
               {
    AsyncWebServerResponse* response = request->beginResponse_P(200, "image/x-icon", favicon, 2520);
         response->addHeader("Content-Encoding", "gzip");
         request->send(response); });

#endif
  webserver.onNotFound([](AsyncWebServerRequest *request)
                       { request->redirect("index.html"); });
}

void HttpServer::begin()
{
  webserver.begin();
}
