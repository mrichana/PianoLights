#include <Arduino.h>
#include <BLEMidi.h>

#include "ledstrip.h"

#include <avdweb_Switch.h>

#define BUTTON_PIN 18
Switch button = Switch(BUTTON_PIN);

void onNoteOn(uint8_t channel, uint8_t note, uint8_t velocity, uint16_t timestamp)
{
  Serial.printf("Note on : channel %d, note %d, velocity %d (timestamp %dms)\n", channel, note, velocity, timestamp);
  if (channel == 15)
  {
    ledStrip.ledOnFromNote(note);
  }
}

void onNoteOff(uint8_t channel, uint8_t note, uint8_t velocity, uint16_t timestamp)
{
  Serial.printf("Note off : channel %d, note %d, velocity %d (timestamp %dms)\n", channel, note, velocity, timestamp);
  if (channel == 15)
  {
    ledStrip.ledOffFromNote(note);
  }
}

void onAfterTouchPoly(uint8_t channel, uint8_t note, uint8_t pressure, uint16_t timestamp)
{
  Serial.printf("Polyphonic after touch : channel %d, note %d, pressure %d (timestamp %dms)\n", channel, note, pressure, timestamp);
}

void onControlChange(uint8_t channel, uint8_t controller, uint8_t value, uint16_t timestamp)
{
  Serial.printf("Control change : channel %d, controller %d, value %d (timestamp %dms)\n", channel, controller, value, timestamp);
}

void onProgramChange(uint8_t channel, uint8_t program, uint16_t timestamp)
{
  Serial.printf("Program change : channel %d, program %d (timestamp %dms)\n", channel, program, timestamp);
}

void onAfterTouch(uint8_t channel, uint8_t pressure, uint16_t timestamp)
{
  Serial.printf("After touch : channel %d, pressure %d (timestamp %dms)\n", channel, pressure, timestamp);
}

void onPitchbend(uint8_t channel, uint16_t value, uint16_t timestamp)
{
  Serial.printf("Pitch bend : channel %d, value %d (timestamp %dms)\n", channel, value, timestamp);
}

void onClientNoteOn(uint8_t channel, uint8_t note, uint8_t velocity, uint16_t timestamp)
{
  Serial.printf("Note on : channel %d, note %d, velocity %d (timestamp %dms)\n", channel, note, velocity, timestamp);
  ledStrip.ledOnFromNote(note, velocity);
}

void onClientNoteOff(uint8_t channel, uint8_t note, uint8_t velocity, uint16_t timestamp)
{
  Serial.printf("Note off : channel %d, note %d, velocity %d (timestamp %dms)\n", channel, note, velocity, timestamp);
  ledStrip.ledOffFromNote(note, velocity);
}

void onMidiConnect()
{
  if (BLEMidiServer.isConnected())
  {
    Serial.println("Connected to Tablet");
  }
  else
  {
    Serial.println("Connected to Piano");
  }
  ledStrip.reset();
}

void onMidiDisconnect()
{
  ledStrip.reset();
  Serial.println("Disconnected");
  ESP.restart();
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Initializing bluetooth");
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

  Serial.println("Intializing client");
  BLEMidiClient.begin("Piano Lights Client");
  BLEMidiClient.setNoteOnCallback(onClientNoteOn);
  BLEMidiClient.setNoteOffCallback(onClientNoteOff);

  // BLEMidiClient.enableDebugging(); // Uncomment to see debugging messages from the library
}

// unsigned long clientNextTry = 0;

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
      Serial.println("Next Pattern");
      ledStrip.nextPattern();
    }
    if (button.longPress() == true)
    {
      Serial.println("Client Connect");
      int nDevices = BLEMidiClient.scan();
      // BLEMidiClient.getScannedDevice(0).
      if (nDevices > 0)
      {
        if (BLEMidiClient.connect(0))
          Serial.println("Piano connection established");
        else
        {
          Serial.println("Piano connection failed");
        }
      }
    }
    // do some periodic updates
  }
}
