#include "wrapSysAt.h"

void WrapSysAt::reset(WrapMacAt *macWrapper){
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
}

void WrapSysAt::sleep(){

}