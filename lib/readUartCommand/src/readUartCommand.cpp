#include "readUartCommand.h"

ReadUartCommand::ReadUartCommand(){
}

void ReadUartCommand::begin(){	
    begin(DEFAULT_BAUD);
}

void ReadUartCommand::begin(int baudrate){
    Serial.begin(baudrate);
    Serial.println("Serial started @baudrate: " + String(baudrate));
    //Initialize lorawan parameters and reset LMIC library at start
    macWrapper.begin();
}

char * ReadUartCommand::getCommand(){
    static char buffer[MAX_LENGTH_MESSAGE];
    int index = 0;

    while(index < MAX_LENGTH_MESSAGE-1 && Serial.available() > 0){
        char byte = tolower(Serial.read());
        if(byte == '\n'){
            buffer[index] = '\n';
            return buffer;
        } else{
            buffer[index] = byte;
            index++;
        }
    } 

    return nullptr; 
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
    }else if(strcmp(word, "test") == 0){
        response = "test";
    }else{
        response = "invalid_param";
    }

    sendResponse(response);
    
}

void ReadUartCommand::parseMacCommand(char* command){
    char * word;
    word = strtok(command, " ");
    int len = strlen(word)+1;

    if(strcmp(word, "reset")==0){
        response = macWrapper.reset(atoi(getRemainingPart(command,len)));
    }else if(strcmp(word, "tx")==0){
        parseMacTxCommand(getRemainingPart(command, len));
    }else if(strcmp(word, "join")==0){
        parseJoinCommand(getRemainingPart(command, len));
    }else if(strcmp(word,"save")== 0){
        response = macWrapper.save();
    }else if(strcmp(word,"forceENABLE")== 0){
        response = macWrapper.forceEnable();
    }else if(strcmp(word,"pause")== 0){
        response = macWrapper.pause();
    }else if(strcmp(word,"resume")== 0){
        response = macWrapper.resume();
    }else if(strcmp(word,"set")== 0){
        parseMacSetCommand(getRemainingPart(command, len));
    }else if(strcmp(word,"get")== 0){
        parseMacGetCommand(getRemainingPart(command, len));
    }else{
        response = "invalid command";
    }

    sendResponse(response);
}

void ReadUartCommand::parseMacTxCommand(char* txCommand){
    char* cnf;
    char* portno; 
    char* data;

    get3Params(txCommand, &cnf, &portno, &data);

    response = macWrapper.tx(cnf, atoi(portno), data);

    sendResponse(response);
}

void ReadUartCommand::parseJoinCommand(char* joinMethod){
    if(strcmp(joinMethod, "otaa")==0){
        response = macWrapper.joinOtaa();
    }else if(strcmp(joinMethod, "abp")==0){
        response = macWrapper.joinABP();
    }else{
        response = "invalid_param";
    }

    sendResponse(response);
}

void ReadUartCommand::parseMacSetCommand(char* setCommand){
    char * word;
    word = strtok(setCommand, " ");
    int len = strlen(word)+1;

    char* param = getRemainingPart(setCommand, len);

    if(strcmp(word, "devaddr")==0){
        response = macWrapper.setDevAddr(param);
    }else if(strcmp(word, "deveui")==0){
        response = macWrapper.setDevEui(param);
    }else if(strcmp(word, "appeui")==0){
        response = macWrapper.setAppEui(param);
    }else if(strcmp(word, "nwkskey")==0){
        response = macWrapper.setNwkskey(param);
    }else if(strcmp(word, "appskey")==0){
        response = macWrapper.setAppsKey(param);
    }else if(strcmp(word, "appkey")==0){
        response = macWrapper.setAppKey(param);
    }else if(strcmp(word, "pwridx")==0){
        response = macWrapper.setPwridx(atoi(param));
    }else if(strcmp(word, "dr")==0){
        response = macWrapper.setDr(atoi(param));
    }else if(strcmp(word, "adr")==0){
        response = macWrapper.setAdr(param);
    }else if(strcmp(word, "bat")==0){
        macWrapper.setBat(atoi(param));
    }else if(strcmp(word, "retx")==0){
        macWrapper.setBat(atoi(param));
    }else if(strcmp(word, "linkchk")==0){
        macWrapper.setLinkChk(atoi(param));
    }else if(strcmp(word, "rxdelay1")==0){
        macWrapper.setRxDelay1(atoi(param));
    }else if(strcmp(word, "ar")==0){
        macWrapper.setAdr(param);
    }else if(strcmp(word, "rx2")==0){
        char* dr;
        char* freq;
        get2Params(getRemainingPart(setCommand,len), &dr, &freq);
        macWrapper.setRx2(atoi(dr), atoi(freq));
    }else if(strcmp(word, "ch")==0){
        parseMacSetChCommand(getRemainingPart(setCommand,len));
    }

    sendResponse(response);
}

void ReadUartCommand::parseMacSetChCommand(char* setChCommand){
    char * word;
    word = strtok(setChCommand, " ");
    int len = strlen(word)+1;

    if(strcmp(word, "freq")==0){
        char* chID;
        char* freq;
        get2Params(getRemainingPart(setChCommand,len), &chID, &freq);
        macWrapper.setChFreq(atoi(chID), atoi(freq));
    }else if(strcmp(word, "dcycle")==0){
        char* chID;
        char* dcycle;
        get2Params(getRemainingPart(setChCommand,len), &chID, &dcycle);
        macWrapper.setChDCycle(atoi(chID), atoi(dcycle));
    }else if(strcmp(word, "drrange")==0){
        char* chID;
        char* minRange;
        char* maxRange;
        get3Params(getRemainingPart(setChCommand,len), &chID, &minRange, &maxRange);
        macWrapper.setChDrRange(atoi(chID), atoi(minRange), atoi(maxRange));
    }else if(strcmp(word, "status")==0){
        char* chID;
        char* param2;
        get2Params(getRemainingPart(setChCommand,len), &chID, &param2);
        macWrapper.setChStatus(atoi(chID), param2);
    }

    sendResponse(response);
}

void ReadUartCommand::parseMacGetCommand(char* getCommand){
    char * word;
    word = strtok(getCommand, " ");
    int len = strlen(word)+1;

    if(strcmp(word,"devaddr")==0){
        sendResponse(macWrapper.getDevAddr());
    }else if(strcmp(word, "deveui")==0){
        sendResponse(macWrapper.getDevEui());
    }else if(strcmp(word, "appeui")==0){
        sendResponse(macWrapper.getAppEui());
    }else if(strcmp(word, "dr")==0){
        sendResponse(macWrapper.getDr());
    }else if(strcmp(word, "band")==0){
        sendResponse(macWrapper.getBand());
    }else if(strcmp(word, "pwridx")==0){
        sendResponse(macWrapper.getPwridx());
    }else if(strcmp(word, "adr")==0){
        sendResponse(macWrapper.getAdr());
    }else if(strcmp(word, "retx")==0){
        sendResponse(macWrapper.getRetX());
    }else if(strcmp(word, "rxdelay1")==0){
        sendResponse(macWrapper.getRxDelay1());
    }else if(strcmp(word, "rxdelay2")==0){
        sendResponse(macWrapper.getRxDelay2());
    }else if(strcmp(word, "ar")==0){
        sendResponse(macWrapper.getAr());
    }else if(strcmp(word, "rx2")==0){
        sendResponse(macWrapper.getRx2(atoi(getRemainingPart(getCommand,len))));
    }else if(strcmp(word, "dcycleps")==0){
        sendResponse(macWrapper.getDcycleps());
    }else if(strcmp(word, "mrgn")==0){
        sendResponse(macWrapper.getMrgn());
    }else if(strcmp(word, "gwnb")==0){
        sendResponse(macWrapper.getGwnb());
    }else if(strcmp(word, "status")==0){
        sendResponse(macWrapper.getSatus());
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
        sendResponse(macWrapper.getChFreq(atoi(chID)));
    }else if(strcmp(word, "dcycle")==0){
        sendResponse(macWrapper.getChDcycle(atoi(chID)));
    }else if(strcmp(word, "drrange")==0){
        sendResponse(macWrapper.getChDrrange(atoi(chID)));
    }else if(strcmp(word, "status")==0){
        sendResponse(macWrapper.getChStatus(atoi(chID)));
    }
}

void ReadUartCommand::parseSysCommand(char* command){
    char * word;
    word = strtok(command, " ");
    int len = strlen(word)+1;
    
    if(strcmp(word, "sleep")==0){
        sysWrapper.sleep();
    }else if(strcmp(word, "reset")==0){
        sysWrapper.reset(&macWrapper);
    }
}

void ReadUartCommand::parseRadioCommand(char* command){
    //radioWrapper
}

void ReadUartCommand::sendResponse(String response){
    Serial.println(response);
    response = "";
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


