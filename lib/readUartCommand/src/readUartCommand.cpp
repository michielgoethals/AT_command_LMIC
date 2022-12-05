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

    buff = &arr[offset];

    buff[len+1] = '\0';

    return buff;
}

void ReadUartCommand::parseMacCommand(char* command){
    char filter[MACSETCHARS]; //get first 8 char of arr to check if mac set or mac get command

    strncpy(filter, command, MACSETCHARS);
    filter[MACSETCHARS] = '\0';

    char * part;

    if(strcmp(filter,"mac set ")== 0){
        part = getRemainingPart(command, MACSETCHARS);
        mySerial.write(part);
        parseMacSetCommand(part);
    }else if(strcmp(filter,"mac get ")== 0){
        part = getRemainingPart(command, MACGETCHARS);
        parseMacGetCommand(part);
    }else if(strcmp(command, "mac reset 868")==0){
        wrapper.macReset(868);
    }else if(strcmp(command, "mac reset 434")==0){
        wrapper.macReset(434);
    }else{
        mySerial.write("other mac command");
    }
}

void ReadUartCommand::parseMacSetCommand(char* macsetcom){
  /*   LoraParam test = 0;
    if(strcmp(command, "devaddr ")==0){
        wrapper.setDevAddr(test);
    } */
}

void ReadUartCommand::parseMacGetCommand(char* command){

}

void ReadUartCommand::parseSysCommand(char* command){

}

void ReadUartCommand::parseRadioCommand(char* command){
    
}



