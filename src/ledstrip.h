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
  typedef void (LedStrip::*SimplePatternList[])();
  static const byte brightness = 10, framesPerSecond = 90;
  static const unsigned long period = 1000/framesPerSecond;
  CRGBArray<NUM_LEDS> leds;
  byte gHue = 0;                  // rotating "base color" used by many of the patterns

  // List of patterns to cycle through.  Each is defined as a separate function below.
  void bpm();
  void juggle();
  void rainbow();
  void off();
  void addGlitter(fract8 chanceOfGlitter);
  void rainbowWithGlitter();
  void confetti();
  void sinelon();
  void purple();
  void pink();
  void blue();

  typedef void (LedStrip::*visualizer)();

  LedStrip::visualizer getPattern();

  unsigned long nextMillis = 0;

public:
  LedStrip();
  ~LedStrip();

  void init();

  void reset();

  void setBrightness(byte brightness);
  byte nextPattern();
  byte setPattern(byte visualizerId);
  void run();
  void ledOn(byte lednumber);
  void ledOn(byte ledNumber, CRGB color);
  void ledOn(byte ledNumber, CHSV color);
  void ledOff(byte ledNumber);
  void ledOnFromNote(byte note, byte intensity = 127);
  void ledOffFromNote(byte note, byte intensity =127);
};

extern LedStrip ledStrip;
#endif