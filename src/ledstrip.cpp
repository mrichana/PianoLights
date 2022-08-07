#include <esp_random.h>
#include "options.h"
#include "ledstrip.h"

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

extern HardwareSerial Debug;

void LedStrip::reset()
{
    FastLED.clear();
    FastLED.show();
}

#pragma region Patterns
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

void LedStrip::purple()
{
    // FastLED's built-in rainbow generator
    fill_solid(leds, NUM_LEDS, CRGB(255, 0, 255));
}

void LedStrip::pink()
{
    // FastLED's built-in rainbow generator
    fill_solid(leds, NUM_LEDS, CRGB(128, 0, 0));
}

void LedStrip::blue()
{
    // FastLED's built-in rainbow generator
    fill_solid(leds, NUM_LEDS, CRGB(0, 0, 255));
}

#pragma endregion

LedStrip::visualizer LedStrip::getPattern()
{
    static uint8_t lastVisualizerId = -1;
    static LedStrip::visualizer currentVisualizer;
    if (lastVisualizerId != options.visualizerId)
    {
        lastVisualizerId = options.visualizerId;
        Debug.print("getPattern():");
        Debug.println(options.visualizerId);

        switch (options.visualizerId)
        {
        case 0:
            currentVisualizer = &LedStrip::rainbow;
            break;
        case 1:
            currentVisualizer = &LedStrip::confetti;
            break;
        case 2:
            currentVisualizer = &LedStrip::sinelon;
            break;
        case 3:
            currentVisualizer = &LedStrip::juggle;
        case 4:
            currentVisualizer = &LedStrip::bpm;
            break;
        case 5:
            currentVisualizer = &LedStrip::purple;
            break;
        case 6:
            currentVisualizer = &LedStrip::pink;
            break;
        case 7:
            currentVisualizer = &LedStrip::blue;
            break;
        default:
            currentVisualizer = &LedStrip::off;
            break;
        }
    }
    return currentVisualizer;
}

void LedStrip::nextPattern()
{
    byte visualizerId = (++(options.visualizerId) >= 9) ? 0 : options.visualizerId;
    setPattern(visualizerId);
}

void LedStrip::setPattern(byte visualizerId)
{
    // order: rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm, off
    if (visualizerId < 0 || visualizerId >= 9)
        return;

    options.visualizerId = visualizerId;
    Debug.println(options.visualizerId);
}

void LedStrip::run()
{
    unsigned long curMillis = millis();
    if (nextMillis < curMillis)
    {
        nextMillis = curMillis + period;
        (this->*getPattern())();
        if (options.sparkle) addGlitter(80);
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
    options.visualizerId = 2;
    setPattern(options.visualizerId);
    options.sparkle = false;
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
    FastLED.setBrightness(LedStrip::brightness);
    FastLED.setMaxPowerInVoltsAndMilliamps(3, 500);
}

LedStrip::~LedStrip()
{
}

LedStrip ledStrip = LedStrip();
