#include "wrapSysAt.h"

//General
String WrapSysAt::sleep(){
    response = "ok";

    return response;

}

String WrapSysAt::reset(WrapMacAt *macWrapper){
    response = macWrapper->reset(*(u2_t*)EEPROM_START_ADDR_BAND);

    //restore deveui
    char deveui[LORA_EUI_SIZE*2];
    for(u1_t i = 0; i < LORA_EUI_SIZE; i++){
        sprintf(deveui+i*2, "%02X", *((char*)EEPROM_START_ADDR_DEVEUI+i));
    }
    response = macWrapper->setDevEui(deveui);

    //restore appeui
    char appeui[LORA_EUI_SIZE*2];
    for (u1_t i = 0; i < LORA_EUI_SIZE; i++){
        sprintf(appeui+i*2, "%02X", *((char*)EEPROM_START_ADDR_APPEUI+i));
    }
    response = macWrapper->setAppEui(appeui);


    //restore appkey
    char appkey[LORA_KEY_SIZE*2];
    for(u1_t i = 0; i < LORA_KEY_SIZE; i++){
        sprintf(appkey+i*2, "%02X", *((char*)(EEPROM_START_ADDR_APPKEY+i)));    
    }
    response = macWrapper->setAppKey(appkey);

    //restore nwkskey
    char nwkskey[LORA_KEY_SIZE*2];
    for(u1_t i = 0; i < LORA_KEY_SIZE; i++){
        sprintf(nwkskey+i*2, "%02X", *((char*)(EEPROM_START_ADDR_NWKSKEY+i)));        
    }
    response = macWrapper->setNwkskey(nwkskey);

    //restore appskey
    char appskey[LORA_KEY_SIZE*2];
    for(u1_t i = 0; i < LORA_KEY_SIZE; i++){
        sprintf(appskey+i*2, "%02X", *((char*)(EEPROM_START_ADDR_APPSKEY+i)));       
    }
    response = macWrapper->setAppsKey(appskey);

    //restore devaddr
    char devaddr[LORA_ADDR_SIZE*2];
    for(u1_t i = 0; i < LORA_ADDR_SIZE; i++){
        sprintf(devaddr+i*2, "%02X", *((char*)EEPROM_START_ADDR_DEVADDR-i-1+LORA_ADDR_SIZE));
    }
    response = macWrapper->setDevAddr(devaddr);

    //TO DO SET CHANNELS

    response = "ok";

    return  response;
}

void WrapSysAt::eraseFW(){

}

String WrapSysAt::factoryRESET(){

}


//Set commands
String WrapSysAt::setNvm(char address, char data){

}

String WrapSysAt::setPinDig(char pinName, u1_t pinState){

}


//Get commands
String WrapSysAt::getVer(){

}

String WrapSysAt::getNvm(char address){

}

String WrapSysAt::getVdd(){

}

String WrapSysAt::getHweui(){

}
