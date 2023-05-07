#include <Arduino.h>
#include <Preferences.h>
#include "httpServer.h"
#include "options.h"

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
        brightness = preferences.getUChar("brightness", 127);
        for (int i = 0; i < 3; i++)
        {
            color[i].R = preferences.getUChar("color_r_" + i, 127);
            color[i].G = preferences.getUChar("color_g_" + i, 127);
            color[i].B = preferences.getUChar("color_b_" + i, 127);
        }
        preferences.end();
        server.sendJSON();
        init = true;
    };
}

const String Options::json()
{
    String buffer = "{\"visualizerId\":" + String(visualizerId) + ", \"sparkle\":"+ (sparkle ? "true" : "false") + ", \"midiConnected\":" + (midiConnected ? "true" : "false") 
    +", \"brightness\": " + String(brightness) + ", \"customColors\": [{\"Red\": "+ String(color[0].R) + ", \"Green\":" + String(color[0].G) + ", \"Blue\":" + String(color[0].B) 
    + "}, {\"Red\":" + String(color[1].R) + ", \"Green\":" + String(color[1].G) + ", \"Blue\": " + String(color[1].B) 
    + "}, {\"Red\":" + String(color[2].R) + ", \"Green\":" + String(color[2].R) + ", \"Blue\":" + String(color[2].B) + "}]}";
    return buffer;
}

void Options::setVisualizerId(byte value)
{
    visualizerId = value;
    preferences.begin("PianoLights");
    preferences.putUChar("visualizerId", value);
    preferences.end();

    server.sendJSON();
}

byte Options::getVisualizerId()
{
    return visualizerId;
}

void Options::setSparkle(bool value)
{
    sparkle = value;
    preferences.begin("PianoLights");
    preferences.putBool("sparkle", value);
    preferences.end();

    server.sendJSON();
}

bool Options::getSparkle()
{
    return sparkle;
}

void Options::setBrightness(byte value)
{
    brightness = value;
    preferences.begin("PianoLights");
    preferences.putUChar("brightness", value);
    preferences.end();

    server.sendJSON();
}

byte Options::getBrightness()
{
    return brightness;
}

void Options::setColor(byte o, byte r, byte g, byte b)
{
    color[o].R = r;
    color[o].G = g;
    color[o].B = b;
    preferences.begin("PianoLights");
    preferences.putUChar("color_r_" + o, r);
    preferences.putUChar("color_g_" + o, g);
    preferences.putUChar("color_b_" + o, b);
    preferences.end();

    server.sendJSON();
}

Options::RGB Options::getColor(byte o)
{
    return color[o];
}

Options options = Options();
