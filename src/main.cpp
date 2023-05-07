#include <Arduino.h>

#include <BLEMidi.h>
#include <avdweb_Switch.h>
#include <WiFi.h>
#include <WiFiClient.h>

#include "webDebug.h"

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
    webDebug.println("Connected to Tablet");
  }
  else
  {
    webDebug.println("Connected to Piano");
  }
  ledStrip.reset();
}

void onMidiDisconnect()
{
  ESP32reset();
}

bool connectToPiano()
{
  webDebug.println("Establishing Piano connection");
  ledStrip.reset();
  btStart();
  BLEMidiClient.begin("Piano Lights Client");
  int nDevices = BLEMidiClient.scan();
  if (nDevices > 0 && BLEMidiClient.connect(0))
  {
    webDebug.println("Piano connection established");
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
    webDebug.println("Piano connection failed");
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
  BLEMidiServer.begin("Piano Lights");

  BLEMidiServer.setOnConnectCallback(onMidiConnect);
  BLEMidiServer.setOnDisconnectCallback(onMidiDisconnect);

  BLEMidiServer.setNoteOnCallback(onNoteOn);
  BLEMidiServer.setNoteOffCallback(onNoteOff);

  webDebug.println("Intializing client");
  BLEMidiClient.setNoteOnCallback(onClientNoteOn);
  BLEMidiClient.setNoteOffCallback(onClientNoteOff);

  // BLEMidiClient.enableDebugging(); // Uncomment to see debugging messages from the library

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  webDebug.println(options.json());

  server.begin();
  webDebug.println("Connecting WiFi...");
}

bool WiFiConnected = false;

void loop()
{
  static unsigned long lastTime = millis();
  unsigned long time = millis();
  unsigned long elapsedTime = time - lastTime;

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
      webDebug.println("Next Pattern");
      showLedPattern(ledStrip.nextPattern());
    }

    if (tryToChangePattern != 0xFF)
    {
      showLedPattern(ledStrip.setPattern(tryToChangePattern));
      tryToChangePattern = 0xFF;
    }

    if (button.longPress() || tryToConnectToPiano)
    {
      webDebug.println("Client Connect");
      connectToPiano();
    }

  } else {
      ledStrip.setBrightness(128);
      if (tryToDisconnectFromPiano) { // If allready connected it means an http command to disconnect
        webDebug.println("MidiDisconnect");
        onMidiDisconnect();
      }
  }

  if (!WiFiConnected) 
  {
    webDebug.print(".");
  }

  if (!WiFiConnected && WiFi.isConnected())
  {
    WiFiConnected = true;
    webDebug.println("");
    webDebug.print("IP address: ");
    webDebug.println(WiFi.localIP().toString());
  } 

  lastTime = time;
}
