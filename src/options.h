#ifndef __INC_OPTIONS_H
#define __INC_OPTIONS_H
#include <Arduino.h>

class Options {

    public:
    struct RGB {
        byte R;
        byte G;
        byte B; 
    };

    private:
    char buffer[1000] = "";
    unsigned char visualizerId = 2;
    bool sparkle = false;
    RGB color;

    
    
    public:
    Options();
    char* json();
    void init();
    void setVisualizerId(unsigned char value);
    unsigned char getVisualizerId();
    void setSparkle(bool value);
    bool getSparkle();
    void setColor(byte r, byte g, byte b);
    RGB getColor();

    bool midiConnected = false;

};

extern Options options;
#endif