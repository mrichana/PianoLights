#ifndef __INC_LED_H
#define __INC_LED_H

#include <FastLED.h>

#define NUM_LEDS 176
#define DATA_PIN 23
#define VOLTS 3
// #define VOLTS 5
// #define DATA_PIN 13
#define AMPS 1000

class LedStrip
{
  CRGBArray<NUM_LEDS> leds;

public:
  enum LightType
  {
    random,
    rainbow,
    red,
    blue,
    green,
    white
  };

  LedStrip();
  ~LedStrip();

  void setup();

  void reset();

  void setBrightness(byte brightness);
  void ledOn(byte lednumber);
  void ledOn(byte ledNumber, CRGB color);
  void ledOn(byte ledNumber, CHSV color);
  void ledOff(byte ledNumber);
  void ledOnFromNote(byte note, byte intensity, LightType lighttype);
  void ledOffFromNote(byte note);
};

extern LedStrip ledStrip;
#endif