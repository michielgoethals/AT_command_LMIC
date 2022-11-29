#pragma once

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <string>

#define MAX_LENGTH_MESSAGE 127

#define CHAR_NEWLINE '\n'
#define CHAR_RETURN '\r'
#define CHAR_TERMINATING '\0'
#define RETURN_NEWLINE "\r\n"

using namespace std;

class ReadUartCommand{
    public:
        ReadUartCommand();
        ReadUartCommand(int rx, int tx);
        void begin(int baudrate);
        void begin();
        char * getCommand();
        void sendReply(char * reply);

    private:
        SoftwareSerial mySerial;
        char * command = nullptr;
};