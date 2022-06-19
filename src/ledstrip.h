#ifndef __INC_LED_H
#define __INC_LED_H

#include <FastLED.h>
#define NUM_LEDS 176
#define DATA_PIN 23

class LedStrip
{
  typedef void (LedStrip::*SimplePatternList[])();
  static const byte brightness = 42, framesPerSecond = 90;
  static const unsigned long period = 1000/framesPerSecond;
  CRGBArray<NUM_LEDS> leds;
  byte gCurrentPatternNumber = 0; // Index number of which pattern is current
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

  byte currentVisualizerId = 0;

  typedef void (LedStrip::*visualizer)();

  visualizer currentVisualizer = &LedStrip::rainbow;

  unsigned long nextMillis = 0;

public:
  LedStrip();
  ~LedStrip();

  void reset();

  void nextPattern();
  void run();
  void ledOn(byte ledNumber);
  void ledOff(byte ledNumber);
  void ledOnFromNote(byte note, byte intensity = 127);
  void ledOffFromNote(byte note, byte intensity =127);
};

extern LedStrip ledStrip;
#endif