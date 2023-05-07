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
    byte visualizerId = 2;
    bool sparkle = false;
    RGB color[3];
    byte brightness;

    
    
    public:
    Options();
    const String json();
    void init();
    void setVisualizerId(byte value);
    byte getVisualizerId();
    void setSparkle(bool value);
    bool getSparkle();
    void setBrightness(byte value);
    byte getBrightness();
    void setColor(byte o, byte r, byte g, byte b);
    RGB getColor(byte o);

    bool midiConnected = false;

};

extern Options options;
#endif