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
static u4_t _netid;

bool joined = false;
bool packetTx = false;

//getters for LMIC library
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

void WrapLmicAT::begin(){
    os_init();
    LMIC_reset();
    LMIC_setDrTxpow(dr,KEEP_TXPOW); //default txpow is 16dBm
    setPwridx(pwrIndex); //we set it to pwridx 1 = 14 dBm
    setRetX(retX);
    LMIC_setAdrMode(adr);
    LMIC_setLinkCheckMode(linkchk);
    LMIC.margin = mrgn;
    LMIC.dn2Dr = 3;
}

void WrapLmicAT::macReset(u2_t band){
    joined = false;
    if(band == 868 | band == 433){
        this->band = band;
        Serial.println("ok");  
    }else{
        Serial.println("invalid_param");
    }
}

void WrapLmicAT::macTx(char* cnf, u1_t portno, char* data){
    u1_t* datatx = (u1_t*)data;
    if(paused){
        Serial.println("mac_paused");
    }else if(!joined){
        Serial.println("not_joined");
    }else if (LMIC.opmode & OP_TXRXPEND){
        Serial.println("busy"); 
    }else if (sizeof(datatx)-1 > sizeof(LMIC.pendTxData)){
        Serial.println("invalid_data_len");
    }else if(portno > 0 & portno < 224){
        if(strcmp(cnf, "cnf")==0){
                LMIC_setTxData2(portno, datatx, sizeof(datatx)-1, 1); //do not change datarate as in RN module
                packetTx=true;
        }else if (strcmp(cnf, "uncnf")==0){
                LMIC_setTxData2(portno, datatx, sizeof(datatx)-1, 0); 
                packetTx=true;
        }
    }
    else{
        Serial.println("invalid_param");
    }
    
    while(packetTx){ // This makes it blocking
        os_runloop_once();
    }    
}

void WrapLmicAT::macJoinOtaa(){
    if(paused){
        Serial.println("mac_paused");
    }else if (LMIC.opmode & OP_TXRXPEND){
        Serial.println("busy"); 
    }else if(!devEuiSet && !appEuiSet && !appKeySet){
        Serial.println("keys_not_init");
    }else{
        LMIC_startJoining();
        while(!joined){
            os_runloop_once();
        }

        LMIC_getSessionKeys(&_netid, &_devaddr, _nwkskey, _appskey); 
        devAddr = String(_devaddr); //get devaddr to return in mac get devaddr

        LMIC_setLinkCheckMode(linkchk);
    }
}

void WrapLmicAT::macJoinABP(){
    if(paused){
        Serial.println("mac_paused");
    }else if (LMIC.opmode & OP_TXRXPEND) {
        Serial.println("busy"); 
    }else if(!devEuiSet && !appEuiSet && !appKeySet){
        Serial.println("keys_not_init");
    }else{
        LMIC_setSession(NET_ID,_devaddr, _nwkskey, _appskey);
        joined = true;

        //TO DO ABP joining 
    }
}

//save band, deveui, appeui, appkey, nwkskey, appskey, devaddr, ch (freq, dcycle, drrange, status) to eeprom
void WrapLmicAT::macSave(){
    HAL_FLASHEx_DATAEEPROM_Unlock();

    HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD, EEPROM_START_ADDR_BAND, band); //2 bytes

    for(u1_t i = 0; i < LORA_EUI_SIZE; i++){
        HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE, EEPROM_START_ADDR_DEVEUI+i, _deveui[i]);
        HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE, EEPROM_START_ADDR_APPEUI+i, _appeui[i]);
    }

    for(u1_t i = 0; i < LORA_KEY_SIZE; i++){
        HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE, EEPROM_START_ADDR_APPKEY+i, _appkey[i]);
        HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE, EEPROM_START_ADDR_NWKSKEY+i, _nwkskey[i]);
        HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE, EEPROM_START_ADDR_APPSKEY+i, _appskey[i]);
    }

    HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD, EEPROM_START_ADDR_DEVADDR, _devaddr); //4 bytes
    for(u1_t i = 0; i< MAX_CHANNELS; i++){
        HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD, EEPROM_START_ADDR_CH_FREQ + 9*i, LMIC.channelFreq[i]);
        HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD, EEPROM_START_ADDR_CH_DCYCLE + 9*i, LMIC.bands[LMIC.channelFreq[i] & 0x3].txcap);
        HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_HALFWORD, EEPROM_START_ADDR_CH_DRRANGE + 9*i, LMIC.channelDrMap[i]);
        HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE, EEPROM_START_ADDR_CH_STATUS + 9*i, LMIC.channelMap << i);
    }

    HAL_FLASHEx_DATAEEPROM_Lock();
    Serial.println("ok");
}

//disables silent immediatly state
void WrapLmicAT::macForceEnable(){
    Serial.println("ok");
}

//pause mac functionality replies time interval (ms) for how long it can be paused
//0 = can't be paused
//4294967295 = mac in idle 
void WrapLmicAT::macPause(){
    paused = 1;
    //TODO: calculate time and return it
}

//resume mac
void WrapLmicAT::macResume(){
    paused = 0;
    Serial.println("ok");
}

void WrapLmicAT::setDevAddr(char* devaddr){

    _devaddr = (u4_t)strtol(devaddr,NULL,16); //save to LMIC library format
    if(_devaddr < sizeof(u4_t)){
        this->devAddr = String(devaddr);
        devAddrSet = true;
        Serial.println("ok");
    }else{
        Serial.println("invalid_param");
    }
}

void WrapLmicAT::setDevEui(char* deveui){
    this->devEui = deveui;

    for(uint8_t i = 0; i < LORA_EUI_SIZE; i++){
    	tempStr[0] = *(deveui+(i*2));
    	tempStr[1] = *(deveui+(i*2)+1);
    	*(_deveui+i) = (u1_t)strtol(tempStr, NULL, 16);
    }
    devEuiSet = true;
    Serial.println("ok");
}

void WrapLmicAT::setAppEui(char* appeui){
    this->appEui = String(appeui);

    for(uint8_t i = 0; i < LORA_EUI_SIZE; i++){
    	tempStr[0] = *(appeui+(i*2));
    	tempStr[1] = *(appeui+(i*2)+1);
    	*(_appeui+i) = (u1_t)strtol(tempStr, NULL, 16);
    }
    appEuiSet = true;
    Serial.println("ok");
}

//Keys must be stored in big endian
void WrapLmicAT::setNwkskey(char* nwkskey){
    for(uint8_t i = 0; i < LORA_KEY_SIZE; i++){
    	tempStr[0] = *(nwkskey+(i*2));
    	tempStr[1] = *(nwkskey+(i*2)+1);
    	*(_nwkskey+i) = (u1_t)strtol(tempStr, NULL, 16);
    }
    nwksKeySet = true;
    Serial.println("ok");
}

void WrapLmicAT::setAppsKey(char* appskey){
    for(uint8_t i = 0; i < LORA_KEY_SIZE; i++){
    	tempStr[0] = *(appskey+(i*2));
    	tempStr[1] = *(appskey+(i*2)+1);
    	*(_appskey+i) = (u1_t)strtol(tempStr, NULL, 16);
    }
    appsKeySet = true;
    Serial.println("ok");
}

void WrapLmicAT::setAppKey(char* appkey){
    for(uint8_t i = 0; i < LORA_KEY_SIZE; i++){
    	tempStr[0] = *(appkey+(i*2));
    	tempStr[1] = *(appkey+(i*2)+1);
    	*(_appkey+i) = (u1_t)strtol(tempStr, NULL, 16);
    }
    appKeySet = true;
    Serial.println("ok");
}

void WrapLmicAT::setPwridx(u1_t pwrIndex){
    switch (pwrIndex)
    {
    case 1:
        LMIC.adrTxPow = 14;
        break;
    case 2:
        LMIC.adrTxPow = 12;
        break;
    case 3:
        LMIC.adrTxPow = 10;
        break;
    case 4:
        LMIC.adrTxPow = 8;
        break;
    case 5:
        LMIC.adrTxPow = 6;
        break;   
    default:
        if(band == 434 && pwrIndex == 0){
            LMIC.adrTxPow = 16;
        }
        else{
        }
        break;
    }
}


void WrapLmicAT::setDr(u1_t dr){
    LMIC.datarate = dr;
}

void WrapLmicAT::setAdr(char* state){
    pwrUpdated = 1;
    nbRepUpdated = 1;
    if(strcmp(state, "on")==0){
        adr = 1;
        LMIC_setAdrMode(1);
    }else if(strcmp(state, "off")==0){
        adr = 0;
        LMIC_setAdrMode(0);
    }
}

void WrapLmicAT::setBat(u1_t level){
    LMIC_setBatteryLevel(level);
}

void WrapLmicAT::setRetX(u1_t retX){
    LMIC.upRepeat = retX;
}

//TO DO seconds implementation
void WrapLmicAT::setLinkChk(u2_t sec){
    if(sec ==0){
        linkchk = 0;
        LMIC_setLinkCheckMode(0);
    }else{
        linkchk = 1;
        LMIC_setLinkCheckMode(1);
        LMIC.adrAckReq = sec;
    }
}

void WrapLmicAT::setRxDelay1(u2_t rxdelay1){
    LMIC.rxDelay = rxdelay1/1000;
}

//Automatic replay TO DO LMIC Auto reply on;
void WrapLmicAT::setAr(char* state){
    if(strcmp(state, "on")==0){
        ar = 1;
    }else if(strcmp(state, "off")==0){
        ar = 0;
    }
}

void WrapLmicAT::setRx2(u1_t dr, u4_t freq){
    RX2Updated = 1;
    LMIC.dn2Dr = dr;
    LMIC.dn2Freq = freq; 
}

void WrapLmicAT::setChFreq(u1_t chID, u4_t frequency){
    chUpdated = 1;
    LMIC.channelFreq[chID] = frequency;
}

void WrapLmicAT::setChDCycle(u1_t chID, u2_t dCycle){
    chUpdated = 1;
    u2_t actualDcycle = 100/(dCycle + 1); //in %
    u2_t txcap = 1/actualDcycle;
    u1_t const band = LMIC.channelFreq[chID] & 0x3;
    LMIC.bands[band].txcap = txcap;
    prescalerUpdated = 1;
}

void WrapLmicAT::setChDrRange(u1_t chID, u1_t minRange, u1_t maxRange){
    chUpdated = 1;
    LMIC.channelDrMap[chID] = DR_RANGE_MAP(minRange, maxRange);
}

void WrapLmicAT::setChStatus(u1_t chID, char* enable){
    chUpdated = 1;
    if(strcmp(enable, "on/r/n")==0){
        LMIC_enableChannel(chID);
    }else if(strcmp(enable, "off/r/n")==0){
        LMIC_disableChannel(chID);
    }else{
        //invalid param
    }
}

String WrapLmicAT::getDevAddr(){
    return devAddr;
}

String WrapLmicAT::getDevEui(){
    return devEui;
}

String WrapLmicAT::getAppEui(){
    return appEui;
}

u1_t WrapLmicAT::getDr(){
    return LMIC.datarate;
}

u2_t WrapLmicAT::getBand(){
    return band;
}

u1_t WrapLmicAT::getPwridx(){
    u1_t pwridx = 0;
    switch (LMIC.adrTxPow)
    {
    case 16:
        pwridx = 0;
        break;
    case 14:
        pwridx = 1;
        break;
    case 12:
        pwridx = 2;
        break;
    case 10:
        pwridx = 3;
        break;
    case 8:
        pwridx = 4;
        break;
    case 6:
        pwridx = 5;
        break;
    }
    return pwridx;
}

String WrapLmicAT::getAdr(){
    String result = "off";
    if(LMIC.adrEnabled==1){
        result = "on";
    }
    return result;
}

u1_t WrapLmicAT::getRetX(){
    return LMIC.upRepeat;
}

u2_t WrapLmicAT::getRxDelay1(){
    return LMIC.rxDelay*1000;
}

u2_t WrapLmicAT::getRxDelay2(){
    return (LMIC.rxDelay*1000)+1000;
}

//automatic reply on/off
String WrapLmicAT::getAr(){
    String result = "off";
    if(ar == 1){
        result = "on";
    }
    return result;
}

//Second receive window parameters
String WrapLmicAT::getRx2(u1_t band){
    return String(LMIC.dn2Dr) + " " + String(LMIC.dn2Freq);
}

//duty cycle prescaler TO DO
u2_t WrapLmicAT::getDcycleps(){
    return LMIC.globalDutyRate;
}

//margin
u1_t WrapLmicAT::getMrgn(){
    return LMIC.margin;
}

//number of gateways received linkchk TO DO 0 if disabled, 1-255 if enabled)
u1_t WrapLmicAT::getGwnb(){
    return gwnb;
}   

//2-byte hex representing status of module default 0x00
u2_t WrapLmicAT::getSatus(){
    //clear status reg
    status &= 0b0000000000000000;
    //bit 0: joined
    if(joined){
        status |= 0b0000000000000001;   
    }
    //bit 1,2,3: mac state   
    if(LMIC.opmode & OP_TXRXPEND){
        status |= 0b0000000000000010;
    }
    //bit 4: automatic reply
    if(ar == 1){
        status |= 0b0000000000010000;
    }
    //bit 5: ADR
    if(adr ==1){
        status |= 0b0000000000100000;
    }
    //bit 6: silent
    if(silent ==1){
        status |= 0b0000000001000000;
    }
    //bit 7: mac paused
    if(paused == 1){
        status |= 0b0000000010000000;
    }
    //bit 8: RFU
    ///???
    //bit 9: link check
    if(linkchk == 1){
        status |= 0b0000001000000000;
    }
    //bit 10: channels updated
    status |= chUpdated << 10;
    //bit 11: output power updated
    status |= pwrUpdated << 11;
    //bit 12: number of rep updated (repetitions uncnf packets)
    status |= nbRepUpdated << 12;
    //bit 13: prescaler updated
    status |= prescalerUpdated << 13;
    //bit 14: RX2 updated
    status |= RX2Updated << 14;
    //bit 15: TX timing updated
    status |= TXUpdated << 15;
    
    //reset the update status bit
    chUpdated = 0;
    pwrUpdated = 0;
    nbRepUpdated = 0;
    prescalerUpdated = 0;
    RX2Updated = 0;
    TXUpdated = 0;

    return status;
}

//Get channel frequency
u4_t WrapLmicAT::getChFreq(u1_t chID){
    return LMIC.channelFreq[chID];
}

//Get channel duty cycle
u2_t WrapLmicAT::getChDcycle(u1_t chID){
    u1_t const band = LMIC.channelFreq[chID] & 0x3;
    u2_t txcap = LMIC.bands[band].txcap;
    u2_t dCycle = 1 / txcap;
    return (100/dCycle) - 1;
}

//get
u2_t WrapLmicAT::getChDrrange(u1_t chID){
    return LMIC.channelDrMap[chID];
}

String WrapLmicAT::getChStatus(u1_t chID){
    bit_t enabled = LMIC.channelMap << chID;
    String status = "off";
    
    if(enabled == 1){
        status = "on";
    }

    return status;
}

void WrapLmicAT::sysReset(){

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
            Serial.println("ok");
            break;
        case EV_JOINED:
            Serial.println("accepted");
            joined = true;
            break;
        case EV_JOIN_FAILED:
            Serial.println(F("EV_JOIN_FAILED"));
            break;
        case EV_REJOIN_FAILED:
            Serial.println(F("EV_REJOIN_FAILED"));
            break;
        case EV_TXCOMPLETE:
            Serial.println("mac_tx_ok");
            if (LMIC.txrxFlags & TXRX_ACK)
              Serial.println("mac_rx");
            if (LMIC.dataLen) {
              Serial.println(F("Received "));
              Serial.println(LMIC.dataLen);
              Serial.println(F(" bytes of payload"));
            }
            packetTx = false;
            break;
        case EV_TXSTART:
            //packet was forwarded for transmission
            Serial.println("ok");
            break;
        case EV_TXCANCELED:
            Serial.println(F("mac_err"));
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
        case EV_RXSTART:
            break;
        case EV_JOIN_TXCOMPLETE:
            Serial.println("denied");
            break;
        default:
            Serial.print(F("Unknown event: "));
            Serial.println((unsigned) ev);
            break;
    }
}
