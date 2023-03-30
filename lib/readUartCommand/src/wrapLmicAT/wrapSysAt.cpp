#include "wrapSysAt.h"

void WrapSysAt::reset(WrapMacAt *macWrapper){
    macWrapper->reset(*(u2_t*)EEPROM_START_ADDR_BAND);

    char deveui[LORA_EUI_SIZE*2];
    char appeui[LORA_EUI_SIZE*2];

    for(u1_t i = 0; i < LORA_EUI_SIZE; i++){
        sprintf(deveui+i*2, "%02X", *((char*)EEPROM_START_ADDR_DEVEUI+i));
        sprintf(appeui+i*2, "%02X", *((char*)EEPROM_START_ADDR_APPEUI+i));
    }
    
    macWrapper->setDevEui(deveui);
    macWrapper->setAppEui(appeui);

    char appkey[LORA_KEY_SIZE*2];
    char nwkskey[LORA_KEY_SIZE*2];
    char appskey[LORA_KEY_SIZE*2];

    for(u1_t i = 0; i < LORA_KEY_SIZE; i++){
        sprintf(appkey+i*2, "%02X", *((char*)EEPROM_START_ADDR_APPKEY+i));
        sprintf(nwkskey+i*2, "%02X", *((char*)EEPROM_START_ADDR_NWKSKEY+i));
        sprintf(appskey+i*2, "%02X", *((char*)EEPROM_START_ADDR_APPSKEY+i));
    }

    macWrapper->setAppKey(appkey);
    macWrapper->setNwkskey(nwkskey);
    macWrapper->setAppsKey(appskey);

    //TO DO SET CHANNELS
}

void WrapSysAt::sleep(){

}