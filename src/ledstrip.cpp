#include <Arduino.h>

// #include "options.h"
#include "ledstrip.h"

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void LedStrip::reset()
{
    FastLED.clear();
    FastLED.show();
    FastLED.clear();
    FastLED.show();
}

void LedStrip::setBrightness(byte brightness)
{
    FastLED.setBrightness(brightness);
}

void LedStrip::ledOn(byte ledNumber)
{
    leds[(ledNumber * 2)] = CRGB::White;
    leds[(ledNumber * 2) + 1] = CRGB::White;
    FastLED.show();
}
void LedStrip::ledOn(byte ledNumber, CRGB color)
{
    leds[(ledNumber * 2)] = color;
    leds[(ledNumber * 2) + 1] = color;
    FastLED.show();
}
void LedStrip::ledOn(byte ledNumber, CHSV color)
{
    leds[(ledNumber * 2)] = color;
    leds[(ledNumber * 2) + 1] = color;
    FastLED.show();
}

void LedStrip::ledOff(byte ledNumber)
{
    leds[(ledNumber * 2)] = CRGB::Black;
    leds[(ledNumber * 2) + 1] = CRGB::Black;
    FastLED.show();
}

long getLedsFromNote(byte note)
{
    long ret;
    if (note < 73)
    {
        ret = 174 - ((note - 21) << 1);
    }
    else if (note == 73)
    {
        ret = 174 - ((note - 21) << 1);
    }
    else if (note > 73)
    {
        ret = 174 - ((note - 21) << 1) + 1;
    }
    return ret;
}

void LedStrip::ledOnFromNote(byte note, byte intensity, LightType lighttype)
{
    long activeLed = getLedsFromNote(note);
    uint8_t hue, saturation = 255;
    switch (lighttype) {
        case random:
        hue = random8();
        saturation = random8();
        break;
        case rainbow:
        hue = ((float)note-21)/88*255; //88*255
        break;
        case red:
        hue=0;
        break;
        case green:
        hue=85;
        case blue:
        hue=171;
        case white:
        hue=255;
        saturation=0;
        break;
    }
    leds[activeLed] = CHSV(hue, saturation, intensity);
    leds[activeLed + 1] = CHSV(hue, saturation, intensity);
    FastLED.show();
}

void LedStrip::ledOffFromNote(byte note)
{
    long activeLed = getLedsFromNote(note);

    leds[activeLed] = CRGB::Black;
    leds[activeLed + 1] = CRGB::Black;
    FastLED.show();
}

LedStrip::LedStrip()
{
}

LedStrip::~LedStrip()
{
}

void LedStrip::setup()
{
    static bool init = false;
    if (!init)
    {
        FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
        FastLED.setMaxPowerInVoltsAndMilliamps(VOLTS, AMPS);
        FastLED.setDither(0);
        init = true;
    }
}

LedStrip ledStrip = LedStrip();
