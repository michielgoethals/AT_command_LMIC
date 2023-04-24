#include "wrapMacAt.h"

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
bool join_failed = false;
bool otaa = false;
bool abp = true;

//getters for LMIC library
// LMIC expects reverse from TTN
void os_getArtEui (u1_t* buf) {
    for(byte i = 8; i>0; i--){buf[8-i] = _appeui[i-1];}
}
// LMIC expects reverse from TTN
void os_getDevEui (u1_t* buf) {
    for(byte i = 8; i>0; i--){buf[8-i] = _deveui[i-1];}
}
// no reverse here
void os_getDevKey (u1_t* buf) {
    memcpy(buf, _appkey, 16); 
}
    
void WrapMacAt::begin(){
    os_init();
    LMIC_reset();
    LMIC_setDrTxpow(dr, KEEP_TXPOW); //default txpow is 16dBm
    setPwridx(pwrIndex); //we set it to pwridx 1 = 14 dBm
    setRetX(retX);
    LMIC_setAdrMode(adr);
    LMIC_setLinkCheckMode(linkchk);
    LMIC.margin = mrgn;
    LMIC.dn2Dr = 3;
}

String WrapMacAt::reset(u2_t band){
    LMIC_unjoin();
    joined = false;
    otaa = false;
    abp = false;

    begin();
    if(band == 868 | band == 433){
        this->band = band;
        response = "ok"; 
    }else{
        response = "invalid_param";
    }

    return response;
}

String WrapMacAt::tx(char* cnf, u1_t portno, char* data){
    u1_t* datatx = (u1_t*)data;
    int result = 0;

    if(paused){
        response = "mac_paused";
    }else if(!joined){
        Serial.println("not_joined");
    }else if(((portno < 224) & (strcmp(cnf, "cnf")==0)) | ((portno < 224) & (strcmp(cnf, "uncnf")==0))){
        if(strcmp(cnf, "cnf")==0){
                result = LMIC_setTxData2(portno, datatx, sizeof(datatx)-1, 1); //do not change datarate as in RN module
                packetTx=true;
        }else if(strcmp(cnf, "uncnf")==0){
                result = LMIC_setTxData2(portno, datatx, sizeof(datatx)-1, 0); 
                packetTx=true;
        }

        

    switch (result){
        case LMIC_ERROR_SUCCESS:
            response = "ok";
            break;
        case LMIC_ERROR_TX_BUSY:
            response = "busy";
            break;
        case LMIC_ERROR_TX_TOO_LARGE:
            response = "invalid_data_len";
            break;
        default:
            break;
    }

    }else{
        response = "invalid_param";
    }

    return response;

    while(packetTx){ // This makes it blocking
        os_runloop_once();
    }
}

String WrapMacAt::joinOtaa(){
    if(paused){
        response = "mac_paused";
    }else if (LMIC.opmode & OP_TXRXPEND){
        response = "busy"; 
    }else if(!(devEuiSet && appEuiSet && appKeySet)){
        response = "keys_not_init";
    }else{
        otaa = true;
        LMIC_startJoining();
        while(!joined && !join_failed){
            os_runloop_once();
        }
        
        if(joined){
            LMIC_getSessionKeys(&_netid, &_devaddr, _nwkskey, _appskey); 
            char buffer[LORA_EUI_SIZE];
            itoa(_devaddr, buffer, 16);
            devAddr = String(buffer); //get devaddr to return in mac get devaddr

            LMIC_setLinkCheckMode(linkchk);
        }

        join_failed = false;
    }

    return response;
}

String WrapMacAt::joinABP(){
    if(paused){
        response = "mac_paused";
    }else if (LMIC.opmode & OP_TXRXPEND) {
        response = "busy"; 
    }else if(!(devAddrSet && nwksKeySet && appsKeySet)){
        response = "keys_not_init";
    }else{
        //nwkskey & appskey are already set
        LMIC_setSession(NET_ID, _devaddr, NULL, NULL);
        joined = true;
        abp = true;
        
        response = "ok \r\n accepted";
        
        LMIC.seqnoDn = *(u4_t*)EEPROM_START_ADDR_FCNTDOWN;
        LMIC.seqnoUp = *(u4_t*)EEPROM_START_ADDR_FCNTUP;
    }
}

//save band, deveui, appeui, appkey, nwkskey, appskey, devaddr, ch (freq, dcycle, drrange, status) to eeprom
String WrapMacAt::save(){
    HAL_FLASHEx_DATAEEPROM_Unlock();

    HAL_FLASHEx_DATAEEPROM_Erase(EEPROM_START_ADDR_BAND);

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

    //HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD, EEPROM_START_ADDR_CH_FREQ, LMIC.channelFreq[0]);
    
    //this for loop makes endless loop
    for(u1_t i = 0; i < MAX_CHANNELS; i++){
        //HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD, EEPROM_START_ADDR_CH_FREQ + (9*i), LMIC.channelFreq[i]);
        //HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD, EEPROM_START_ADDR_CH_DCYCLE + 9*i, LMIC.bands[LMIC.channelFreq[i] & 0x3].txcap);
        //HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_HALFWORD, EEPROM_START_ADDR_CH_DRRANGE + 9*i, LMIC.channelDrMap[i]);
        //HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE, EEPROM_START_ADDR_CH_STATUS + 9*i, LMIC.channelMap << i);
    } 

    HAL_FLASHEx_DATAEEPROM_Lock();

    response ="ok";

    return response;
}

//disables silent immediatly state
String WrapMacAt::forceEnable(){
    response = "ok";

    return response;
}

//pause mac functionality replies time interval (ms) for how long it can be paused
//0 = can't be paused
//4294967295 = mac in idle 
String WrapMacAt::pause(){
    paused = 1;
    response = "ok";
    //TODO: calculate time and return it

    return response;
}

//resume mac
String WrapMacAt::resume(){
    paused = 0;
    response = "ok";

    return response;
}

String WrapMacAt::setDevAddr(char* devaddr){
    if(strlen(devaddr) < LORA_EUI_SIZE+1){
        this->devAddr = String(devaddr);

        _devaddr = (u4_t)strtol(devaddr,NULL,16);

        devAddrSet = true;
        response = "ok";
    }else{
        response = "invalid_param";
    }

    return response;
}

String WrapMacAt::setDevEui(char* deveui){
    if(strlen(deveui) < (LORA_EUI_SIZE*2)+1){
        this->devEui = String(deveui);

        for(uint8_t i = 0; i < LORA_EUI_SIZE; i++){
    	    tempStr[0] = *(deveui+(i*2));
    	    tempStr[1] = *(deveui+(i*2)+1);
    	    *(_deveui+i) = (u1_t)strtol(tempStr, NULL, 16);
        }

        devEuiSet = true;
        response = "ok";
    }else{
        response = "invalid_param";
    }

    return response;
}

String WrapMacAt::setAppEui(char* appeui){
    if(strlen(appeui) < (LORA_EUI_SIZE*2)+1){
        this->appEui = String(appeui);

        for(uint8_t i = 0; i < LORA_EUI_SIZE; i++){
            tempStr[0] = *(appeui+(i*2));
            tempStr[1] = *(appeui+(i*2)+1);
            *(_appeui+i) = (u1_t)strtol(tempStr, NULL, 16);
        }

        appEuiSet = true;
        response = "ok";
    }else{
        response = "invalid_param";
    }

    return response;
}

//Keys must be stored in big endian
String WrapMacAt::setNwkskey(char* nwkskey){
    if(strlen(nwkskey) < (LORA_KEY_SIZE*2)+1){
        for(uint8_t i = 0; i < LORA_KEY_SIZE; i++){
            tempStr[0] = *(nwkskey+(i*2));
            tempStr[1] = *(nwkskey+(i*2)+1);
            *(_nwkskey+i) = (u1_t)strtol(tempStr, NULL, 16);
            *(LMIC.nwkKey+i) = (u1_t)strtol(tempStr, NULL, 16);
        }

        nwksKeySet = true;
        response = "ok";
    }else{
        response = "invalid_param";
    }

    return response;
}

String WrapMacAt::setAppsKey(char* appskey){
    if(strlen(appskey) < (LORA_KEY_SIZE*2)+1){
        for(uint8_t i = 0; i < LORA_KEY_SIZE; i++){
            tempStr[0] = *(appskey+(i*2));
            tempStr[1] = *(appskey+(i*2)+1);
            *(_appskey+i) = (u1_t)strtol(tempStr, NULL, 16);
            *(LMIC.artKey+i) = (u1_t)strtol(tempStr, NULL, 16);
        }

        appsKeySet = true;
        response = "ok";
    }else{
        response = "invalid_param";
    }

    return response;
}

String WrapMacAt::setAppKey(char* appkey){
    if(strlen(appkey) < (LORA_KEY_SIZE*2) + 1){
        for(uint8_t i = 0; i < LORA_KEY_SIZE; i++){
    	    tempStr[0] = *(appkey+(i*2));
    	    tempStr[1] = *(appkey+(i*2)+1);
    	    *(_appkey+i) = (u1_t)strtol(tempStr, NULL, 16);
        }

        appKeySet = true;
        response = "ok";
    }else{
        response = "invalid_param";
    }

    return response;
}

String WrapMacAt::setPwridx(u1_t pwrIndex){
    response = "ok";
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
        response = "invalid_param";
        break;
    }
    
    return response;
}

String WrapMacAt::setDr(u1_t dr){
    if(dr < 8){
        LMIC.datarate = dr;
        response = "ok";
    }else{
        response = "invalid_param"; 
    }

    return response;
}

String WrapMacAt::setAdr(char* state){
    pwrUpdated = 1;
    nbRepUpdated = 1;

    response = "ok";

    if(strcmp(state, "on")==0){
        adr = 1;
        LMIC_setAdrMode(1);
    }else if(strcmp(state, "off")==0){
        adr = 0;
        LMIC_setAdrMode(0);
    }else{
        response = "invalid_param";
    }

    return response;
}

void WrapMacAt::setBat(u1_t level){
    LMIC_setBatteryLevel(level);

}

void WrapMacAt::setRetX(u1_t retX){
    LMIC.upRepeat = retX;
}

//TO DO seconds implementation
void WrapMacAt::setLinkChk(u2_t sec){
    if(sec ==0){
        linkchk = 0;
        LMIC_setLinkCheckMode(0);
    }else{
        linkchk = 1;
        LMIC_setLinkCheckMode(1);
        LMIC.adrAckReq = sec;
    }
}

void WrapMacAt::setRxDelay1(u2_t rxdelay1){
    LMIC.rxDelay = rxdelay1/1000;
}

//Automatic replay TO DO LMIC Auto reply on;
void WrapMacAt::setAr(char* state){
    if(strcmp(state, "on")==0){
        ar = 1;
    }else if(strcmp(state, "off")==0){
        ar = 0;
    }
}

void WrapMacAt::setRx2(u1_t dr, u4_t freq){
    RX2Updated = 1;
    setDr(dr);
    LMIC.dn2Freq = freq; 
}

void WrapMacAt::setChFreq(u1_t chID, u4_t frequency){
    chUpdated = 1;
    LMIC.channelFreq[chID] = frequency;
}

void WrapMacAt::setChDCycle(u1_t chID, u2_t dCycle){
    chUpdated = 1;
    u2_t actualDcycle = 100/(dCycle + 1); //in %
    u2_t txcap = 1/actualDcycle;
    u1_t const band = LMIC.channelFreq[chID] & 0x3;
    LMIC.bands[band].txcap = txcap;
    prescalerUpdated = 1;
}

void WrapMacAt::setChDrRange(u1_t chID, u1_t minRange, u1_t maxRange){
    chUpdated = 1;
    LMIC.channelDrMap[chID] = DR_RANGE_MAP(minRange, maxRange);
}

void WrapMacAt::setChStatus(u1_t chID, char* enable){
    chUpdated = 1;
    if(strcmp(enable, "on/r/n")==0){
        LMIC_enableChannel(chID);
    }else if(strcmp(enable, "off/r/n")==0){
        LMIC_disableChannel(chID);
    }else{
        //invalid param
    }
}

String WrapMacAt::getDevAddr(){
    return devAddr;
}

String WrapMacAt::getDevEui(){
    return devEui;
}

String WrapMacAt::getAppEui(){
    return appEui;
}

u1_t WrapMacAt::getDr(){
    return LMIC.datarate;
}

u2_t WrapMacAt::getBand(){
    return band;
}

u1_t WrapMacAt::getPwridx(){
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

String WrapMacAt::getAdr(){
    String result = "off";
    if(LMIC.adrEnabled==1){
        result = "on";
    }
    return result;
}

u1_t WrapMacAt::getRetX(){
    return LMIC.upRepeat;
}

u2_t WrapMacAt::getRxDelay1(){
    return LMIC.rxDelay*1000;
}

u2_t WrapMacAt::getRxDelay2(){
    return (LMIC.rxDelay*1000)+1000;
}

//automatic reply on/off
String WrapMacAt::getAr(){
    String result = "off";
    if(ar == 1){
        result = "on";
    }
    return result;
}

//Second receive window parameters
String WrapMacAt::getRx2(u1_t band){
    return String(LMIC.dn2Dr) + " " + String(LMIC.dn2Freq);
}

//duty cycle prescaler TO DO
u2_t WrapMacAt::getDcycleps(){
    return LMIC.globalDutyRate;
}

//margin
u1_t WrapMacAt::getMrgn(){
    return LMIC.margin;
}

//number of gateways received linkchk TO DO 0 if disabled, 1-255 if enabled)
u1_t WrapMacAt::getGwnb(){
    return gwnb;
}   

//2-byte hex representing status of module default 0x00
u2_t WrapMacAt::getSatus(){
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
u4_t WrapMacAt::getChFreq(u1_t chID){
    return LMIC.channelFreq[chID];
}

//Get channel duty cycle
u2_t WrapMacAt::getChDcycle(u1_t chID){
    u1_t const band = LMIC.channelFreq[chID] & 0x3;
    u2_t txcap = LMIC.bands[band].txcap;
    u2_t dCycle = 1 / txcap;
    return (100/dCycle) - 1;
}

//get
String WrapMacAt::getChDrrange(u1_t chID){
    u2_t mask = LMIC.channelDrMap[chID];

    u1_t minRange = 0;
    u1_t maxRange = 15;

    if(mask != 0){
        while (((mask >> minRange) & 1) == 0) {
        minRange++;
        }

        while (((mask >> maxRange) & 1) == 0) {
        maxRange--;
        }
    }else{
        minRange = 0;
        maxRange = 0;
    } 

    return String(minRange) + " " + String(maxRange);
}

String WrapMacAt::getChStatus(u1_t chID){
    bit_t enabled = LMIC.channelMap << chID;
    String status = "off";
    
    if(enabled == 1){
        status = "on";
    }

    return status;
}

void WrapMacAt::sysReset(){

}

void onEvent (ev_t ev) {
    switch(ev) {
        case EV_SCAN_TIMEOUT:
            Serial.println("EV_SCAN_TIMEOUT");
            break;
        case EV_BEACON_FOUND:
            Serial.println("EV_BEACON_FOUND");
            break;
        case EV_BEACON_MISSED:
            Serial.println("EV_BEACON_MISSED");
            break;
        case EV_BEACON_TRACKED:
            Serial.println("EV_BEACON_TRACKED");
            break;
        case EV_JOINING:
            Serial.println("ok");
            break;
        case EV_JOINED:
            Serial.println("accepted");
            joined = true;
            break;
        case EV_JOIN_FAILED:
            Serial.println("EV_JOIN_FAILED");
            break;
        case EV_REJOIN_FAILED:
            Serial.println("EV_REJOIN_FAILED");
            break;
        case EV_TXCOMPLETE:
            if(abp==true){
                HAL_FLASHEx_DATAEEPROM_Unlock();

                HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD, EEPROM_START_ADDR_FCNTUP, LMIC.seqnoUp); //4 bytes
                HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD, EEPROM_START_ADDR_FCNTDOWN, LMIC.seqnoDn); //4 bytes

                HAL_FLASH_Lock();
            }

            if (LMIC.txrxFlags & TXRX_ACK){ //if server ack the message
                Serial.print("mac_rx ");
                Serial.print(String(LMIC.dataBeg-1)); //port of received ack
            }else{
                Serial.println("mac_tx_ok");
            }
            packetTx = false;
            break;
        case EV_TXSTART:
            //packet was forwarded for transmission
            if(joined){
                Serial.println("ok");
            }
            break;
        case EV_TXCANCELED:
            Serial.println("mac_err");
            break;
        case EV_LOST_TSYNC:
            Serial.println("EV_LOST_TSYNC");
            break;
        case EV_RESET:
            Serial.println(F("EV_RESET"));
            break;
        case EV_RXCOMPLETE:
            // data received in ping slot
            Serial.println("EV_RXCOMPLETE");
            break;
        case EV_LINK_DEAD:
            Serial.println("EV_LINK_DEAD");
            break;
        case EV_LINK_ALIVE:
            Serial.println("EV_LINK_ALIVE");
            break;
        case EV_RXSTART:
            break;
        case EV_JOIN_TXCOMPLETE:
            Serial.println("denied");
            join_failed = true;
            break;
        default:
            Serial.print("Unknown event: ");
            Serial.println((unsigned) ev);
            break;
    }
}
