#ifndef __INC_OPTIONS_H
#define __INC_OPTIONS_H

class Options {

    char buffer[1000] = "";
    unsigned char visualizerId = 2;
    bool sparkle = false;
    
    
    public:
    Options();
    char* json();
    void init();
    void setVisualizerId(unsigned char value);
    unsigned char getVisualizerId();
    void setSparkle(bool value);
    bool getSparkle();

    bool midiConnected = false;

};

extern Options options;
#endif