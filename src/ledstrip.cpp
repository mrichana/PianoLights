#include <esp_random.h>
#include "ledstrip.h"

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void LedStrip::reset()
{
    FastLED.clear();
    FastLED.show();
}

void LedStrip::bpm()
{

    // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
    uint8_t BeatsPerMinute = 62;
    CRGBPalette16 palette = PartyColors_p;
    uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
    for (int i = 0; i < NUM_LEDS; i++)
    { // 9948
        leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
    }
}

void LedStrip::juggle()
{
    // eight colored dots, weaving in and out of sync with each other
    fadeToBlackBy(leds, NUM_LEDS, 20);
    uint8_t dothue = 0;
    for (int i = 0; i < 8; i++)
    {
        leds[beatsin16(i + 7, 0, NUM_LEDS - 1)] |= CHSV(dothue, 200, 255);
        dothue += 32;
    }
}

void LedStrip::rainbow()
{
    // FastLED's built-in rainbow generator
    fill_rainbow(leds, NUM_LEDS, gHue, 7);

}

void LedStrip::off()
{
    reset();
}

void LedStrip::addGlitter(fract8 chanceOfGlitter)
{

    if (random8() < chanceOfGlitter)
    {
        leds[random16(NUM_LEDS)] += CRGB::White;
    }
}

void LedStrip::rainbowWithGlitter()
{
    // built-in FastLED rainbow, plus some random sparkly glitter
    rainbow();
    addGlitter(80);
}

void LedStrip::confetti()
{
    // random colored speckles that blink in and fade smoothly
    fadeToBlackBy(leds, NUM_LEDS, 10);
    int pos = random16(NUM_LEDS);
    leds[pos] += CHSV(gHue + random8(64), 200, 255);
}

void LedStrip::sinelon()
{
    // a colored dot sweeping back and forth, with fading trails
    fadeToBlackBy(leds, NUM_LEDS, 20);
    int pos = beatsin16(13, 0, NUM_LEDS - 1);
    leds[pos] += CHSV(gHue, 255, 192);
}

// List of patterns to cycle through.  Each is defined as a separate function below.

void LedStrip::nextPattern()
{
    currentVisualizerId = (++currentVisualizerId>=7)?0:currentVisualizerId;
    //order: rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm, off
    Serial.println(currentVisualizerId);
    if (currentVisualizerId == 0) 
    {
        currentVisualizer = &LedStrip::rainbow;
    }
    if (currentVisualizerId == 1) 
    {
        currentVisualizer = &LedStrip::rainbowWithGlitter;
    }
    if (currentVisualizerId == 2) 
    {
        currentVisualizer = &LedStrip::confetti;
    }
    if (currentVisualizerId == 3) 
    {
        currentVisualizer = &LedStrip::sinelon;
    }
    if (currentVisualizerId == 4) 
    {
        currentVisualizer = &LedStrip::juggle;
    }
    if (currentVisualizerId == 5) 
    {
        currentVisualizer = &LedStrip::bpm;
    }
    if (currentVisualizerId == 6) 
    {
        currentVisualizer = &LedStrip::off;
    }
}

void LedStrip::run()
{
    unsigned long curMillis = millis();
    if (nextMillis< curMillis)
    {
        nextMillis = curMillis + period;
        (this->*currentVisualizer)();
        FastLED.show();
        EVERY_N_MILLISECONDS(20) { gHue++; } // slowly cycle the "base color" through the rainbow
    }
}

void LedStrip::ledOn(byte ledNumber)
{
}

void LedStrip::ledOff(byte ledNumber)
{
}

void LedStrip::ledOnFromNote(byte note, byte intensity)
{
    long activeLed = 174 - ((note - 21) * 2);
    int hue = esp_random();
    leds[activeLed] = CHSV(hue, 187, intensity);
    leds[activeLed + 1] = CHSV(hue, 187, intensity);
    FastLED.show();
}

void LedStrip::ledOffFromNote(byte note, byte intensity)
{
    long activeLed = 174 - ((note - 21) * 2);
    leds[activeLed] = CRGB::Black;
    leds[activeLed + 1] = CRGB::Black;
    FastLED.show();
}

LedStrip::LedStrip()
{
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
    FastLED.setBrightness(LedStrip::brightness);
}

LedStrip::~LedStrip()
{
}


LedStrip ledStrip = LedStrip();
