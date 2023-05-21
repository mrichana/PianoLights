#include <Arduino.h>
#include <Preferences.h>
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
        preferences.end();
        init = true;
    };
}


Options options = Options();
