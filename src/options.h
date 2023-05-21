#ifndef __INC_OPTIONS_H
#define __INC_OPTIONS_H
#include <Arduino.h>

class Options {

    public:

    private:
    
    public:
    Options();
    void init();
    bool midiConnected = false;

};

extern Options options;
#endif