#include "wrapLmicAt.h"

static u1_t _appeui[LORA_EUI_SIZE];
static u1_t _deveui[LORA_EUI_SIZE];
static u1_t _appkey[LORA_KEY_SIZE];

static u1_t _appskey[LORA_KEY_SIZE];
static u1_t _nwkskey[LORA_KEY_SIZE];
static devaddr_t _devaddr;
static u4_t _netid;

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
}

void WrapLmicAT::tx(bool cnf, int portno, char* data){

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

}

void WrapLmicAT::setDevEui(LoraParam deveui){

}

void WrapLmicAT::setAppEui(LoraParam appeui){

}

void WrapLmicAT::setNwkskey(LoraParam nwskey){

}

void WrapLmicAT::setAppsKey(LoraParam appskey){

}

void WrapLmicAT::setAppKey(LoraParam appkey){

}


void WrapLmicAT::setPwridx(int pwrIndex){

}

void WrapLmicAT::setDr(int dataRate){

}

void WrapLmicAT::setAdr(bool state){

}

void WrapLmicAT::setBat(__uint8_t level){

}

void WrapLmicAT::setRetX(int retX){

}

void WrapLmicAT::setLinkChk(__uint16_t sec){

}

void WrapLmicAT::setRxDelay1(__uint16_t rxDelay){

}

void WrapLmicAT::setAr(bool state){

}

void WrapLmicAT::setRx2(int dataRate, int frequency){

}

void WrapLmicAT::setChFreq(int chID, int frequency){

}

void WrapLmicAT::setChDutyCycle(int chID, int dutyCycle){

}

void WrapLmicAT::setChDrRange(int chID, int minRange, int maxRange){

}

void WrapLmicAT::setChStatus(int chIDn, bool state){

}

char* WrapLmicAT::getDevAddr(){
    char * devaddr;
    //os_getDevKey(devaddr);
    return devaddr;
}

char* WrapLmicAT::getDevEui(){
    char * deveui;
    //os_getDevEui(deveui);
    return deveui;
}

char* WrapLmicAT::getAppEui(){
    char * appeui;
    //os_getArtEui(appeui);
    return appeui;
}

int WrapLmicAT::getDr(){

}

int WrapLmicAT::getBand(){
    
}

int WrapLmicAT::getPwridx(){

}

bool WrapLmicAT::getAdr(){

}

int WrapLmicAT::getRetX(){

}

int WrapLmicAT::getRxDelay1(int rxdelay){

}

int WrapLmicAT::getRxDelay2(int rxdelay){

}

bool WrapLmicAT::getAr(){

}

int WrapLmicAT::getRx2(int band){

}

int WrapLmicAT::getDcycleps(){

}

int WrapLmicAT::getMrgn(){

}

int WrapLmicAT::getGwnb(){

}

char* WrapLmicAT::getSatus(){

}

int WrapLmicAT::getChFreq(int chID){

}

int WrapLmicAT::getChDcycle(int chID){

}

int WrapLmicAT::getChdrrange(int chID){

}

bool WrapLmicAT::getChStatus(int chID){

}