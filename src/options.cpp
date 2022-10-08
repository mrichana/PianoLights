#include "options.h"
#include <Arduino.h>
#include <Preferences.h>

    Preferences preferences;

    Options::Options (){
        init();
    }

    void Options::init() {
        preferences.begin("PianoLights");
        visualizerId = preferences.getUChar("visualizerId", 0);
        sparkle = preferences.getBool("sparkle", false);
        preferences.end();
    }

    char* Options::json() {
        sprintf(buffer, "{\"visualizerId\":%u, \"sparkle\":%s, \"midiConnected\":%s}"
            , visualizerId
            , sparkle? "true":"false"
            , midiConnected? "true":"false");
        return buffer;
    } 

    void Options::setVisualizerId(unsigned char value) {
        visualizerId = value;
        preferences.begin("PianoLights");
        preferences.putUChar("visualizerId", value);
        preferences.end();
    }
    unsigned char Options::getVisualizerId() {
        return visualizerId;
    }

    void Options::setSparkle(bool value) {
        sparkle = value;
        // preferences.begin("PianoLights");
        // preferences.putBool("sparkle", value);
        // preferences.end();
    }
    
    bool Options::getSparkle() {
        return sparkle;
    }

    Options options = Options();
