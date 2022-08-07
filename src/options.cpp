#include "options.h"
#include "Arduino.h"

    Options::Options() 
    {}
    char* Options::json() {
        sprintf(buffer, "{\"visualizerId\":%u, \"sparkle\":%s, \"midiConnected\":%s}"
            , visualizerId
            , sparkle? "true":"false"
            , midiConnected? "true":"false");
        return buffer;
    } 

    Options options = Options();
