#pragma once

#include <Arduino.h>
#include "wrapLmicAT/wrapMacAt.h"
#include "wrapLmicAT/wrapSysAt.h"
#include "wrapLmicAT/wrapRadioAt.h"
#include <cstring>
#include <cctype>

#define MAX_LENGTH_MESSAGE 127

using namespace std;

class ReadUartCommand{
    //general
    public:
        void begin(int baudrate, UART_HandleTypeDef *uart, RTC_HandleTypeDef *rtc);
        char* getCommand();
        void parseCommand(char* command);
        void sendResponse(String response);
        void sendResponse(int response);
        void sendResponseHex(int response);

    //mac related commands 
    public:
        void parseMacCommand(char* command);
        void parseMacTxCommand(char* txCommand);
        void parseJoinCommand(char* joinMethod);
        void parseMacSetCommand(char* setCommand);
        void parseMacSetChCommand(char* setChCommand);
        void parseMacGetCommand(char* getCommand);
        void parseMacGetChCommand(char* getChcommand);

    //system related commands
    public:
        void parseSysCommand(char* command);
        void parseSysSetCommand(char* setCommand);
        void parseSysGetCommand(char* getCommand);

    //radio related commands
        void parseRadioCommand(char* command);
        void parseRadioSetCommand(char* setCommand);
        void parseRadioGetCommand(char* getCommand);
 
    //private functions
    private:
        char * getRemainingPart(char* arr, int offset);
        void get2Params(char* params, char** param1, char** param2);
        void get3Params(char* params, char** param1, char** param2, char** param3);

    //private attributes
    private:
        String startupMessage = "STM32 started";

        char * command = nullptr;

        RTC_HandleTypeDef hrtc;
        UART_HandleTypeDef huart;

        WrapMacAt macWrapper;
        WrapSysAt sysWrapper;
        WrapRadioAt radioWrapper;
        
        String responses[13] = {"ok", "busy", "frame_counter_err_rejoin_needed", 
        "invalid_class", "invalid_data_len", "invalid_param", "keys_not_init", "mac_paused","multicast_keys_not_set",  
        "no_free_ch", "silent", "err"};
};