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
        mySerial.write("mac command found\r\n");
        parseMacCommand(getRemainingPart(command, len));
    }else if(strcmp(word, "sys") == 0){
        mySerial.write("sys command found\r\n");
        parseSysCommand(getRemainingPart(command, len));
    }else if(strcmp(word, "radio") == 0){
        mySerial.write("radio command found\r\n");
        parseRadioCommand(getRemainingPart(command, len));
    }else{
        mySerial.write("no command found\r\n");
    }
    //delete[] word;
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
    //delete[] word;
}

void ReadUartCommand::parseMacTxCommand(char* txCommand){
    mySerial.write(txCommand);

    char* cnf;
    char* portno; 
    char* data;

    get3Params(txCommand, &cnf, &portno, &data);

    wrapper.tx(cnf, atoi(portno), data);
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
        wrapper.setChDutyCycle(atoi(chID), atoi(dcycle));
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
    word = getRemainingPartWithoutCRNL(getCommand,0);
    int len = strlen(word)+1;

    if(strcmp(word,"devaddr")==0){
        mySerial.println(wrapper.getDevAddr());
    }else if(strcmp(word, "deveui")==0){
        mySerial.println(wrapper.getDevEui());
    }else if(strcmp(word, "appeui")==0){
        mySerial.println(wrapper.getAppEui());
    }else if(strcmp(word, "dr")==0){
        mySerial.println(wrapper.getDr());
    }else if(strcmp(word, "band")==0){
        mySerial.println(wrapper.getBand());
    }else if(strcmp(word, "pwridx")==0){
        mySerial.println(wrapper.getPwridx());
    }else if(strcmp(word, "adr")==0){
        mySerial.println(wrapper.getAdr());
    }else if(strcmp(word, "retx")==0){
        mySerial.println(wrapper.getRetX());
    }else if(strcmp(word, "rxdelay1")==0){
        mySerial.println(wrapper.getRxDelay1());
    }else if(strcmp(word, "rxdelay2")==0){
        mySerial.println(wrapper.getRxDelay2());
    }else if(strcmp(word, "ar")==0){
        mySerial.println(wrapper.getAr());
    }else if(strcmp(word, "rx2")==0){
        mySerial.println(wrapper.getRx2(atoi(getRemainingPart(getCommand,len))));
    }else if(strcmp(word, "dcycleps")==0){
        mySerial.println(wrapper.getDcycleps());
    }else if(strcmp(word, "mrgn")==0){
        mySerial.println(wrapper.getMrgn());
    }else if(strcmp(word, "gwnb")==0){
        mySerial.println(wrapper.getGwnb());
    }else if(strcmp(word, "status")==0){
        mySerial.println(wrapper.getSatus());
    }
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
    delete[] arrCRNL;
}

void ReadUartCommand::get2Params(char* params, char** param1, char** param2){
    *param1 = strtok(params, " ");
    int lenParam1 = strlen(*param1)+1;
    *param2 = getRemainingPartWithoutCRNL(params, lenParam1);
    return;
}

void ReadUartCommand::get3Params(char* params, char** param1, char** param2, char** param3){
    *param1 = strtok(params, " ");

    int lenParam1 = strlen(*param1)+1;
    char* buff = getRemainingPart(params, lenParam1);

    get2Params(buff, param2, param3);
    return;
    delete[] buff;
    }


