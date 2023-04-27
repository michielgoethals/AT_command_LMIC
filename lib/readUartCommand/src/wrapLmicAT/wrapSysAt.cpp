#include "wrapSysAt.h"

//General
String WrapSysAt::sleep(){
    response = "ok";

    return response;

}

String WrapSysAt::reset(WrapMacAt *macWrapper){
    //restore band
    macWrapper->reset(*(u2_t*)EEPROM_START_ADDR_BAND);

    //restore deveui
    char deveui[LORA_EUI_SIZE*2];
    for(u1_t i = 0; i < LORA_EUI_SIZE; i++){
        sprintf(deveui+i*2, "%02X", *((char*)EEPROM_START_ADDR_DEVEUI+i));
    }
    macWrapper->setDevEui(deveui);

    //restore appeui
    char appeui[LORA_EUI_SIZE*2];
    for (u1_t i = 0; i < LORA_EUI_SIZE; i++){
        sprintf(appeui+i*2, "%02X", *((char*)EEPROM_START_ADDR_APPEUI+i));
    }
    macWrapper->setAppEui(appeui);


    //restore appkey
    char appkey[LORA_KEY_SIZE*2];
    for(u1_t i = 0; i < LORA_KEY_SIZE; i++){
        sprintf(appkey+i*2, "%02X", *((char*)(EEPROM_START_ADDR_APPKEY+i)));    
    }
    macWrapper->setAppKey(appkey);

    //restore nwkskey
    char nwkskey[LORA_KEY_SIZE*2];
    for(u1_t i = 0; i < LORA_KEY_SIZE; i++){
        sprintf(nwkskey+i*2, "%02X", *((char*)(EEPROM_START_ADDR_NWKSKEY+i)));        
    }
    macWrapper->setNwkskey(nwkskey);

    //restore appskey
    char appskey[LORA_KEY_SIZE*2];
    for(u1_t i = 0; i < LORA_KEY_SIZE; i++){
        sprintf(appskey+i*2, "%02X", *((char*)(EEPROM_START_ADDR_APPSKEY+i)));       
    }
    macWrapper->setAppsKey(appskey);

    //restore devaddr
    char devaddr[LORA_ADDR_SIZE*2];
    for(u1_t i = 0; i < LORA_ADDR_SIZE; i++){
        sprintf(devaddr+i*2, "%02X", *((char*)EEPROM_START_ADDR_DEVADDR-i-1+LORA_ADDR_SIZE));
    }
    macWrapper->setDevAddr(devaddr);

    //TO DO SET CHANNELS

    return  "ok";
}

void WrapSysAt::eraseFW(){

}

String WrapSysAt::factoryRESET(){

}


//Set commands
String WrapSysAt::setNvm(char* address, char* data){
    u2_t _address = strtol(address, NULL, 16);
    u2_t _data = strtol(data, NULL, 16);
    
    if(_address < 0x300 || _address > 0x3FF){
        response = "invalid_param";
    }else if(_data < 0x00 || _data > 0xFF){
        response = "invalid_param";
    }else{
        HAL_FLASHEx_DATAEEPROM_Unlock();
        HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE, EEPROM_START_ADDR_BAND+_address, _data);
        HAL_FLASHEx_DATAEEPROM_Lock();

        response = "ok";
    }
    
    return response;
}

String WrapSysAt::setPinDig(char* pinName, u1_t pinState){
    if(pinState != 0 && pinState != 1){
        response = "invalid_param";
    }else{
        for (u1_t i = 0; i < MAX_GPIO_PINS; i++){
            if(String(pinName) == pinNameList[i]){
                //pinMode(pinList[i], OUTPUT);
                //digitalWrite(pinList[i], pinState);
                response = "ok";
            }else{
                response = "invalid_param";
            }
        }
    } 

    return response; 
}


//Get commands
String WrapSysAt::getVer(){

}

String WrapSysAt::getNvm(char address){

}

String WrapSysAt::getVdd(){

}

//get hwei of radio module
String WrapSysAt::getHweui(){
    //u1_t hweui;
    //hal_spi_read(0x80, &hweui, 8);

}
