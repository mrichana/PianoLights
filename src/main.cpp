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

void ESP32reset()
{
  ledStrip.reset();
  ESP.restart();
}

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

#define BUTTON_PIN 18
Switch button = Switch(BUTTON_PIN);

void onNoteOn(uint8_t channel, uint8_t note, uint8_t velocity, uint16_t timestamp)
{
  Debug.printf("Note on : channel %d, note %d, velocity %d (timestamp %dms)\n", channel, note, velocity, timestamp);
  if (channel == 15)
  {
    ledStrip.ledOnFromNote(note);
  }
}

void onNoteOff(uint8_t channel, uint8_t note, uint8_t velocity, uint16_t timestamp)
{
  Debug.printf("Note off : channel %d, note %d, velocity %d (timestamp %dms)\n", channel, note, velocity, timestamp);
  if (channel == 15)
  {
    ledStrip.ledOffFromNote(note);
  }
}

void onAfterTouchPoly(uint8_t channel, uint8_t note, uint8_t pressure, uint16_t timestamp)
{
  Debug.printf("Polyphonic after touch : channel %d, note %d, pressure %d (timestamp %dms)\n", channel, note, pressure, timestamp);
}

void onControlChange(uint8_t channel, uint8_t controller, uint8_t value, uint16_t timestamp)
{
  Debug.printf("Control change : channel %d, controller %d, value %d (timestamp %dms)\n", channel, controller, value, timestamp);
}

void onProgramChange(uint8_t channel, uint8_t program, uint16_t timestamp)
{
  Debug.printf("Program change : channel %d, program %d (timestamp %dms)\n", channel, program, timestamp);
}

void onAfterTouch(uint8_t channel, uint8_t pressure, uint16_t timestamp)
{
  Debug.printf("After touch : channel %d, pressure %d (timestamp %dms)\n", channel, pressure, timestamp);
}

void onPitchbend(uint8_t channel, uint16_t value, uint16_t timestamp)
{
  Debug.printf("Pitch bend : channel %d, value %d (timestamp %dms)\n", channel, value, timestamp);
}

void onClientNoteOn(uint8_t channel, uint8_t note, uint8_t velocity, uint16_t timestamp)
{
  Debug.printf("Note on : channel %d, note %d, velocity %d (timestamp %dms)\n", channel, note, velocity, timestamp);
  ledStrip.ledOnFromNote(note, velocity);
}

void onClientNoteOff(uint8_t channel, uint8_t note, uint8_t velocity, uint16_t timestamp)
{
  Debug.printf("Note off : channel %d, note %d, velocity %d (timestamp %dms)\n", channel, note, velocity, timestamp);
  ledStrip.ledOffFromNote(note, velocity);
}

void onMidiConnect()
{
  if (BLEMidiServer.isConnected())
  {
    Debug.println("Connected to Tablet");
  }
  else
  {
    Debug.println("Connected to Piano");
  }
  ledStrip.reset();
}

void onMidiDisconnect()
{
  ESP32reset();
}

bool connectToPiano()
{
  ledStrip.reset();
  btStart();
  BLEMidiClient.begin("Piano Lights Client");
  int nDevices = BLEMidiClient.scan();
  if (nDevices > 0 && BLEMidiClient.connect(0))
  {
    Debug.println("Piano connection established");
    options.midiConnected = true;
    BLEMidiClient.setNoteOnCallback(onClientNoteOn);
    BLEMidiClient.setNoteOffCallback(onClientNoteOff);
    BLEMidiClient.setOnDisconnectCallback(onMidiDisconnect);
    ledStrip.ledOn(0, 0, 255, 0);
    delay(200);
    ledStrip.ledOff(0);
    delay(200);
    ledStrip.ledOn(0, 0, 255, 0);
    delay(200);
    ledStrip.ledOff(0);
    delay(200);
    ledStrip.ledOn(0, 0, 255, 0);
    delay(200);
    ledStrip.ledOff(0);
  }
  else
  {
    Debug.println("Piano connection failed");
    options.midiConnected = false;
    ledStrip.ledOn(0, 255, 0, 0);
    delay(200);
    ledStrip.ledOff(0);
    delay(200);
    ledStrip.ledOn(0, 255, 0, 0);
    delay(200);
    ledStrip.ledOff(0);
    delay(200);
    ledStrip.ledOn(0, 255, 0, 0);
    delay(200);
    ledStrip.ledOff(0);
    ESP32reset();
   }
  return options.midiConnected;
}

void setup()
{
  btStop();

  // Debug.begin("PianoLights Debug");
  Debug.begin(115200);
  options.init();
  ledStrip.init();
  Debug.println(options.json());

  //BLEMidiServer.begin("Piano Lights");

  BLEMidiServer.setOnConnectCallback(onMidiConnect);
  BLEMidiServer.setOnDisconnectCallback(onMidiDisconnect);

  BLEMidiServer.setNoteOnCallback(onNoteOn);
  BLEMidiServer.setNoteOffCallback(onNoteOff);
  // BLEMidiServer.setAfterTouchPolyCallback(onAfterTouchPoly);
  // BLEMidiServer.setControlChangeCallback(onControlChange);
  // BLEMidiServer.setProgramChangeCallback(onProgramChange);
  // BLEMidiServer.setAfterTouchCallback(onAfterTouch);
  // BLEMidiServer.setPitchBendCallback(onPitchbend);
  
  Debug.println("Intializing client");
  BLEMidiClient.setNoteOnCallback(onClientNoteOn);
  BLEMidiClient.setNoteOffCallback(onClientNoteOff);

  BLEMidiClient.enableDebugging(); // Uncomment to see debugging messages from the library

  SPIFFS.begin(false, "/spiffs", 64);
//   File root = SPIFFS.open("/");
//   File file = root.openNextFile();
//   while(file){
 
//       Debug.print("FILE: ");
//       Debug.println(file.name());
 
//       file = root.openNextFile();
// }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  server.on("/getJSON", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              Debug.print("getJSON: ");
              Debug.println(options.json());
              request->send(200, "text/json", options.json());
            });
  server.on("/startMidi", HTTP_POST, [](AsyncWebServerRequest *request)
            {
              Debug.write("startMidi");
              connectToPiano();
              return request->send(200, "text/json", options.json()); });
  server.on("/setPattern", HTTP_POST, [](AsyncWebServerRequest *request)
            { 
              Debug.write("setPattern");
              String message;
              if (request->hasParam("pattern")) {
                message = request->getParam("pattern")->value();
              } else {
                request->send(400, "text/plain", "No pattern id");
              }
              byte id = message.toInt();
              ledStrip.setPattern(id); 

              request->send(200, "text/json", options.json()); });
  server.on("/setSparkle", HTTP_POST, [](AsyncWebServerRequest *request)
            { 
              Debug.write("setSparkle");
              String message;
              if (request->hasParam("sparkle")) {
                message = request->getParam("sparkle")->value();
              } else {
                request->send(400, "text/plain", "No value");
              }
              message.toLowerCase(); 
              options.setSparkle(message=="true");

              request->send(200, "text/json", options.json()); });
  server.on("/setColor", HTTP_POST, [](AsyncWebServerRequest *request) 
            {
              Debug.write("setColor");
              
              char buffer[20];

              String rs;
              String gs;
              String bs;
              if (request->hasParam("r") && request->hasParam("g") && request->hasParam("b")) {
                rs = request->getParam("r")->value();
                gs = request->getParam("g")->value();
                bs = request->getParam("b")->value();
              } else {
                request->send(400, "text/plain", "No value");
              }

              int r = (int)rs.toInt(); 
              int g = (int)gs.toInt(); 
              int b = (int)bs.toInt(); 

              options.setColor( r, g, b);

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

    if (button.singleClick())
    {
      Debug.println("Next Pattern");
      ledStrip.nextPattern();
    }
    if (button.longPress())
    {
      Debug.println("Client Connect");
      connectToPiano();
    }
    // do some periodic updates
  }

  if (!WiFiConnected && WiFi.isConnected())
  {
    WiFiConnected = true;
    Debug.print("IP address: ");
    Debug.println(WiFi.localIP());
    if (!MDNS.begin(mDSNName)) {
      Debug.println("Error setting up mDNS");
    } else {
      Debug.print("mDNS: ");
      Debug.print(mDSNName);
      Debug.println(".local");
    }
  }
}
