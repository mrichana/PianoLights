//#define Debug Serial
class httpDebug {

    static char buffer[5000];
    static int index;

    public:

    static char* getString() {
        buffer[index+1]='\0';
        index = -1;
        return buffer;
    }

    static void print(const char* string) {
        int i = 0;
        while (string[i] != '\0'){
            index++;
            buffer[index] = string[i];
            i++;
        }
    }

    static void println(const char* string) {
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
