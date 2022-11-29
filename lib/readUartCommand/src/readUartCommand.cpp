#include "readUartCommand.h"

ReadUartCommand::ReadUartCommand(int rx, int tx): mySerial(rx,tx){
}

void ReadUartCommand::begin(int baudrate){
    mySerial.begin(baudrate);
    mySerial.write("Serial started @STM\r\n");
}

void ReadUartCommand::sendReply(char * reply){
    mySerial.write(reply);
}

char * ReadUartCommand::getCommand(){
    char buffer[MAX_LENGTH_MESSAGE];
    int index = 0;

    while(mySerial.available() > 0){
        buffer[index]= mySerial.read();
        index++;
    }  

    command = new char[index];
    strncpy(command,buffer,index+1);
    command[index] = '\0';
    return command;
}

void ReadUartCommand::parseCommand(char* command){
    char macOrSys[3];
    char radio[5];

    strncpy(macOrSys, command, 3);
    macOrSys[3] = '\0';
    strncpy(radio, command, 5);
    radio[5] = '\0';

    if(strcmp(macOrSys,"mac")== 0){
        mySerial.write("mac command found\r\n");
        parseMacCommand(command);
    }else if(strcmp(macOrSys, "sys") == 0){
        mySerial.write("system command found\r\n");
        parseSysCommand(command);
    }else if(strcmp(radio, "radio") == 0){
        mySerial.write("radio command found\r\n");
        parseRadioCommand(command);
    }else{
        mySerial.write("no command found\r\n");
    }
}

void ReadUartCommand::parseMacCommand(char* command){
    char mac_set[7];
    char mac_get[7];

    strncpy(mac_set, command, 7);
    mac_set[7] = '\0';
    strncpy(mac_get, command, 7);
    mac_get[5] = '\0';

    if(strcmp(mac_set,"mac set")== 0){
        mySerial.write("mac set command found\r\n");
        parseMacSetCommand(command);
    }else if(strcmp(mac_get,"mac get")== 0){
        mySerial.write("mac get command found\r\n");
        parseMacGetCommand(command);
    }else{
        mySerial.write("other mac command");
    }
}

void ReadUartCommand::parseMacSetCommand(char* command){

}

void ReadUartCommand::parseMacGetCommand(char* command){

}

void ReadUartCommand::parseSysCommand(char* command){

}

void ReadUartCommand::parseRadioCommand(char* command){
    
}



