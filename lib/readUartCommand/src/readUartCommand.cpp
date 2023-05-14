#include "readUartCommand.h"

void ReadUartCommand::begin(int baudrate, RTC_HandleTypeDef *hrtc1){
    hrtc = *hrtc1;
    
    Serial.begin(baudrate);
    sendResponse(startupMessage);
    //reset LMIC library and restore lorawan configuration from eeprom
    macWrapper.begin();
    sysWrapper.begin(&hrtc);
}

char * ReadUartCommand::getCommand(){
    static char buffer[MAX_LENGTH_MESSAGE];
    int index = 0;

    while(index < MAX_LENGTH_MESSAGE-1 && Serial.available() > 0){
        char byte = tolower(Serial.read()); //read byte from serial port and convert to lowercase
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
        sendResponse("test");
    }else{
        sendResponse("invalid_command");
    }
}

void ReadUartCommand::parseMacCommand(char* command){
    char * word;
    word = strtok(command, " ");
    int len = strlen(word)+1;

    if(strcmp(word, "reset")==0){
        sendResponse(macWrapper.reset(atoi(getRemainingPart(command,len))));
    }else if(strcmp(word, "tx")==0){
        parseMacTxCommand(getRemainingPart(command, len));
    }else if(strcmp(word, "join")==0){
        parseJoinCommand(getRemainingPart(command, len));
    }else if(strcmp(word,"save")== 0){
        sendResponse(macWrapper.save());
    }else if(strcmp(word,"forceENABLE")== 0){
        sendResponse(macWrapper.forceEnable());
    }else if(strcmp(word,"pause")== 0){
        sendResponse(macWrapper.pause());
    }else if(strcmp(word,"resume")== 0){
        sendResponse(macWrapper.resume());
    }else if(strcmp(word,"set")== 0){
        parseMacSetCommand(getRemainingPart(command, len));
    }else if(strcmp(word,"get")== 0){
        parseMacGetCommand(getRemainingPart(command, len));
    }else{
        sendResponse("invalid_param");
    }
}

void ReadUartCommand::parseMacTxCommand(char* txCommand){
    char* cnf;
    char* portno; 
    char* data;

    get3Params(txCommand, &cnf, &portno, &data);

    sendResponse(macWrapper.tx(cnf, atoi(portno), data));
}

void ReadUartCommand::parseJoinCommand(char* joinMethod){
    if(strcmp(joinMethod, "otaa")==0){
        sendResponse(macWrapper.joinOtaa());
    }else if(strcmp(joinMethod, "abp")==0){
        sendResponse(macWrapper.joinABP());
    }else{
        sendResponse("invalid_param");
    }
}

void ReadUartCommand::parseMacSetCommand(char* setCommand){
    char * word;
    word = strtok(setCommand, " ");
    int len = strlen(word)+1;

    char* param = getRemainingPart(setCommand, len);

    if(strcmp(word, "devaddr")==0){
        sendResponse(macWrapper.setDevAddr(param));
    }else if(strcmp(word, "deveui")==0){
        sendResponse(macWrapper.setDevEui(param));
    }else if(strcmp(word, "appeui")==0){
        sendResponse(macWrapper.setAppEui(param));
    }else if(strcmp(word, "nwkskey")==0){
        sendResponse(macWrapper.setNwksKey(param));
    }else if(strcmp(word, "appskey")==0){
        sendResponse(macWrapper.setAppsKey(param));
    }else if(strcmp(word, "appkey")==0){
        sendResponse(macWrapper.setAppKey(param));
    }else if(strcmp(word, "pwridx")==0){
        sendResponse(macWrapper.setPwridx(atoi(param)));
    }else if(strcmp(word, "dnctr")==0){
        sendResponse(macWrapper.setDnCtr(atoi(param)));
    }else if(strcmp(word, "upctr")==0){
        sendResponse(macWrapper.setUpCtr(atoi(param)));
    }else if(strcmp(word, "dr")==0){
        sendResponse(macWrapper.setDr(atoi(param)));
    }else if(strcmp(word, "adr")==0){
        sendResponse(macWrapper.setAdr(param));
    }else if(strcmp(word, "bat")==0){
        int level = atoi(param);
        if (level <= 255){sendResponse(macWrapper.setBat(level));}
        else{sendResponse("invalid_param");}
    }else if(strcmp(word, "retx")==0){
        int retx = atoi(param);
        if (retx <= 255){sendResponse(macWrapper.setRetX(retx));}
        else{sendResponse("invalid_param");}
    }else if(strcmp(word, "linkchk")==0){
        int sec = atoi(param);
        if (sec <= 65535){sendResponse(macWrapper.setLinkChk(sec));}
        else{sendResponse("invalid_param");}
    }else if(strcmp(word, "rxdelay1")==0){
        int delay = atoi(param);
        if (delay <= 65535){sendResponse(macWrapper.setRxDelay1(delay));}
        else{sendResponse("invalid_param");}
    }else if(strcmp(word, "ar")==0){
        sendResponse(macWrapper.setAdr(param));
    }else if(strcmp(word, "rx2")==0){
        char* dr;
        char* freq;
        get2Params(getRemainingPart(setCommand,len), &dr, &freq);
        sendResponse(macWrapper.setRx2(atoi(dr), atoi(freq)));
    }else if(strcmp(word, "ch")==0){
        parseMacSetChCommand(getRemainingPart(setCommand,len));
    }else{
        sendResponse("invalid_param");
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
        sendResponse(macWrapper.setChFreq(atoi(chID), atoi(freq)));
    }else if(strcmp(word, "dcycle")==0){
        char* chID;
        char* dcycle;
        get2Params(getRemainingPart(setChCommand,len), &chID, &dcycle);
        sendResponse(macWrapper.setChDCycle(atoi(chID), atoi(dcycle)));
    }else if(strcmp(word, "drrange")==0){
        char* chID;
        char* minRange;
        char* maxRange;
        get3Params(getRemainingPart(setChCommand,len), &chID, &minRange, &maxRange);
        sendResponse(macWrapper.setChDrRange(atoi(chID), atoi(minRange), atoi(maxRange)));
    }else if(strcmp(word, "status")==0){
        char* chID;
        char* param2;
        get2Params(getRemainingPart(setChCommand,len), &chID, &param2);
        sendResponse(macWrapper.setChStatus(atoi(chID), param2));
    }else{
        sendResponse("invalid_param");
    }
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
    }else if(strcmp(word, "dnctr")==0){
        sendResponse(macWrapper.getDnCtr());
    }else if(strcmp(word, "upctr")==0){
        sendResponse(macWrapper.getUpCtr());
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
    }else{
        sendResponse("invalid_param");
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
    }else{
        sendResponse("invalid_param");
    }
}

void ReadUartCommand::parseSysCommand(char* command){
    char * word;
    word = strtok(command, " ");
    int len = strlen(word)+1;
    
    if(strcmp(word, "sleep")==0){
        sendResponse(sysWrapper.sleep(atoi(getRemainingPart(command,len))));
    }else if(strcmp(word, "reset")==0){
        sysWrapper.reset();
    }else if(strcmp(word, "erasefw")==0){
        sysWrapper.eraseFW();
    }else if(strcmp(word, "factoryreset")==0){
        sysWrapper.factoryRESET(&macWrapper);
    }else if(strcmp(word, "set")==0){
        parseSysSetCommand(getRemainingPart(command,len));
    }else if(strcmp(word, "get")==0){
        parseSysGetCommand(getRemainingPart(command,len));
    }else{
        sendResponse("invalid_param");
    }
}

void ReadUartCommand::parseSysSetCommand(char* setCommand){
    char * word;
    word = strtok(setCommand, " ");
    int len = strlen(word)+1;

    if(strcmp(word, "nvm")==0){
        char* address;
        char* data;
        get2Params(getRemainingPart(setCommand,len), &address, &data);
        sendResponse(sysWrapper.setNvm(address, data));
    }else if(strcmp(word, "pindig")==0){
        char* pinname;
        char* pinstate;
        get2Params(getRemainingPart(setCommand,len), &pinname, &pinstate);
        u1_t state = atoi(pinstate);
        sendResponse(sysWrapper.setPinDig(pinname, state));
    }else{
        sendResponse("invalid_param");
    }
}

void ReadUartCommand::parseSysGetCommand(char* getCommand){
    char * word;
    word = strtok(getCommand, " ");
    int len = strlen(word)+1;

    if(strcmp(word, "ver")==0){
        sendResponse(sysWrapper.getVer());
    }else if(strcmp(word, "nvm")==0){
        char* address;
        address = getRemainingPart(getCommand,len);
        sendResponse(sysWrapper.getNvm(address));
    }else if(strcmp(word, "vdd")==0){
        sendResponse(sysWrapper.getVdd());
    }else if(strcmp(word, "hweui")==0){
        sendResponse(sysWrapper.getHweui());
    }else{
        sendResponse("invalid_param");
    }
}

void ReadUartCommand::parseRadioCommand(char* command){
    char * word;
    word = strtok(command, " ");
    int len = strlen(word)+1;
    
    if(strcmp(word, "rx")==0){
        int windowSize = atoi(getRemainingPart(command,len));
        if (windowSize <= 65535){sendResponse(radioWrapper.rx(windowSize));}
        else{sendResponse("invalid_param");}
    }else if(strcmp(word, "tx")==0){
        sendResponse(radioWrapper.tx(getRemainingPart(command,len)));
    }else if(strcmp(word, "cw")==0){
        sendResponse(radioWrapper.cw(getRemainingPart(command,len)));
    }else if(strcmp(word, "set")==0){
        parseRadioSetCommand(getRemainingPart(command,len));
    }else if(strcmp(word, "get")==0){
        parseRadioGetCommand(getRemainingPart(command, len));
    }else{
        sendResponse("invalid_param");
    }
}

void ReadUartCommand::parseRadioSetCommand(char* setCommand){
    char * word;
    word = strtok(setCommand, " ");
    int len = strlen(word)+1;

    if (strcmp(word, "bt")==0){
        sendResponse(radioWrapper.setBt(getRemainingPart(setCommand,len)));
    }else if(strcmp(word, "mod")==0){
        sendResponse(radioWrapper.setMod(getRemainingPart(setCommand,len)));
    }else if(strcmp(word, "freq")==0){
        sendResponse(radioWrapper.setFreq(atoi(getRemainingPart(setCommand,len))));
    }else if(strcmp(word, "pwr")==0){
        sendResponse(radioWrapper.setPwr(atoi(getRemainingPart(setCommand,len))));
    }else if(strcmp(word, "sf")==0){
        sendResponse(radioWrapper.setSf(getRemainingPart(setCommand,len)));
    }else if(strcmp(word, "afcbw")==0){
        sendResponse(radioWrapper.setAfcBw(atoi(getRemainingPart(setCommand,len))));
    }else if(strcmp(word, "rxbw")==0){
        sendResponse(radioWrapper.setRxBw(atoi(getRemainingPart(setCommand,len))));
    }else if(strcmp(word, "bitrate")==0){
        sendResponse(radioWrapper.setBitRate(atoi(getRemainingPart(setCommand,len))));
    }else if(strcmp(word, "fdev")==0){
        sendResponse(radioWrapper.setFdev(atoi(getRemainingPart(setCommand,len))));
    }else if(strcmp(word, "prlen")==0){
        sendResponse(radioWrapper.setPrLen(atoi(getRemainingPart(setCommand,len))));
    }else if(strcmp(word, "crc")==0){
        sendResponse(radioWrapper.setCrc(getRemainingPart(setCommand,len)));
    }else if(strcmp(word, "iqi")==0){
        sendResponse(radioWrapper.setIqi(getRemainingPart(setCommand,len)));
    }else if(strcmp(word, "cr")==0){
        sendResponse(radioWrapper.setCr(getRemainingPart(setCommand,len)));
    }else if(strcmp(word, "wdt")==0){
        sendResponse(radioWrapper.setWdt(atoi(getRemainingPart(setCommand,len))));
    }else if(strcmp(word, "bw")==0){
        sendResponse(radioWrapper.setBw(atoi(getRemainingPart(setCommand,len))));
    }else{
        sendResponse("invalid_param");
    }
}   

void ReadUartCommand::parseRadioGetCommand(char* getCommand){
    char * word;
    word = strtok(getCommand, " ");
    int len = strlen(word)+1;

    if(strcmp(word, "bt")==0){
        sendResponse(radioWrapper.getBt());
    }else if(strcmp(word, "mod")==0){
        sendResponse(radioWrapper.getMod());
    }else if(strcmp(word, "freq")==0){
        sendResponse(radioWrapper.getFreq());
    }else if(strcmp(word, "pwr")==0){
        sendResponse(radioWrapper.getPwr());
    }else if(strcmp(word, "sf")==0){
        sendResponse(radioWrapper.getSf());
    }else if(strcmp(word, "afcbw")==0){
        sendResponse(radioWrapper.getAfcBw());
    }else if(strcmp(word, "rxbw")==0){
        sendResponse(radioWrapper.getRxBw());
    }else if(strcmp(word, "bitrate")==0){
        sendResponse(radioWrapper.getBitRate());
    }else if(strcmp(word, "fdev")==0){
        sendResponse(radioWrapper.getFdev());
    }else if(strcmp(word, "prlen")==0){
        sendResponse(radioWrapper.getPrLen());
    }else if(strcmp(word, "crc")==0){
        sendResponse(radioWrapper.getCrc());
    }else if(strcmp(word, "iqi")==0){
        sendResponse(radioWrapper.getIqi());
    }else if(strcmp(word, "cr")==0){
        sendResponse(radioWrapper.getCr());
    }else if(strcmp(word, "wdt")==0){
        sendResponse(radioWrapper.getWdt());
    }else if(strcmp(word, "bw")==0){
        sendResponse(radioWrapper.getBw());
    }else if(strcmp(word, "snr")==0){
        sendResponse(radioWrapper.getSnr());
    }else{
        sendResponse("invalid_param");
    }
}

void ReadUartCommand::sendResponse(String response){
    Serial.println(response);
    /* 
    response = response + "\r\n";
    HAL_UART_Transmit(&huart, (uint8_t*)response.c_str(), response.length(), HAL_MAX_DELAY);
    */
} 

void ReadUartCommand::sendResponse(int response){
    sendResponse((String)response);
}

void ReadUartCommand::sendResponseHex(int response){
    Serial.println(response, HEX);
    
    
    /* char hexResult[sizeof(response)*2+1];   

    sprintf(hexResult, "%02X", response);

    HAL_UART_Transmit(&huart, (uint8_t*)hexResult, strlen(hexResult), HAL_MAX_DELAY); */
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
}

void ReadUartCommand::get3Params(char* params, char** param1, char** param2, char** param3){
    *param1 = strtok(params, " ");

    int lenParam1 = strlen(*param1)+1;
    char* buff = getRemainingPart(params, lenParam1);

    get2Params(buff, param2, param3);
}


