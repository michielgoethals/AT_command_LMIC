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
    char filter1[3];
    char filter2[5];

    strncpy(filter1, command, 3);
    filter1[3] = '\0';
    strncpy(filter2, command, 5);
    filter2[5] = '\0';

    if(strcmp(filter1,"mac")== 0){
        mySerial.write("mac command found\r\n");
        parseMacCommand(command);
    }else if(strcmp(filter1, "sys") == 0){
        mySerial.write("system command found\r\n");
        parseSysCommand(command);
    }else if(strcmp(filter2, "radio") == 0){
        mySerial.write("radio command found\r\n");
        parseRadioCommand(command);
    }else{
        mySerial.write("no command found\r\n");
    }
}

char* ReadUartCommand::getRemainingPart(char* arr, int offset){
    char * buff;
    int len = strlen(arr);

    for(int i = offset; i < len; i++){
        buff[i-offset] = arr[i];
    }
    
    buff[len+1] = '\0';

    return buff;
}

void ReadUartCommand::parseMacCommand(char* command){
    char filter[8]; //get first 8 char of arr to check if mac set or mac get command

    strncpy(filter, command, 8);
    filter[8] = '\0';

    if(strcmp(filter,"mac set ")== 0){
        mySerial.write("mac set command found\r\n");
        char * part;
        part getRemainingPart(command, strlen(command));
        parseMacSetCommand(part);
    }else if(strcmp(filter,"mac get")== 0){
        mySerial.write("mac get command found\r\n");
        parseMacGetCommand(command);
    }else if(strcmp(command, "mac reset 868")==0){
        wrapper.macReset(868);
    }else{
        mySerial.write("other mac command");
    }
}

void ReadUartCommand::parseMacSetCommand(char* command){
    if(strcmp(command, "devaddr ")==0){
        wrapper.setDevAddr()
    }
}

void ReadUartCommand::parseMacGetCommand(char* command){

}

void ReadUartCommand::parseSysCommand(char* command){

}

void ReadUartCommand::parseRadioCommand(char* command){
    
}



