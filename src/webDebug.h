#ifndef __INC_DEBUG_H
#define __INC_DEBUG_H

class WebDebug {

    bool serialInit;
    String log = String();

    public:

    void serial(const char* string) {
        if (!serialInit) {
            serialInit = true;
            Serial.begin(115200);
        }
        Serial.write(string);

    }

    String getString() {
        return log;
    }

    void print(const char* string) {
        serial(string);
        log += string;
    }

    void println(const char* string) {
        serial(string);
        serial("\n");
        log += string;
        log += '\n';
    }

    void print(const String string) {
        print(string.c_str());
    }

    void println(const String string) {
        println(string.c_str());
    }

    void print(short number) {
        print(String(number).c_str());
    }
    void println(short number) {
        println(String(number).c_str());
    }
};
#endif

extern WebDebug webDebug;
