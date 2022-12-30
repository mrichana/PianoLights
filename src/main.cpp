#include <Arduino.h>

#include <BLEMidi.h>
#include <avdweb_Switch.h>
#include <WiFi.h>
#include <WiFiClient.h>

#include "debug.h"

#include "options.h"

#include "ledstrip.h"

#include "httpServer.h"

const char *ssid = "mrichana";
const char *password = "2106009557";
const char *mDSNName = "pianolights";

volatile bool tryToConnectToPiano = false;
volatile bool tryToDisconnectFromPiano = false;
volatile byte tryToChangePattern = 0xFF;

hw_timer_t *timer = NULL;

HttpServer server = HttpServer();

void ESP32reset()
{
  ledStrip.reset();
  ESP.restart();
}

#define BUTTON_PIN 18
Switch button = Switch(BUTTON_PIN);

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
  httpDebug::println("Establishing Piano connection");
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
    ledStrip.setBrightness(128);
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
  tryToConnectToPiano = false;
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
  options.init();
  ledStrip.init();
  httpDebug::println(options.json());

  // btStop();
  // BLEMidiServer.begin("Piano Lights");

  BLEMidiServer.setOnConnectCallback(onMidiConnect);
  BLEMidiServer.setOnDisconnectCallback(onMidiDisconnect);

  BLEMidiServer.setNoteOnCallback(onNoteOn);
  BLEMidiServer.setNoteOffCallback(onNoteOff);

  httpDebug::println("Intializing client");
  BLEMidiClient.setNoteOnCallback(onClientNoteOn);
  BLEMidiClient.setNoteOffCallback(onClientNoteOff);

  // BLEMidiClient.enableDebugging(); // Uncomment to see debugging messages from the library

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  server.begin();
}

bool WiFiConnected = false;

void loop()
{
  unsigned long time = millis();
  button.poll();

  static byte lastBrightness = 0;
  if (!BLEMidiServer.isConnected() && !BLEMidiClient.isConnected())
  {
    if (lastBrightness != options.getBrightness())
    {
      ledStrip.setBrightness(options.getBrightness());
      lastBrightness = options.getBrightness();
    }

    ledStrip.run();

    if (button.singleClick())
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

  } else {
      ledStrip.setBrightness(128);
      if (tryToDisconnectFromPiano) { // If allready connected it means an http command to disconnect
        onMidiDisconnect();
      }
  }

  if (!WiFiConnected && WiFi.isConnected())
  {
    WiFiConnected = true;
    httpDebug::print("IP address: ");
    httpDebug::println(WiFi.localIP().toString().c_str());
  }
}
