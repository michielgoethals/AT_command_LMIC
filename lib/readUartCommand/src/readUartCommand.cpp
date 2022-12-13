#include "readUartCommand.h"

ReadUartCommand::ReadUartCommand(int rx, int tx): mySerial(rx,tx){
}

void ReadUartCommand::begin(int baudrate){
    mySerial.begin(baudrate);
    mySerial.write("Serial started @STM\r\n");
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

void ReadUartCommand::parseMacCommand(char* command){
    char * word;
    word = strtok(command, " ");
    int len = strlen(word)+1;
    
    if(strcmp(word, "reset")==0){
        wrapper.reset(atoi(getRemainingPartWithoutCRNL(command,len)));
    }else if(strcmp(word, "tx")==0){
        parseMacTxCommand(getRemainingPart(command, len));
    }else if(strcmp(word, "join")==0){
        parseJoinCommand(getRemainingPartWithoutCRNL(command, len));
    }else if(strcmp(word,"save\r\n")== 0){
        wrapper.save();
    }else if(strcmp(word,"forceENABLE\r\n")== 0){
        wrapper.forceEnable();
    }else if(strcmp(word,"pause\r\n")== 0){
        wrapper.pause();
    }else if(strcmp(word,"resume\r\n")== 0){
        wrapper.pause();
    }else if(strcmp(word,"set")== 0){
        parseMacSetCommand(getRemainingPart(command, len));
    }else if(strcmp(word,"get")== 0){
        parseMacGetCommand(getRemainingPart(command, len));
    }
}

void ReadUartCommand::parseMacTxCommand(char* txCommand){
    char* cnf;
    char* param2;
    char* portno; 
    char* data;
    get3Params(txCommand, cnf, portno, data);

    if(strcmp(cnf, "cnf")==0){
        wrapper.tx(true, atoi(portno), data);
    }else if(strcmp(cnf, "uncnf")==0){
        wrapper.tx(false, atoi(portno), data);
    }
}

void ReadUartCommand::parseJoinCommand(char* joinMethod){
        if(strcmp(joinMethod, "otaa")==0){
            wrapper.joinOtaa();
        }else if(strcmp(joinMethod, "abp")==0){
            wrapper.joinABP();
        }
}

void ReadUartCommand::parseMacSetCommand(char* setCommand){
    char * word;
    word = strtok(setCommand, " ");
    int len = strlen(word)+1;

    LoraParam param = getRemainingPartWithoutCRNL(setCommand,len);

    if(strcmp(word, "devaddr")==0){
        wrapper.setDevAddr(param);
    }else if(strcmp(word, "deveui")==0){
        wrapper.setDevEui(param);
    }else if(strcmp(word, "appeui")==0){
        wrapper.setAppEui(param);
    }else if(strcmp(word, "nwkskey")==0){
        wrapper.setNwkskey(param);
    }else if(strcmp(word, "appskey")==0){
        wrapper.setAppsKey(param);
    }else if(strcmp(word, "appkey")==0){
        wrapper.setAppKey(param);
    }else if(strcmp(word, "pwridx")==0){
        wrapper.setPwridx(atoi(getRemainingPartWithoutCRNL(command,len)));
    }else if(strcmp(word, "dr")==0){
        wrapper.setDr(atoi(getRemainingPartWithoutCRNL(command,len)));
    }else if(strcmp(word, "adr")==0){
        char* param = getRemainingPartWithoutCRNL(setCommand,len);
        if(strcmp(param, "on")==0){
            wrapper.setAdr(true);
        }else if(strcmp(param, "off")==0){
            wrapper.setAdr(false);
        }else{
            //return invalid param
        }
    }else if(strcmp(word, "bat")==0){
        wrapper.setBat(atoi(getRemainingPartWithoutCRNL(command,len)));
    }else if(strcmp(word, "retx")==0){
        wrapper.setBat(atoi(getRemainingPartWithoutCRNL(command,len)));
    }else if(strcmp(word, "linkchk")==0){
        wrapper.setLinkChk(atoi(getRemainingPartWithoutCRNL(command,len)));
    }else if(strcmp(word, "rxdelay1")==0){
        wrapper.setRxDelay1(atoi(getRemainingPartWithoutCRNL(command,len)));
    }else if(strcmp(word, "ar")==0){
        char* param = getRemainingPartWithoutCRNL(setCommand,len);
        if(strcmp(param, "on")==0){
            wrapper.setAr(true);
        }else if(strcmp(param, "off")==0){
            wrapper.setAr(false);
        }
   
    }else if(strcmp(word, "rx2")==0){
        char* dr;
        char* freq;
        get2Params(getRemainingPart(setCommand,len), dr, freq);
        wrapper.setRx2(atoi(dr), atoi(freq));
    }else if(strcmp(word, "ch")==0){
        parseMacSetChCommand(getRemainingPart(setCommand,len));
    }
}

void ReadUartCommand::parseMacSetChCommand(char* setChCommand){
    char * word;
    word = strtok(setChCommand, " ");
    int len = strlen(word)+1;

    if(strcmp(word, "freq")==0){
        char* chID;
        char* freq;
        get2Params(getRemainingPart(setChCommand,len), chID, freq);
        wrapper.setChFreq(atoi(chID), atoi(freq));
    }else if(strcmp(word, "dcycle")==0){
        char* chID;
        char* dcycle;
        get2Params(getRemainingPart(setChCommand,len), chID, dcycle);
        wrapper.setChDutyCycle(atoi(chID), atoi(dcycle));
    }else if(strcmp(word, "drrange")==0){
        char* chID;
        char* minRange;
        char* maxRange;

        get3Params(getRemainingPart(setChCommand,len), chID, minRange, maxRange);
    
        wrapper.setChDrRange(atoi(chID), atoi(minRange), atoi(maxRange));

    }else if(strcmp(word, "status")==0){
        char* chID;
        char* param2;
        get2Params(getRemainingPart(setChCommand,len), chID, param2);
        if(strcmp(param2, "on\r\n")==0){
            wrapper.setChStatus(atoi(chID), true);
        }else if(strcmp(param2, "off\r\n")==0){
            wrapper.setChStatus(atoi(chID), false);
        }else{
            //return invalid param
        }
    }
}

void ReadUartCommand::parseMacGetCommand(char* command){

}

void ReadUartCommand::parseSysCommand(char* command){

}

void ReadUartCommand::parseRadioCommand(char* command){
    
}

char* ReadUartCommand::getRemainingPart(char* arr, int offset){
    char* buff = &arr[offset];

    return buff;
}

char* ReadUartCommand::getRemainingPartWithoutCRNL(char* arr, int offset){
    char* arrCRNL = strtok(arr, "\r");
    char* buff = &arrCRNL[offset];

    return buff;
}

void ReadUartCommand::get2Params(char* params, char* param1, char* param2){
    param1 = strtok(params, " ");
    int lenParam1 = strlen(param1)+1;
    param2 = getRemainingPartWithoutCRNL(params,lenParam1);
}

void ReadUartCommand::get3Params(char* params, char* param1, char* param2, char* param3){
    param1 = strtok(params, " ");
    int lenParam1 = strlen(param1)+1;
    char* buff = getRemainingPart(params, lenParam1);
    get2Params(buff, param2, param3);
    }


