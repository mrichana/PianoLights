#include <Arduino.h>

#include <BLEMidi.h>
#include <avdweb_Switch.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <ESPmDNS.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

#include "debug.h"

#include "options.h"

#include "ledstrip.h"

AsyncWebServer server(80);
const char *ssid = "mrichana";
const char *password = "2106009557";
const char *mDSNName = "pianolights";

bool tryToConnectToPiano = false;
byte tryToChangePattern = 0xFF;

void ESP32reset()
{
  ledStrip.reset();
  ESP.restart();
}

void notFound(AsyncWebServerRequest *request)
{
  if (request->method() == HTTP_OPTIONS)
  {
    request->send(200);
  }
  else
  {
    request->send(404, "application/json", "{\"message\":\"Not found\"}");
  }
}

#define BUTTON_PIN 18
Switch button = Switch(BUTTON_PIN, INPUT_PULLUP, false, 50, 500);

void onNoteOn(uint8_t channel, uint8_t note, uint8_t velocity, uint16_t timestamp)
{
  if (channel == 15)
  {
    ledStrip.ledOnFromNote(note);
  }
}

void onNoteOff(uint8_t channel, uint8_t note, uint8_t velocity, uint16_t timestamp)
{
  if (channel == 15)
  {
    ledStrip.ledOffFromNote(note);
  }
}

void onAfterTouchPoly(uint8_t channel, uint8_t note, uint8_t pressure, uint16_t timestamp)
{
}

void onControlChange(uint8_t channel, uint8_t controller, uint8_t value, uint16_t timestamp)
{
}

void onProgramChange(uint8_t channel, uint8_t program, uint16_t timestamp)
{
}

void onAfterTouch(uint8_t channel, uint8_t pressure, uint16_t timestamp)
{
}

void onPitchbend(uint8_t channel, uint16_t value, uint16_t timestamp)
{
}

void onClientNoteOn(uint8_t channel, uint8_t note, uint8_t velocity, uint16_t timestamp)
{
  ledStrip.ledOnFromNote(note, velocity);
}

void onClientNoteOff(uint8_t channel, uint8_t note, uint8_t velocity, uint16_t timestamp)
{
  ledStrip.ledOffFromNote(note, velocity);
}

void onMidiConnect()
{
  if (BLEMidiServer.isConnected())
  {
    httpDebug::println("Connected to Tablet");
  }
  else
  {
    httpDebug::println("Connected to Piano");
  }
  ledStrip.reset();
}

void onMidiDisconnect()
{
  ESP32reset();
}

bool connectToPiano()
{
  tryToConnectToPiano = false;
  ledStrip.reset();
  btStart();
  BLEMidiClient.begin("Piano Lights Client");
  int nDevices = BLEMidiClient.scan();
  if (nDevices > 0 && BLEMidiClient.connect(0))
  {
    httpDebug::println("Piano connection established");
    options.midiConnected = true;
    BLEMidiClient.setNoteOnCallback(onClientNoteOn);
    BLEMidiClient.setNoteOffCallback(onClientNoteOff);
    BLEMidiClient.setOnDisconnectCallback(onMidiDisconnect);
    ledStrip.ledOn(0, CRGB::Green);
    delay(200);
    ledStrip.ledOff(0);
    delay(200);
    ledStrip.ledOn(0, CRGB::Green);
    delay(200);
    ledStrip.ledOff(0);
    delay(200);
    ledStrip.ledOn(0, CRGB::Green);
    delay(200);
    ledStrip.ledOff(0);
  }
  else
  {
    httpDebug::println("Piano connection failed");
    options.midiConnected = false;
    ledStrip.ledOn(0, CRGB::Red);
    delay(200);
    ledStrip.ledOff(0);
    delay(200);
    ledStrip.ledOn(0, CRGB::Red);
    delay(200);
    ledStrip.ledOff(0);
    delay(200);
    ledStrip.ledOn(0, CRGB::Red);
    delay(200);
    ledStrip.ledOff(0);
    ESP32reset();
  }
  return options.midiConnected;
}

void showLedPattern(byte patternId)
{
  ledStrip.reset();
  ledStrip.ledOn(patternId + 1, CHSV(255 / (patternId + 1), 255, 255));
  delay(600);
  ledStrip.ledOff(patternId + 1);
  delay(200);
}

void setup()
{
  btStop();

  // httpDebug.begin("PianoLights Debug");
  // httpDebug.begin(115200);
  options.init();
  ledStrip.init();
  httpDebug::println(options.json());

  // BLEMidiServer.begin("Piano Lights");

  BLEMidiServer.setOnConnectCallback(onMidiConnect);
  BLEMidiServer.setOnDisconnectCallback(onMidiDisconnect);

  BLEMidiServer.setNoteOnCallback(onNoteOn);
  BLEMidiServer.setNoteOffCallback(onNoteOff);
  // BLEMidiServer.setAfterTouchPolyCallback(onAfterTouchPoly);
  // BLEMidiServer.setControlChangeCallback(onControlChange);
  // BLEMidiServer.setProgramChangeCallback(onProgramChange);
  // BLEMidiServer.setAfterTouchCallback(onAfterTouch);
  // BLEMidiServer.setPitchBendCallback(onPitchbend);

  httpDebug::println("Intializing client");
  BLEMidiClient.setNoteOnCallback(onClientNoteOn);
  BLEMidiClient.setNoteOffCallback(onClientNoteOff);

  //BLEMidiClient.enableDebugging(); // Uncomment to see debugging messages from the library

  SPIFFS.begin(false, "/spiffs", 64);
  File root = SPIFFS.open("/");
  // File file = root.openNextFile();
  // while (file)
  // {
  //   httpDebug::print("FILE: ");
  //   httpDebug::println(file.name());

  //   file = root.openNextFile();
  // }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, POST, PUT");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Content-Type");

  const byte ERROR = 255;

  server.on("/getDebug", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              request->send(200, "text/json", String(httpDebug::getString())); });
  
  server.on("/getJSON", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              httpDebug::print("getJSON: ");
              httpDebug::println(options.json());
              request->send(200, "text/json", options.json()); });

  server.on("/startMidi", HTTP_POST, [](AsyncWebServerRequest *request)
            {
              httpDebug::println("startMidi");
              tryToConnectToPiano = true;
              connectToPiano();
              return request->send(200, "text/json", options.json()); });
  server.on("/setPattern", HTTP_POST, [](AsyncWebServerRequest *request)
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
  server.on("/setSparkle", HTTP_POST, [](AsyncWebServerRequest *request)
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
  server.on("/setBrightness", HTTP_POST, [](AsyncWebServerRequest *request)
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

              ledStrip.setBrightness(brightness);

              request->send(200, "text/json", options.json()); });
  server.on("/setColor", HTTP_POST, [](AsyncWebServerRequest *request)
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

  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
  server.onNotFound(notFound);

  server.begin();
}

bool WiFiConnected = false;

void loop()
{
  unsigned long time = millis();
  button.poll();

  if (!BLEMidiServer.isConnected() && !BLEMidiClient.isConnected())
  {
    ledStrip.run();

    if (button.pushed())
    {
      httpDebug::println("Next Pattern");
      showLedPattern(ledStrip.nextPattern());
    }

    if (tryToChangePattern != 0xFF)
    {
      showLedPattern(ledStrip.setPattern(tryToChangePattern));
      tryToChangePattern = 0xFF;
    }

    if (button.longPress() || tryToConnectToPiano)
    {
      httpDebug::println("Client Connect");
      connectToPiano();
    }
    // do some periodic updates
  }

  if (!WiFiConnected && WiFi.isConnected())
  {
    WiFiConnected = true;
    httpDebug::print("IP address: ");
    httpDebug::println(WiFi.localIP());
    if (!MDNS.begin(mDSNName))
    {
      httpDebug::println("Error setting up mDNS");
    }
    else
    {
      httpDebug::print("mDNS: ");
      httpDebug::print(mDSNName);
      httpDebug::println(".local");
    }
  }
}
