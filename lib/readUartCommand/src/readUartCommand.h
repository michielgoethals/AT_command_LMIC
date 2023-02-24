#pragma once

#include <Arduino.h>
#include <SoftwareSerial.h>
#include "wrapLmicAT/wrapLmicAt.h"
#include <cstring>
#include <cctype>

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
        void parseMacGetCommand(char* getCommand);
        void parseMacGetChCommand(char* getChcommand);
        void parseSysCommand(char* command);
        void parseRadioCommand(char* command);
        void sendResponse(String response);
        void sendResponse(int response);
        void sendResponseHex(int response);
        

    private:
        char * getRemainingPart(char* arr, int offset);
        void get2Params(char* params, char** param1, char** param2);
        void get3Params(char* params, char** param1, char** param2, char** param3);

    private:
        SoftwareSerial mySerial;
        char * command = nullptr;
        WrapLmicAT wrapper;
};