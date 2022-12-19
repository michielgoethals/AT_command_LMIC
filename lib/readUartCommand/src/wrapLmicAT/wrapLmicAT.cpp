#include "wrapLmicAt.h"

const lmic_pinmap lmic_pins = {
  .nss = LMIC_NSS_PIN,
  .rxtx = LMIC_UNUSED_PIN,
  .rst = LMIC_RST_PIN,
  .dio = {LMIC_DIO0_PIN, LMIC_DIO1_PIN, LMIC_DIO2_PIN},
};

static u1_t _appeui[LORA_EUI_SIZE];
static u1_t _deveui[LORA_EUI_SIZE];
static u1_t _appkey[LORA_KEY_SIZE]; 

static u1_t _nwkskey[LORA_KEY_SIZE];
static u1_t _appskey[LORA_KEY_SIZE];
static u4_t _devaddr;

static uint8_t mydata[] = "Hello, world!";

void os_getArtEui (u1_t* buf) { // LMIC expects reverse from TTN
  for(byte i = 8; i>0; i--){
    buf[8-i] = _appeui[i-1];
  }
}

void os_getDevEui (u1_t* buf) { // LMIC expects reverse from TTN
  for(byte i = 8; i>0; i--){
    buf[8-i] = _deveui[i-1];
  }
}

void os_getDevKey (u1_t* buf) {  // no reverse here
	memcpy(buf, _appkey, 16);
} 

void WrapLmicAT::reset(int band){
    if(band == 868){
        this->band = band;
        Serial.write("Resetted to 868\r\n");
        os_init();
        LMIC_reset();
        LMIC_setLinkCheckMode(0);
        LMIC_setAdrMode(0);
    }else if(band == 434){
        this->band = band;
        //Serial.println("Resetted to 434\r\n");
        //set CFG_eu434 = 1
        //os_init();
        //LMIC_reset();
    }else{
        //return invalid_param
    } 
}

void WrapLmicAT::tx(char* cnf, int portno, char* data){
    if(strcmp(cnf, "cnf")==0){
        LMIC_setTxData2(portno, (u1_t*)data, sizeof(data)-1, 0);
    }else if (strcmp(cnf, "uncnf")==0){
        LMIC_setTxData2(portno, (u1_t*)data, sizeof(data)-1, 1);
    }
}

void WrapLmicAT::joinOtaa(){
    if(devEuiSet && appEuiSet && appKeySet){
        Serial.write("Trying to join OTAA\r\n");
        //do_send(&sendjob);
        LMIC_startJoining();
        os_runloop();
    }
}

void WrapLmicAT::joinABP(){
    if(devAddrSet && nwksKeySet && appsKeySet){
        LMIC_setSession(0x13,_devaddr, _nwkskey, _appskey);
    }
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
    _devaddr = (u4_t)strtol(devaddr,NULL,16);
    devAddrSet = true;
}

void WrapLmicAT::setDevEui(LoraParam deveui){
    for(uint8_t i = 0; i < LORA_EUI_SIZE; i++){
    	tempStr[0] = *(deveui+(i*2));
    	tempStr[1] = *(deveui+(i*2)+1);
    	*(_deveui+i) = (u1_t)strtol(tempStr, NULL, 16);
    }
    devEuiSet = true;
}

void WrapLmicAT::setAppEui(LoraParam appeui){
    for(uint8_t i = 0; i < LORA_EUI_SIZE; i++){
    	tempStr[0] = *(appeui+(i*2));
    	tempStr[1] = *(appeui+(i*2)+1);
    	*(_appeui+i) = (u1_t)strtol(tempStr, NULL, 16);
    }
    appEuiSet = true;
}

void WrapLmicAT::setNwkskey(LoraParam nwkskey){
    for(uint8_t i = 0; i < LORA_KEY_SIZE; i++){
    	tempStr[0] = *(nwkskey+(i*2));
    	tempStr[1] = *(nwkskey+(i*2)+1);
    	*(_nwkskey+i) = (u1_t)strtol(tempStr, NULL, 16);
    }
    nwksKeySet = true;
}

void WrapLmicAT::setAppsKey(LoraParam appskey){
    for(uint8_t i = 0; i < LORA_KEY_SIZE; i++){
    	tempStr[0] = *(appskey+(i*2));
    	tempStr[1] = *(appskey+(i*2)+1);
    	*(_appskey+i) = (u1_t)strtol(tempStr, NULL, 16);
    }
    appsKeySet = true;
}

void WrapLmicAT::setAppKey(LoraParam appkey){
    for(uint8_t i = 0; i < LORA_KEY_SIZE; i++){
    	tempStr[0] = *(appkey+(i*2));
    	tempStr[1] = *(appkey+(i*2)+1);
    	*(_appkey+i) = (u1_t)strtol(tempStr, NULL, 16);
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
    LMIC_setBatteryLevel(level);
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
}

char* WrapLmicAT::getDevEui(){
 
}

char* WrapLmicAT::getAppEui(){

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

void onEvent (ev_t ev) {
    Serial.print(os_getTime());
    Serial.print(": ");
    switch(ev) {
        case EV_SCAN_TIMEOUT:
            Serial.println(F("EV_SCAN_TIMEOUT"));
            break;
        case EV_BEACON_FOUND:
            Serial.println(F("EV_BEACON_FOUND"));
            break;
        case EV_BEACON_MISSED:
            Serial.println(F("EV_BEACON_MISSED"));
            break;
        case EV_BEACON_TRACKED:
            Serial.println(F("EV_BEACON_TRACKED"));
            break;
        case EV_JOINING:
            Serial.println(F("EV_JOINING"));
            break;
        case EV_JOINED:
            Serial.println(F("EV_JOINED"));
            break;
        /*
        || This event is defined but not used in the code. No
        || point in wasting codespace on it.
        ||
        || case EV_RFU1:
        ||     Serial.println(F("EV_RFU1"));
        ||     break;
        */
        case EV_JOIN_FAILED:
            Serial.println(F("EV_JOIN_FAILED"));
            break;
        case EV_REJOIN_FAILED:
            Serial.println(F("EV_REJOIN_FAILED"));
            break;
        case EV_TXCOMPLETE:
            Serial.println(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
            if (LMIC.txrxFlags & TXRX_ACK)
              Serial.println(F("Received ack"));
            if (LMIC.dataLen) {
              Serial.println(F("Received "));
              Serial.println(LMIC.dataLen);
              Serial.println(F(" bytes of payload"));
            }
            break;
        case EV_LOST_TSYNC:
            Serial.println(F("EV_LOST_TSYNC"));
            break;
        case EV_RESET:
            Serial.println(F("EV_RESET"));
            break;
        case EV_RXCOMPLETE:
            // data received in ping slot
            Serial.println(F("EV_RXCOMPLETE"));
            break;
        case EV_LINK_DEAD:
            Serial.println(F("EV_LINK_DEAD"));
            break;
        case EV_LINK_ALIVE:
            Serial.println(F("EV_LINK_ALIVE"));
            break;
        /*
        || This event is defined but not used in the code. No
        || point in wasting codespace on it.
        ||
        || case EV_SCAN_FOUND:
        ||    Serial.println(F("EV_SCAN_FOUND"));
        ||    break;
        */
        case EV_TXSTART:
            Serial.println(F("EV_TXSTART"));
            break;
        case EV_TXCANCELED:
            Serial.println(F("EV_TXCANCELED"));
            break;
        case EV_RXSTART:
            /* do not print anything -- it wrecks timing */
            break;
        case EV_JOIN_TXCOMPLETE:
            Serial.println(F("EV_JOIN_TXCOMPLETE: no JoinAccept"));
            break;
        default:
            Serial.print(F("Unknown event: "));
            Serial.println((unsigned) ev);
            break;
    }
}