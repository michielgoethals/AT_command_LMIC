#include "wrapLmicAt.h"

static __uint8_t _appeui[LORA_EUI_SIZE];
static __uint8_t _deveui[LORA_EUI_SIZE];
static __uint8_t _appkey[LORA_KEY_SIZE];


void WrapLmicAT::reset(int band){
    if(band == 868){
        //Serial.write("Resetted to 868\r\n");
        //set CFG_eu868 = 1
        //os_init();
        //LMIC_reset();
    }else if(band = 434){
        //Serial.println("Resetted to 434\r\n");
        //set CFG_eu434 = 1
        //os_init();
        //LMIC_reset();
    }else{
        //return invalid_param
    } 

    //disable linkcheck
}

void WrapLmicAT::tx(char* cnf, int portno, char* data){

}

void WrapLmicAT::joinOtaa(){

}

void WrapLmicAT::joinABP(){

}

void WrapLmicAT::save(){

}

void WrapLmicAT::forceEnable(){

}

void WrapLmicAT::pause(){

}

void WrapLmicAT::resume(){

}

void WrapLmicAT::setDevAddr(LoraParam devaddr){
    devAddrSet = true;
}

void WrapLmicAT::setDevEui(LoraParam deveui){
    char tempStr[3] = {0x00, 0x00, 0x00};
    for(__uint8_t i = 0; i < LORA_EUI_SIZE; i++){
    	tempStr[0] = *(deveui+(i*2));
    	tempStr[1] = *(deveui+(i*2)+1);
    	*(_deveui+i) = (__uint8_t)strtol(tempStr, NULL, 16);
    }

    devEuiSet = true;
}

void WrapLmicAT::setAppEui(LoraParam appeui){
    char tempStr[3] = {0x00, 0x00, 0x00};
    for(__uint8_t i = 0; i < LORA_EUI_SIZE; i++){
    	tempStr[0] = *(appeui+(i*2));
    	tempStr[1] = *(appeui+(i*2)+1);
    	*(_appeui+i) = (__uint8_t)strtol(tempStr, NULL, 16);
    }

    appEuiSet = true;
}

void WrapLmicAT::setNwkskey(LoraParam nwskey){
    nwksKeySet = true;
}

void WrapLmicAT::setAppsKey(LoraParam appskey){
    appsKeySet = true;
}

void WrapLmicAT::setAppKey(LoraParam appkey){
    char tempStr[3] = {0x00, 0x00, 0x00};
    for(__uint8_t i = 0; i < LORA_KEY_SIZE; i++){
    	tempStr[0] = *(appkey+(i*2));
    	tempStr[1] = *(appkey+(i*2)+1);
    	*(_appkey+i) = (__uint8_t)strtol(tempStr, NULL, 16);
    }
    
    appKeySet = true;
}

void WrapLmicAT::setPwridx(int pwrIndex){
    this->pwrIndex = pwrIndex;
}

void WrapLmicAT::setDr(int dr){
    this->dr = dr;
}

void WrapLmicAT::setAdr(char* state){
    //this->adr_state = state;
}

void WrapLmicAT::setBat(int level){
    //set battery in lmic
}

void WrapLmicAT::setRetX(int retX){
    this->retX = retX;
}

void WrapLmicAT::setLinkChk(int sec){
    //lmic function
}

void WrapLmicAT::setRxDelay1(int rxdelay1){
    this->rxdelay1 = rxdelay1;
}

void WrapLmicAT::setAr(char* state){
    //this->ar_state = state;
}

void WrapLmicAT::setRx2(int dataRate, int frequency){
    //lmic function
}

void WrapLmicAT::setChFreq(int chID, int frequency){
    //todo
}

void WrapLmicAT::setChDutyCycle(int chID, int dutyCycle){
    //todo
}

void WrapLmicAT::setChDrRange(int chID, int minRange, int maxRange){
    //todo
}

void WrapLmicAT::setChStatus(int chIDn, bool state){
    //todo
}

char* WrapLmicAT::getDevAddr(){
    //os_getDevKey(devaddr);
    //return "0";
}

char* WrapLmicAT::getDevEui(){
    //os_getDevEui(deveui);
    //return "0";
}

char* WrapLmicAT::getAppEui(){
    //os_getArtEui(appeui);
    return (char*)_appeui;
}

int WrapLmicAT::getDr(){
    return dr;
}

int WrapLmicAT::getBand(){
    return band;
}

int WrapLmicAT::getPwridx(){
    return pwrIndex;
}

char* WrapLmicAT::getAdr(){
    //return adr_state;
}

int WrapLmicAT::getRetX(){
    return retX;
}

int WrapLmicAT::getRxDelay1(){
    return rxdelay1;
}

int WrapLmicAT::getRxDelay2(){
    return rxdelay2;
}

char* WrapLmicAT::getAr(){
    //return ar_state;
}

int WrapLmicAT::getRx2(int band){
    //return rx2;
}

int WrapLmicAT::getDcycleps(){
    return dcylceps;
}

int WrapLmicAT::getMrgn(){
    return mrgn;
}

int WrapLmicAT::getGwnb(){
    return gwnb;
}   

char* WrapLmicAT::getSatus(){
    return (char*)status;
}

void WrapLmicAT::getChFreq(int chID){
}

void WrapLmicAT::getChDcycle(int chID){
}

void WrapLmicAT::getChdrrange(int chID){
}

void WrapLmicAT::getChStatus(int chID){
}