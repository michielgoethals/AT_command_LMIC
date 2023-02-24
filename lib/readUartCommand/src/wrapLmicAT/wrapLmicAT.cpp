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
}

void WrapLmicAT::reset(u2_t band){
    joined = false;
    if(band == 868){
        this->band = band;
        Serial.write("Resetted to 868\r\n");
        begin();
    }else if(band == 434){
        this->band = band;
        Serial.println("Resetted to 434\r\n");
        begin();
    }else{

    } 
}

void WrapLmicAT::tx(char* cnf, u1_t portno, char* data){
    u1_t* datatx = (u1_t*)data;

    if (LMIC.opmode & OP_TXRXPEND) {
        Serial.println(F("cannot transmit busy")); 
    }else{
        if(joined == true){
            if(strcmp(cnf, "cnf")==0){
                LMIC_setTxData2(portno, datatx, sizeof(datatx)-1, 1); //do not change datarate as in RN module
                packetTx=true;
            }else if (strcmp(cnf, "uncnf")==0){
                LMIC_setTxData2(portno, datatx, sizeof(datatx)-1, 0); 
                packetTx=true;
            }
        }
    }
    while(packetTx){ // This makes it blocking
        os_runloop_once();
    }    
}

void WrapLmicAT::joinOtaa(){
    if(devEuiSet && appEuiSet && appKeySet){
        
        LMIC_startJoining();
        while(!joined){
            os_runloop_once();
        }

        LMIC_getSessionKeys(&_netid, &_devaddr, _nwkskey, _appskey); 
        //TO DO _devaddr is int convert to hex and store in string
   
    }
}

void WrapLmicAT::joinABP(){
    if(devAddrSet && nwksKeySet && appsKeySet){
        LMIC_setSession(NET_ID,_devaddr, _nwkskey, _appskey);
        if(band = 868){
            LMIC_setClockError(MAX_CLOCK_ERROR * 1 / 100);
            LMIC_setupChannel(0, 868100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
            LMIC_setupChannel(1, 868300000, DR_RANGE_MAP(DR_SF12, DR_SF7B), BAND_CENTI);      // g-band
            LMIC_setupChannel(2, 868500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
            LMIC_setupChannel(3, 867100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
            LMIC_setupChannel(4, 867300000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
            LMIC_setupChannel(5, 867500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
            LMIC_setupChannel(6, 867700000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
            LMIC_setupChannel(7, 867900000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
            LMIC_setupChannel(8, 868800000, DR_RANGE_MAP(DR_FSK,  DR_FSK),  BAND_MILLI);      // g2-band
        }

        LMIC_getSessionKeys(&_netid, &_devaddr, _nwkskey, _appskey); 

       Serial.print("netid: ");
              Serial.println(_netid, DEC);
              Serial.print("devaddr: ");
              Serial.println(_devaddr, HEX);
              Serial.print("AppSKey: ");
              for (size_t i=0; i<LORA_KEY_SIZE; ++i) {
                //_appskey[i]&= 0xff;
                Serial.print(_appskey[i], HEX);
              }
              Serial.println("");
              Serial.print("NwkSKey: ");
              for (size_t i=0; i<LORA_KEY_SIZE; ++i) {
                _nwkskey[i]&= 0xff;
                Serial.print(_nwkskey[i], HEX);
              }

            Serial.println("Joined ABP");

        joined = true;

        //TO DO ABP joining 

    }
}

//save band, deveui, appeui, appkey, nwkskey, appskey, devaddr, ch (freq, dcycle, drrange, status) to eeprom
//TO DO change eeprom address corresponding to data saved
void WrapLmicAT::save(){
    //word = 32 bits = 4 bytes
    HAL_FLASHEx_DATAEEPROM_Unlock();
    //HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD, EEPROM_ADDRESS, band); //2 bytes
    for(u1_t i = 0; i < LORA_EUI_SIZE; i++){
        //HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE, EEPROM_ADDRESS, _deveui[i]); //1 byte
        //HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE, EEPROM_ADDRESS, _appeui[i]); //1 byte
    }
    for(u1_t i = 0; i < LORA_KEY_SIZE; i++){
        //HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE, EEPROM_ADDRESS, _appkey); //1 bytes
        //HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE, EEPROM_ADDRESS, _nwkskey); //1 bytes
        //HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE, EEPROM_ADDRESS, _appskey); //1 bytes
    }
        
    //HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD, EEPROM_ADDRESS, _devaddr); //4 bytes 
    HAL_FLASHEx_DATAEEPROM_Lock();
}

void WrapLmicAT::forceEnable(){

}

//pause mac
void WrapLmicAT::pause(){
    paused = 1;
    //LMIC_shutdown();
}

//resume mac
void WrapLmicAT::resume(){
    paused = 0;
    //LMIC_reset()
}

void WrapLmicAT::setDevAddr(LoraParam devaddr){
    this->devAddr = devaddr;

    _devaddr = (u4_t)strtol(devaddr,NULL,16); //save to LMIC library format
    devAddrSet = true;
}

void WrapLmicAT::setDevEui(LoraParam deveui){
    this->devEui = deveui;

    for(uint8_t i = 0; i < LORA_EUI_SIZE; i++){
    	tempStr[0] = *(deveui+(i*2));
    	tempStr[1] = *(deveui+(i*2)+1);
    	*(_deveui+i) = (u1_t)strtol(tempStr, NULL, 16);
    }
    devEuiSet = true;
}

void WrapLmicAT::setAppEui(LoraParam appeui){
    this->appEui = appeui;

    for(uint8_t i = 0; i < LORA_EUI_SIZE; i++){
    	tempStr[0] = *(appeui+(i*2));
    	tempStr[1] = *(appeui+(i*2)+1);
    	*(_appeui+i) = (u1_t)strtol(tempStr, NULL, 16);
    }
    appEuiSet = true;
}

void WrapLmicAT::setNwkskey(LoraParam nwkskey){
    this->nwksKey = nwkskey;

    //set _nwkskey for LMIC
    for(uint8_t i = 0; i < LORA_KEY_SIZE; i++){
    	tempStr[0] = *(nwkskey+(i*2));
    	tempStr[1] = *(nwkskey+(i*2)+1);
    	*(_nwkskey+i) = (u1_t)strtol(tempStr, NULL, 16);
    }
    nwksKeySet = true;
}

void WrapLmicAT::setAppsKey(LoraParam appskey){
    this->appsKey = appskey;

    //set _appskey for LMIC
    for(uint8_t i = 0; i < LORA_KEY_SIZE; i++){
    	tempStr[0] = *(appskey+(i*2));
    	tempStr[1] = *(appskey+(i*2)+1);
    	*(_appskey+i) = (u1_t)strtol(tempStr, NULL, 16);
    }
    appsKeySet = true;
}

void WrapLmicAT::setAppKey(LoraParam appkey){
    this->appKey = appkey;

    //set _appkey for LMIC
    for(uint8_t i = 0; i < LORA_KEY_SIZE; i++){
    	tempStr[0] = *(appkey+(i*2));
    	tempStr[1] = *(appkey+(i*2)+1);
    	*(_appkey+i) = (u1_t)strtol(tempStr, NULL, 16);
    }
    appKeySet = true;
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
            //return invalid param
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

void WrapLmicAT::setAr(char* state){
    if(strcmp(state, "on")==0){
        ar = 1;
        //TO DO LMIC Auto reply on;
    }else if(strcmp(state, "off")==0){
        ar = 0;
        //TO DO LMIC Auto reply off;
    }
}

void WrapLmicAT::setRx2(u1_t dr, u4_t freq){
    RX2Updated = 1;
    LMIC.dn2Dr = dr;
    LMIC.dn2Freq = freq; 
}

void WrapLmicAT::setChFreq(u1_t chID, u4_t frequency){
    LMIC.channelFreq[chID] = frequency;
}

void WrapLmicAT::setChDutyCycle(u1_t chID, u2_t dutyCycle){
    //todo
}

void WrapLmicAT::setChDrRange(u1_t chID, int minRange, int maxRange){
    //todo
}

void WrapLmicAT::setChStatus(u1_t chID, char* enable){
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

int WrapLmicAT::getRxDelay2(){
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

String WrapLmicAT::getRx2(u1_t band){
    //TO DO return correct format
    u1_t dataRate2ReceiveWindow = LMIC.dn2Dr;
    u4_t freq2ReceiveWindow = LMIC.dn2Freq;
}

//duty cycle in pico seconds
u2_t WrapLmicAT::getDcycleps(){
    return LMIC.globalDutyRate;
}

//margin
u1_t WrapLmicAT::getMrgn(){
    return LMIC.margin;
}

//number of gateways received linkck
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
    if(OP_TXDATA){
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
}

u4_t WrapLmicAT::getChFreq(u1_t chID){
    return LMIC.channelFreq[chID];
}

u2_t WrapLmicAT::getChDcycle(u1_t chID){
    return 0;
}

void WrapLmicAT::getChdrrange(u1_t chID){
    //u2_t drRange = LMIC.channelDrMap[chID]
}

void WrapLmicAT::getChStatus(u1_t chID){
    //LMIC_enableChannel
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
            joined = true;
            break;
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
            packetTx = false;
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
        case EV_TXSTART:
            Serial.println(F("EV_TXSTART"));
            break;
        case EV_TXCANCELED:
            Serial.println(F("EV_TXCANCELED"));
            break;
        case EV_RXSTART:
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
