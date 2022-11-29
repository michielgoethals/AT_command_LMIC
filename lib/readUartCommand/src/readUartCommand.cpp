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
    char buffer[127];
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



