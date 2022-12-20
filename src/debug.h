//#define Debug Serial
#ifndef __INC_DEBUG_H
#define __INC_DEBUG_H

class httpDebug {

    static bool serialInit;
    static char buffer[5000];
    static int index;

    public:

    static void serial(const char* string) {
        if (!serialInit) {
            serialInit = true;
            Serial.begin(115200);
        }
        Serial.print(string);
    }

    static char* getString() {
        buffer[index+1]='\0';
        index = -1;
        return buffer;
    }

    static void print(const char* string) {
        serial(string);
        int i = 0;
        while (string[i] != '\0'){
            index++;
            buffer[index] = string[i];
            i++;
        }
    }

    static void println(const char* string) {
        serial(string);
        serial("\n");
        int i = 0;
        while (string[i] != '\0'){
            index++;
            buffer[index] = string[i];
            i++;
        }
        index++;
        buffer[index] = '\n';
    }

    static void print(byte number) {
        print(String(number).c_str());
    }
    static void println(byte number) {
        println(String(number).c_str());
    }
};

#endif
