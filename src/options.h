#ifndef __INC_OPTIONS_H
#define __INC_OPTIONS_H

class Options {
    char buffer[1000] = "";
    public:
    Options();
    bool midiConnected = false;
    unsigned char visualizerId = 2;
    bool sparkle = false;
    char* json();
};

extern Options options;
#endif