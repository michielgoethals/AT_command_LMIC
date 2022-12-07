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

    char * word;
    word = strtok(command, " ");
    int len = strlen(word)+1;
 
    if(strcmp(word, "mac") == 0){
        parseMacCommand(getRemainingPart(command, len));
    }else if(strcmp(word, "sys") == 0){
        parseSysCommand(getRemainingPart(command, len));
    }else if(strcmp(word, "radio") == 0){
        parseRadioCommand(getRemainingPart(command, len));
    }else{
        mySerial.write("no command found\r\n");
    }


}

char* ReadUartCommand::getRemainingPart(char* arr, int offset){
    char * buff;
    int len = strlen(arr);
    buff = &arr[offset];
    return buff;
}

void ReadUartCommand::parseMacCommand(char* command){
    char * word;
    word = strtok(command, " ");
    int len = strlen(word)+1;

    if(strcmp(word,"set")== 0){
        parseMacSetCommand(getRemainingPart(command, len));
    }else if(strcmp(word,"get")== 0){
        parseMacGetCommand(getRemainingPart(command, len));
    }else if(strcmp(word, "reset")==0){
        wrapper.macReset(atoi(getRemainingPart(command,len)));
    }else if(strcmp(word, "join")==0){
        char * joinMethod = getRemainingPart(command,len);
        if(strcmp(joinMethod, "otaa\r\n")==0){
            wrapper.joinOtaa();
        }else if(strcmp(joinMethod, "abp\r\n")==0){
            wrapper.joinABP();
        }else{
            mySerial.write("invalid join method\r\n");
        }
    }
    else{
        mySerial.write("other mac command\r\n");
    }

}

void ReadUartCommand::parseMacSetCommand(char* command){
    char * word;
    word = strtok(command, " ");
    int len = strlen(word)+1;

    if(strcmp(word, "appeui")==0){
        wrapper.setAppEui((getRemainingPart(command,len)));
    }else if(strcmp(word, "deveui")==0){
        wrapper.setDevEui((getRemainingPart(command,len)));
    }else if(strcmp(word, "appkey")==0){
        wrapper.setAppKey((getRemainingPart(command,len)));
    }
}

void ReadUartCommand::parseMacGetCommand(char* command){

}

void ReadUartCommand::parseSysCommand(char* command){

}

void ReadUartCommand::parseRadioCommand(char* command){
    
}



