#pragma once

#include <Arduino.h>
#include <SoftwareSerial.h>
#include "Wrapper/wrapper.h"

#define MAX_LENGTH_MESSAGE 127

#define MAC_LEN 3
#define SYS_LEN 3
#define RADIO_LEN 5

using namespace std;

class ReadUartCommand{
    public:
        ReadUartCommand();
        ReadUartCommand(int rx, int tx);
        void begin(int baudrate);
        void begin();
        char * getCommand();
        void parseCommand(char* command);
        void sendReply(char * reply);
        void parseMacCommand(char* command);
        void parseMacSetCommand(char* command);
        void parseMacGetCommand(char* command);
        void parseSysCommand(char* command);
        void parseRadioCommand(char* command);

    private:
        SoftwareSerial mySerial;
        char * command = nullptr;
        //const char * list_of_mac_commands[] ={"reset", "set appkey", "set deveui", "set appeui","join otaa", "join abp"};
        //const char * list_of_sys_commands[] ={""};
        //const char * list_of_radio_commands[] ={""};mySerial.write("no command found\r\n");
};