#include "wrapLmicAt.h"

static u1_t _appeui[LORA_EUI_SIZE];
static u1_t _deveui[LORA_EUI_SIZE];
static u1_t _appkey[LORA_KEY_SIZE];

void WrapLmicAT::macReset(int band){
    if(band == 868){
        //set CFG_eu868 = 1
        LMIC_reset();
        os_init();
    }else if(band = 434){
        //set CFG_eu434 = 1
        LMIC_reset();
        os_init();
    }else{
        //return invalid param
    }
}

void WrapLmicAT::setAppEui(LoraParam appeui){
    for(uint8_t i = 0; i < LORA_EUI_SIZE; i++){
    	tempStr[0] = *(appeui+(i*2));
    	tempStr[1] = *(appeui+(i*2)+1);
    	*(_appeui+i) = (u1_t)strtol(tempStr, NULL, 16);
    }

    appEuiSet = true;
}

void WrapLmicAT::setDevEui(LoraParam deveui){
    for(uint8_t i = 0; i < LORA_EUI_SIZE; i++){
    	tempStr[0] = *(deveui+(i*2));
    	tempStr[1] = *(deveui+(i*2)+1);
    	*(_deveui+i) = (u1_t)strtol(tempStr, NULL, 16);
    }

}

void WrapLmicAT::setAppKey(LoraParam appkey){
    for(uint8_t i = 0; i < LORA_KEY_SIZE; i++){
    	tempStr[0] = *(appkey+(i*2));
    	tempStr[1] = *(appkey+(i*2)+1);
    	*(_appkey+i) = (u1_t)strtol(tempStr, NULL, 16);
    }
}

void WrapLmicAT::joinOtaa(){
    if(appEuiSet && devEuiSet && appKeySet){
        //try joining
    }else{
        //return response
    }
}