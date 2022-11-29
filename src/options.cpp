#include "options.h"
#include <Arduino.h>
#include <Preferences.h>

Preferences preferences;

Options::Options()
{
}

void Options::init()
{
    static bool init = false;
    if (!init)
    {
        preferences.begin("PianoLights");
        visualizerId = preferences.getUChar("visualizerId", 0);
        sparkle = preferences.getBool("sparkle", false);
        color.R = preferences.getUShort("color_r", 128);
        color.G = preferences.getUShort("color_g", 128);
        color.B = preferences.getUShort("color_b", 128);
        preferences.end();
        init = true;
    };
}

char *Options::json()
{
    sprintf(buffer, "{\"visualizerId\":%u, \"sparkle\":%s, \"midiConnected\":%s, \"red\":%u, \"green\":%u, \"blue\":%u}", visualizerId, sparkle ? "true" : "false", midiConnected ? "true" : "false", color.R<<1, color.G<<1, color.B);
    return buffer;
}

void Options::setVisualizerId(unsigned char value)
{
    visualizerId = value;
    preferences.begin("PianoLights");
    preferences.putUChar("visualizerId", value);
    preferences.end();
}

unsigned char Options::getVisualizerId()
{
    return visualizerId;
}

void Options::setSparkle(bool value)
{
    sparkle = value;
    preferences.begin("PianoLights");
    preferences.putBool("sparkle", value);
    preferences.end();
}

bool Options::getSparkle()
{
    return sparkle;
}

void Options::setColor(byte r, byte g, byte b)
{
    color.R = r>>1;
    color.G = g>>1;
    color.B = b>>1;
    preferences.begin("PianoLights");
    preferences.putUShort("color_r", r);
    preferences.putUShort("color_g", g);
    preferences.putUShort("color_b", b);
    preferences.end();
}

Options::RGB Options::getColor()
{
    return color;
}

Options options = Options();
