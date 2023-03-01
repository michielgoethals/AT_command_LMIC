#include "readUartCommand.h"

ReadUartCommand::ReadUartCommand(){
}

void ReadUartCommand::begin(){
    begin(DEFAULT_BAUD);
}

void ReadUartCommand::begin(int baudrate){
    Serial.begin(baudrate);
    Serial.println("Serial started @STM");
    wrapper.begin();
}

char * ReadUartCommand::getCommand(){
    char buffer[MAX_LENGTH_MESSAGE];
    int index = 0;

    while(Serial.available() > 0){
        buffer[index]= tolower(Serial.read());
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
        Serial.write("sys command found\r\n");
        parseSysCommand(getRemainingPart(command, len));
    }else if(strcmp(word, "radio") == 0){
        Serial.write("radio command found\r\n");
        parseRadioCommand(getRemainingPart(command, len));
    }else{
        Serial.write("no command found\r\n");
    }
}

void ReadUartCommand::parseMacCommand(char* command){
    char * word;
    word = strtok(command, " ");
    int len = strlen(word)+1;
    
    if(strcmp(word, "reset")==0){
        wrapper.macReset(atoi(getRemainingPart(command,len)));
    }else if(strcmp(word, "tx")==0){
        parseMacTxCommand(getRemainingPart(command, len));
    }else if(strcmp(word, "join")==0){
        parseJoinCommand(getRemainingPart(command, len));
    }else if(strcmp(word,"save")== 0){
        wrapper.macSave();
    }else if(strcmp(word,"forceENABLE")== 0){
        wrapper.macForceEnable();
    }else if(strcmp(word,"pause")== 0){
        wrapper.macPause();
    }else if(strcmp(word,"resume")== 0){
        wrapper.macResume();
    }else if(strcmp(word,"set")== 0){
        parseMacSetCommand(getRemainingPart(command, len));
    }else if(strcmp(word,"get")== 0){
        parseMacGetCommand(getRemainingPart(command, len));
    }else if(strcmp(word,"test")== 0){
        sendResponse("test communication ok");
    }
}

void ReadUartCommand::parseMacTxCommand(char* txCommand){
    char* cnf;
    char* portno; 
    char* data;

    get3Params(txCommand, &cnf, &portno, &data);

    wrapper.macTx(cnf, atoi(portno), data);
}

void ReadUartCommand::parseJoinCommand(char* joinMethod){
        if(strcmp(joinMethod, "otaa")==0){
            wrapper.macJoinOtaa();
        }else if(strcmp(joinMethod, "abp")==0){
            wrapper.macJoinABP();
        }else{
            Serial.print("invalid_param");
        }
}

void ReadUartCommand::parseMacSetCommand(char* setCommand){
    char * word;
    word = strtok(setCommand, " ");
    int len = strlen(word)+1;

    char* param = getRemainingPart(setCommand,len);

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
        wrapper.setPwridx(atoi(param));
    }else if(strcmp(word, "dr")==0){
        wrapper.setDr(atoi(param));
    }else if(strcmp(word, "adr")==0){
        wrapper.setAdr(param);
    }else if(strcmp(word, "bat")==0){
        wrapper.setBat(atoi(param));
    }else if(strcmp(word, "retx")==0){
        wrapper.setBat(atoi(param));
    }else if(strcmp(word, "linkchk")==0){
        wrapper.setLinkChk(atoi(param));
    }else if(strcmp(word, "rxdelay1")==0){
        wrapper.setRxDelay1(atoi(param));
    }else if(strcmp(word, "ar")==0){
        wrapper.setAdr(param);
    }else if(strcmp(word, "rx2")==0){
        char* dr;
        char* freq;
        get2Params(getRemainingPart(setCommand,len), &dr, &freq);
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
        get2Params(getRemainingPart(setChCommand,len), &chID, &freq);
        wrapper.setChFreq(atoi(chID), atoi(freq));
    }else if(strcmp(word, "dcycle")==0){
        char* chID;
        char* dcycle;
        get2Params(getRemainingPart(setChCommand,len), &chID, &dcycle);
        wrapper.setChDCycle(atoi(chID), atoi(dcycle));
    }else if(strcmp(word, "drrange")==0){
        char* chID;
        char* minRange;
        char* maxRange;
        get3Params(getRemainingPart(setChCommand,len), &chID, &minRange, &maxRange);
        wrapper.setChDrRange(atoi(chID), atoi(minRange), atoi(maxRange));
    }else if(strcmp(word, "status")==0){
        char* chID;
        char* param2;
        get2Params(getRemainingPart(setChCommand,len), &chID, &param2);
        wrapper.setChStatus(atoi(chID), param2);
    }
}

void ReadUartCommand::parseMacGetCommand(char* getCommand){
    char * word;
    word = strtok(getCommand, " ");
    int len = strlen(word)+1;

    if(strcmp(word,"devaddr")==0){
        sendResponse(wrapper.getDevAddr());
    }else if(strcmp(word, "deveui")==0){
        sendResponse(wrapper.getDevEui());
    }else if(strcmp(word, "appeui")==0){
        sendResponse(wrapper.getAppEui());
    }else if(strcmp(word, "dr")==0){
        sendResponse(wrapper.getDr());
    }else if(strcmp(word, "band")==0){
        sendResponse(wrapper.getBand());
    }else if(strcmp(word, "pwridx")==0){
        sendResponse(wrapper.getPwridx());
    }else if(strcmp(word, "adr")==0){
        sendResponse(wrapper.getAdr());
    }else if(strcmp(word, "retx")==0){
        sendResponse(wrapper.getRetX());
    }else if(strcmp(word, "rxdelay1")==0){
        sendResponse(wrapper.getRxDelay1());
    }else if(strcmp(word, "rxdelay2")==0){
        sendResponse(wrapper.getRxDelay2());
    }else if(strcmp(word, "ar")==0){
        sendResponse(wrapper.getAr());
    }else if(strcmp(word, "rx2")==0){
        sendResponse(wrapper.getRx2(atoi(getRemainingPart(getCommand,len))));
    }else if(strcmp(word, "dcycleps")==0){
        sendResponse(wrapper.getDcycleps());
    }else if(strcmp(word, "mrgn")==0){
        sendResponse(wrapper.getMrgn());
    }else if(strcmp(word, "gwnb")==0){
        sendResponse(wrapper.getGwnb());
    }else if(strcmp(word, "status")==0){
        sendResponse(wrapper.getSatus());
    }else if(strcmp(word, "ch")==0){
        parseMacGetChCommand(getRemainingPart(getCommand,len));
    }
}

void ReadUartCommand::parseMacGetChCommand(char* getChCommand){
    char * word;
    word = strtok(getChCommand, " ");
    int len = strlen(word)+1;

    char* chID = getRemainingPart(getChCommand,len);

    if(strcmp(word, "freq")==0){
        sendResponse(wrapper.getChFreq(atoi(chID)));
    }else if(strcmp(word, "dcycle")==0){
        sendResponse(wrapper.getChDcycle(atoi(chID)));
    }else if(strcmp(word, "drrange")==0){
        sendResponse(wrapper.getChDrrange(atoi(chID)));
    }else if(strcmp(word, "status")==0){
        sendResponse(wrapper.getChStatus(atoi(chID)));
    }
}

void ReadUartCommand::parseSysCommand(char* command){
    char * word;
    word = strtok(command, " ");
    int len = strlen(word)+1;
    
    if(strcmp(word, "reset")==0){
        wrapper.sysReset();
    }
}

void ReadUartCommand::parseRadioCommand(char* command){
    
}

void ReadUartCommand::sendResponse(String response){
    Serial.println(response);
}

void ReadUartCommand::sendResponse(int response){
    sendResponse((String)response);
}

void ReadUartCommand::sendResponseHex(int response){
    Serial.println(response, HEX);
}

char* ReadUartCommand::getRemainingPart(char* arr, int offset){
    char* buff = &arr[offset];
    char* remaining = strtok(buff, "\r\n"); //remove carriage return
    return remaining;
}

void ReadUartCommand::get2Params(char* params, char** param1, char** param2){
    *param1 = strtok(params, " ");
    int lenParam1 = strlen(*param1)+1;
    *param2 = getRemainingPart(params, lenParam1);
    return;
}

void ReadUartCommand::get3Params(char* params, char** param1, char** param2, char** param3){
    *param1 = strtok(params, " ");

    int lenParam1 = strlen(*param1)+1;
    char* buff = getRemainingPart(params, lenParam1);

    get2Params(buff, param2, param3);
    return;
    }


