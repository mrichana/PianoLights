#include <esp_random.h>
#include <Arduino.h>

#include "Debug.h"

#include "options.h"
#include "ledstrip.h"

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void LedStrip::reset()
{
    FastLED.clear();
    FastLED.show();
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
    Options::RGB color = options.getColor();
    fill_solid(leds, NUM_LEDS, CRGB(color.R, color.G, color.B));
}

void LedStrip::pink()
{
    fill_solid(leds, NUM_LEDS, CRGB::HotPink);
}

void LedStrip::blue()
{
    fill_solid(leds, NUM_LEDS, CRGB::Blue);
}

#pragma endregion

LedStrip::visualizer LedStrip::getPattern()
{
    static uint8_t lastVisualizerId = -1;
    static LedStrip::visualizer currentVisualizer;
    unsigned char VisualizerId = options.getVisualizerId();
    if (lastVisualizerId != VisualizerId)
    {
        lastVisualizerId = VisualizerId;

        switch (VisualizerId)
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
//    byte visualizerId = (options.getVisualizerId() >= 8) ? 0 : options.getVisualizerId();
    unsigned char visualizerId = ((options.getVisualizerId() + 1) + (8 + 1)) % (8 + 1);
    setPattern(visualizerId);
}

void LedStrip::setPattern(byte visualizerId)
{
    // order: rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm, off
    if (visualizerId < 0 || visualizerId >= 9)
        return;

    options.setVisualizerId(visualizerId);
}

void LedStrip::run()
{
    unsigned long curMillis = millis();
    if (nextMillis < curMillis)
    {
        nextMillis = curMillis + period;
        (this->*getPattern())();
        if (options.getSparkle()) addGlitter(80);
        FastLED.show();
        EVERY_N_MILLISECONDS(20) { gHue++; } // slowly cycle the "base color" through the rainbow
    }
}

void LedStrip::ledOn(byte ledNumber, byte r, byte g, byte b)
{
    leds[(ledNumber>>1)] = CRGB(r, g, b);
    leds[(ledNumber>>1)+1] = CRGB(r, g, b);
    FastLED.show();
}

void LedStrip::ledOff(byte ledNumber)
{
    leds[(ledNumber>>1)] = CRGB::Black;
    leds[(ledNumber>>1)+1] = CRGB::Black;
    FastLED.show();
}

void LedStrip::ledOnFromNote(byte note, byte intensity)
{
    long activeLed = 174 - ((note - 21) << 1 );
    int hue = esp_random();
    leds[activeLed] = CHSV(hue, 255, intensity);
    leds[activeLed + 1] = CHSV(hue, 255, intensity);
    FastLED.show();
}

void LedStrip::ledOffFromNote(byte note, byte intensity)
{
    long activeLed = 174 - ((note - 21) << 1);
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

void LedStrip::init()
{
    static bool init = false;
    if (!init) {
    setPattern(options.getVisualizerId());
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
    //FastLED.setBrightness(LedStrip::brightness);
    FastLED.setMaxPowerInVoltsAndMilliamps(3, 1000);
    FastLED.setDither(0);
    init = true;
    }
}

LedStrip ledStrip = LedStrip();
