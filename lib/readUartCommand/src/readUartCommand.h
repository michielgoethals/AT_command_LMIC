#pragma once

#include <Arduino.h>
#include <SoftwareSerial.h>
#include "wrapLmicAT/wrapLmicAt.h"
#include <cstring>

#define MAX_LENGTH_MESSAGE 127
#define BAND868 868

using namespace std;

class ReadUartCommand{
    public:
        ReadUartCommand();
        ReadUartCommand(int rx, int tx);
        void begin(int baudrate);
        void begin();
        char * getCommand();
        void parseCommand(char* command);
        void parseMacCommand(char* command);
        void parseMacTxCommand(char* txCommand);
        void parseJoinCommand(char* joinMethod);
        void parseMacSetCommand(char* setCommand);
        void parseMacSetChCommand(char* setChCommand);
        void parseMacGetCommand(char* command);
        void parseMacGetChCommand(char* command);
        void parseSysCommand(char* command);
        void parseRadioCommand(char* command);

    private:
        char * getRemainingPart(char* arr, int offset);
        char * getRemainingPartWithoutCRNL(char* arr, int offset);
        void get2Params(char* params, char** param1, char** param2);
        void get3Params(char* params, char** param1, char** param2, char** param3);

    private:
        SoftwareSerial mySerial;
        char * command = nullptr;
        WrapLmicAT wrapper;
};