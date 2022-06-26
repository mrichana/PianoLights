#include <Arduino.h>
#include <BLEMidi.h>
#include <avdweb_Switch.h>
//#include <BluetoothSerial.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>


#include "ledstrip.h"

AsyncWebServer server(80);
const char *ssid = "mrichana";
const char *password = "2106009557";
const char *PARAM_MESSAGE = "message";

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

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false);
  });

  // Send a GET request to <IP>/get?message=<message>
  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request)
            {
        String message;
        if (request->hasParam(PARAM_MESSAGE)) {
            message = request->getParam(PARAM_MESSAGE)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, GET: " + message); });

  // Send a POST request to <IP>/post with a form field message set to <message>
  server.on("/post", HTTP_POST, [](AsyncWebServerRequest *request)
            {
        String message;
        if (request->hasParam(PARAM_MESSAGE, true)) {
            message = request->getParam(PARAM_MESSAGE, true)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, POST: " + message); });

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

    // send the 'leds' array out to the actual LED strip
    // insert a delay to keep the framerate modest

    if (button.singleClick() == true)
    {
      Debug.println("Next Pattern");
      ledStrip.nextPattern();
    }
    if (button.longPress() == true)
    {
      Debug.println("Client Connect");
      int nDevices = BLEMidiClient.scan();
      // BLEMidiClient.getScannedDevice(0).
      if (nDevices > 0)
      {
        if (BLEMidiClient.connect(0))
          Debug.println("Piano connection established");
        else
        {
          Debug.println("Piano connection failed");
        }
      }
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
