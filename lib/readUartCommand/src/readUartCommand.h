#pragma once

#include <Arduino.h>
#include "wrapLmicAT/wrapMacAt.h"
#include "wrapLmicAT/wrapSysAt.h"
#include "wrapLmicAT/wrapRadioAt.h"
#include <cstring>
#include <cctype>

#define MAX_LENGTH_MESSAGE 127
#define DEFAULT_BAUD 57600

using namespace std;

class ReadUartCommand{
    public:
        ReadUartCommand();
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
        char * command = nullptr;
        UART_HandleTypeDef huart2;
        WrapMacAt macWrapper;
        WrapSysAt sysWrapper;
        WrapRadioAt radioWrapper;
        String response;
        String responses[13] = {"ok", "busy", "frame_counter_err_rejoin_needed", 
        "invalid_class", "invalid_data_len", "invalid_param", "keys_not_init", "mac_paused","multicast_keys_not_set",  
        "no_free_ch", "silent", "err"};
};