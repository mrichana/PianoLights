#include <Arduino.h>
#include <BLEMidi.h>
#include <avdweb_Switch.h>
//#include <BluetoothSerial.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

#include "options.h"

#include "ledstrip.h"

AsyncWebServer server(80);
const char *ssid = "mrichana3g";
const char *password = "6972427823";

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

#define BUTTON_PIN 18
Switch button = Switch(BUTTON_PIN);
HardwareSerial Debug = Serial;

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
  ledStrip.reset();
  Debug.println("Disconnected");
  ESP.restart();
}

bool connectToPiano()
{
  int nDevices = BLEMidiClient.scan();
  // BLEMidiClient.getScannedDevice(0).
  if (nDevices = 0)
  {
    options.midiConnected = false;
    return options.midiConnected;
  }
  if (BLEMidiClient.connect(0))
  {
    Debug.println("Piano connection established");
    options.midiConnected = true;
  }
  else
  {
    Debug.println("Piano connection failed");
    options.midiConnected = false;
  }
  return options.midiConnected;
}

void setup()
{
  // Debug.begin("PianoLights Debug");
  Debug.begin(115200);
  Debug.println("Initializing bluetooth");
  BLEMidiServer.begin("Piano Lights");

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
  BLEMidiClient.begin("Piano Lights Client");
  BLEMidiClient.setNoteOnCallback(onClientNoteOn);
  BLEMidiClient.setNoteOffCallback(onClientNoteOff);

  // BLEMidiClient.enableDebugging(); // Uncomment to see debugging messages from the library

  SPIFFS.begin(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html", String(), false); });
  server.on("/startMidi", HTTP_POST, [](AsyncWebServerRequest *request)
            {
              connectToPiano();
              return request->send(200, "text/json", options.json()); });
  server.on("/setPattern", HTTP_POST, [](AsyncWebServerRequest *request) 
            { 
              String message;
              if (request->hasParam("pattern")) {
                message = request->getParam("pattern")->value();
              } else {
                request->send(400, "text/plain", "No pattern id");
              }
              byte id = message.toInt();
              ledStrip.setPattern(id); 

              request->send(200, "text/json", options.json());
            });
  server.on("/setSparkle", HTTP_POST, [](AsyncWebServerRequest *request) 
            { 
              String message;
              if (request->hasParam("sparkle")) {
                message = request->getParam("sparkle")->value();
              } else {
                request->send(400, "text/plain", "No value");
              }
              message.toLowerCase(); 
              options.sparkle = (message=="true");

              request->send(200, "text/json", options.json());
            });
  server.serveStatic("/", SPIFFS, "");

  server.onNotFound(notFound);

  server.begin();
}

// unsigned long clientNextTry = 0;
bool WiFiConnected = false;

void loop()
{
  unsigned long time = millis();
  button.poll();

  if (!BLEMidiServer.isConnected() && !BLEMidiClient.isConnected())
  {
    ledStrip.run();

    if (button.singleClick() == true)
    {
      Debug.println("Next Pattern");
      ledStrip.nextPattern();
    }
    if (button.longPress() == true)
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
  }
}
