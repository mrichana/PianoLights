#include <Arduino.h>
#include <NimBLEDevice.h>
#include <BLEMidi.h>

#include "ledstrip.h"

#include <WiFi.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>

#include "secrets.h"

#pragma region midi

void onTabletNoteOn(uint8_t channel, uint8_t note, uint8_t velocity, uint16_t timestamp)
{
  if (channel == 0)
  {
    ledStrip.ledOnFromNote(note, 127, LedStrip::LightType::rainbow);
  }
  else
  {
    BLEMidiClient.noteOn(channel, note, velocity);
  }
}

void onTabletNoteOff(uint8_t channel, uint8_t note, uint8_t velocity, uint16_t timestamp)
{
  if (channel == 0)
  {
    ledStrip.ledOffFromNote(note);
  }
  else
  {
    BLEMidiClient.noteOff(channel, note, velocity);
  }
}

void onPianoNoteOff(uint8_t channel, uint8_t note, uint8_t velocity, uint16_t timestamp)
{
  if (!BLEMidiServer.isConnected())
  {
    ledStrip.ledOffFromNote(note);
  }
  else
  {
    BLEMidiServer.noteOff(channel, note, velocity);
  }
}

void onPianoNoteOn(uint8_t channel, uint8_t note, uint8_t velocity, uint16_t timestamp)
{
  if (velocity == 0)
  {
    onPianoNoteOff(channel, note, velocity, timestamp);
    return;
  }
  if (!BLEMidiServer.isConnected())
  {
    ledStrip.ledOnFromNote(note, velocity, LedStrip::LightType::rainbow);
  }
  else
  {
    BLEMidiServer.noteOn(channel, note, velocity);
  }
}

void onControlChange(uint8_t channel, uint8_t controller, uint8_t value, uint16_t timestamp)
{
  if (BLEMidiServer.isConnected())
  {
    BLEMidiServer.controlChange(channel, controller, value);
  }
}

void onProgramChange(uint8_t channel, uint8_t progarm, uint16_t timestamp)
{
}

void onMidiConnect()
{
  if (BLEMidiClient.isConnected())
  {
  }
  if (BLEMidiServer.isConnected())
  {
    ledStrip.reset();
  }
}

void onMidiDisconnect()
{
  ledStrip.reset();
  esp_restart();
}

#pragma endregion

void setup()
{
  ledStrip.reset();
  ledStrip.setup();

  BLEMidiServer.begin("Piano Lights");

  BLEMidiServer.setOnConnectCallback(&onMidiConnect);
  BLEMidiServer.setOnDisconnectCallback(&onMidiDisconnect);

  BLEMidiServer.setNoteOnCallback(&onTabletNoteOn);
  BLEMidiServer.setNoteOffCallback(&onTabletNoteOff);
  BLEMidiServer.setControlChangeCallback(&onControlChange);
  BLEMidiServer.setProgramChangeCallback(&onProgramChange);

  BLEMidiClient.begin("Piano Lights Client");
  BLEMidiClient.setNoteOnCallback(&onPianoNoteOn);
  BLEMidiClient.setNoteOffCallback(&onPianoNoteOff);
  BLEMidiClient.setOnConnectCallback(&onMidiConnect);
  BLEMidiClient.setOnDisconnectCallback(&onMidiDisconnect);

  BLEMidiClient.backgroundScan(PIANO);

  WiFi.begin(SSID, PASWD);
}

void loop()
{
  static bool connected = false;
  ledStrip.loop();
  if (!connected)
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      connected = true;
      MDNS.begin(mDNS);
      MDNS.addService("http", "tcp", 80);

      ArduinoOTA.begin();
    }
  }
  else
  {
    ArduinoOTA.handle();
  }
}
