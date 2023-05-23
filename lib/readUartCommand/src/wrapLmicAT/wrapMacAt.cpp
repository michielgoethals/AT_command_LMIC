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
bool otaa = false;
bool abp = false;

String defaultKey = "00000000000000000000000000000000";
String defaultEui = "0000000000000000";
String defaultAddr = "00000000";

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

//resets LMIC state and sets up the default values    
void WrapMacAt::begin(UART_HandleTypeDef *uart){
    huart = *uart;
    LMIC_reset();
    setDefaultParameters();
    restoreConfiguration();
}

void WrapMacAt::setDefaultParameters(){
    //set default datarate to 5 = DR_SF7
    LMIC_setDrTxpow(DR_SF7, 14); //default txpow is 16dBm
    //set power index 1 = 14 dBm (max)
    setPwridx(pwrIndex); 
    // disable adr
    LMIC_setAdrMode(adr);
    //set number of retransmissions to 7
    setRetX(retX);
    // disable link check mode
    LMIC_setLinkCheckMode(linkchk);
    //set margin to 255
    LMIC.margin = mrgn;
}

//restore lorawan configuration from EEPROM
void WrapMacAt::restoreConfiguration(){
    //restore band
    reset(*(u2_t*)EEPROM_START_ADDR_BAND);

    //restore deveui
    
    char deveui[LORA_EUI_SIZE*2];
    for(u1_t i = 0; i < LORA_EUI_SIZE; i++){
        sprintf(deveui+i*2, "%02X", *((char*)EEPROM_START_ADDR_DEVEUI+i));
    }

    if(strcmp(deveui, (char*)defaultEui.c_str())!=0){
        setDevEui(deveui);
    }

    //restore appeui
    char appeui[LORA_EUI_SIZE*2];
    for (u1_t i = 0; i < LORA_EUI_SIZE; i++){
        sprintf(appeui+i*2, "%02X", *((char*)EEPROM_START_ADDR_APPEUI+i));
    }
    
    //dont check because appeui can be 0
    setAppEui(appeui);
    
    //restore appkey
    char appkey[LORA_KEY_SIZE*2];
    for(u1_t i = 0; i < LORA_KEY_SIZE; i++){
        sprintf(appkey+i*2, "%02X", *((char*)(EEPROM_START_ADDR_APPKEY+i)));    
    }

    if(strcmp(appkey, (char*)defaultKey.c_str())!=0){
        setAppKey(appkey);
    }

    //restore nwkskey
    char nwkskey[LORA_KEY_SIZE*2];
    for(u1_t i = 0; i < LORA_KEY_SIZE; i++){
        sprintf(nwkskey+i*2, "%02X", *((char*)(EEPROM_START_ADDR_NWKSKEY+i)));        
    }

    if(strcmp(nwkskey, (char*)defaultKey.c_str())!=0){
        setNwksKey(nwkskey);
    }

    //restore appskey
    char appskey[LORA_KEY_SIZE*2];
    for(u1_t i = 0; i < LORA_KEY_SIZE; i++){
        sprintf(appskey+i*2, "%02X", *((char*)(EEPROM_START_ADDR_APPSKEY+i)));       
    }

    if(strcmp(appskey, (char*)defaultKey.c_str())!=0){
        setAppsKey(appskey);
    }
    
    //restore devaddr
    char devaddr[LORA_ADDR_SIZE*2];
    for(u1_t i = 0; i < LORA_ADDR_SIZE; i++){
        sprintf(devaddr+i*2, "%02X", *((char*)EEPROM_START_ADDR_DEVADDR-i-1+LORA_ADDR_SIZE));
    }

    if(strcmp(devaddr, (char*)defaultAddr.c_str())!=0){
        setDevAddr(devaddr);
    }

    //restore upCnt
    setUpCtr(*(u4_t*)EEPROM_START_ADDR_FCNTUP);
    
    //restore dnCnt
    setDnCtr(*(u4_t*)EEPROM_START_ADDR_FCNTDOWN);

    //RESET CHANNELS
}

//reset the MAC layer to a specific band (433MHz not implemented)
String WrapMacAt::reset(u2_t band){
    LMIC_unjoin();
    joined = false;
    otaa = false;
    abp = false;
    LMIC_reset();

    if(band == 868){
        this->band = band;
        //LMICbandplan_setSessionInitDefaultChannels();
        response = "ok"; 
    }else if(band == 433){
        response = "not_implemented";
    }else{
        response = "invalid_param";
    }

    return response;
}

//Send an uplink packet with the specified data and port (cnf or uncnf)
String WrapMacAt::tx(char* cnf, u1_t portno, char* data){
    //convert char* data to u1_t* data
    size_t len = strlen(data);
    size_t size = (len + 1) / 2;
    u1_t datatx[size];

    //if odd add 0 in front of data
    if (len % 2 != 0) {
        char* paddedData = (char*)malloc((len + 2) * sizeof(char));
        paddedData[0] = '0';
        strcpy(paddedData + 1, data);
        len = len + 1;
        data = paddedData;
    }

    for (size_t i = 0; i < size; i++) {
        char temp[3] = {'\0', '\0', '\0'};
        temp[0] = data[i * 2];
        temp[1] = data[i * 2 + 1];
        datatx[i] = (u1_t)strtol(temp, NULL, 16);
    }
    
    if(paused){
        response = "mac_paused";
    }else if(!joined){
        response = "not_joined";
    }else if(((portno < 224 & portno > 0)  & (strcmp(cnf, "cnf")==0)) | ((portno < 224 & portno > 0) & (strcmp(cnf, "uncnf")==0))){
        bit_t _cnf = 0;
        if(strcmp(cnf, "cnf")==0){_cnf = 1;}
        lmic_tx_error_t result = LMIC_sendWithCallback_strict(portno, datatx, size, _cnf, [](void* pUserData, int fSuccess) {
            static_cast<WrapMacAt*>(pUserData)->eventTx(fSuccess);
        }, this);

        switch (result){
            case LMIC_ERROR_SUCCESS:
                //No error occurred, EV_TXCOMPLETE will be posted.
                response = "ok";
                break;
            case LMIC_ERROR_TX_BUSY:
                //The LMIC was busy sending another message. This message was not sent. 
                //EV_TXCOMPLETE will not be posted for this message.
                response = "busy";
                break;
            case LMIC_ERROR_TX_TOO_LARGE:
                //The queued message is too large for the any data rate for this region. 
                //This message was not sent. EV_TXCOMPLETE will not be posted for this message
                response = "invalid_data_len";
                break;
            case LMIC_ERROR_TX_NOT_FEASIBLE:
                break;
                //The queued message is not feasible for the current datarate. 
                //This message was not sent. EV_TXCOMPLETE will notbe posted for this message.
            default:
                break;
        }
    }else{
        response = "invalid_param";
    }
    
    return response;
}

void WrapMacAt::eventTx(int fsucces){
    if(fsucces == 0){
        //transmission not succesfull
        response = "mac_err";
    }else{
        //transmission succesfull
        response = "mac_tx_ok";
        if(abp){
            //save frame up counter
            HAL_FLASHEx_DATAEEPROM_Unlock();
            HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD, EEPROM_START_ADDR_FCNTUP, LMIC.seqnoUp);
            HAL_FLASHEx_DATAEEPROM_Lock();
        }
        
        LMIC_registerRxMessageCb([](void* pUserData, u1_t port, const u1_t* pMessage, size_t nMessage) {
            static_cast<WrapMacAt*>(pUserData)->eventRx(port, pMessage, nMessage);
        }, this);
    }

    response += "\r\n";
    HAL_UART_Transmit(&huart, (uint8_t*)response.c_str(), response.length(), HAL_MAX_DELAY);
}

void WrapMacAt::eventRx(u1_t port, const u1_t* pMessage, size_t nMessage){
    String response = "mac_rx ";
    response += port;
    response += " ";
    for(u1_t i = 0; i < nMessage; i++){
        response += pMessage[i];
    }
    response += "\r\n";
    HAL_UART_Transmit(&huart, (uint8_t*)response.c_str(), response.length(), HAL_MAX_DELAY);

    if(abp){
        //save frame down counter
        HAL_FLASHEx_DATAEEPROM_Unlock();
        HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD, EEPROM_START_ADDR_FCNTDOWN, LMIC.seqnoDn);
        HAL_FLASHEx_DATAEEPROM_Lock();
    }
}

//join the network with OTAA
String WrapMacAt::joinOtaa(){
    if(paused){
        response = "mac_paused";
    }else if (LMIC.opmode & OP_TXRXPEND){
        response = "busy"; 
    }else if(!(devEuiSet && appEuiSet && appKeySet)){
        response = "keys_not_init";
    }else{
        otaa = true;

        //set RX2 datarate to SF12 normal???
        LMIC_startJoining();
        //otherwise join will fail
        setDr(DR_SF9);
        response = "ok";

        LMIC_registerEventCb([](void* pUserData, ev_t ev) {
            static_cast<WrapMacAt*>(pUserData)->eventJoin(ev);
        }, this);
    }

    return response;
}

void WrapMacAt::eventJoin(ev_t ev) {
    String response;
    if (ev == EV_JOINED) {
        joined=true;
        setLinkChk(linkchk);
        response = "accepted\r\n";

        /* LMIC_getSessionKeys(&_netid, &_devaddr, _nwkskey, _appskey); 
        char buffer[LORA_EUI_SIZE];
        itoa(_devaddr, buffer, 16);
        devAddr = String(buffer); //get devaddr to return in mac get devaddr

        setDefaultParameters(); */

    }else if(ev == EV_JOIN_TXCOMPLETE){
        response = "denied\r\n";
    }
    HAL_UART_Transmit(&huart, (uint8_t*)response.c_str(), response.length(), HAL_MAX_DELAY);
}

//setup ABP session
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
        
        response = "ok\raccepted";
        
        //restore frame counters
        LMIC.seqnoDn = *(u4_t*)EEPROM_START_ADDR_FCNTDOWN;
        LMIC.seqnoUp = *(u4_t*)EEPROM_START_ADDR_FCNTUP;
    }
    return response;
}

//save band, deveui, appeui, appkey, nwkskey, appskey, devaddr, ch (freq, dcycle, drrange, status) to EEPROM
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

    //save frame counters
    HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD, EEPROM_START_ADDR_FCNTUP, LMIC.seqnoUp);
    HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD, EEPROM_START_ADDR_FCNTDOWN, LMIC.seqnoDn);

    //save channels (freq, dcycle, drrange, status)
    for(u1_t i = 0; i < MAX_CHANNELS; i++){
        HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD, EEPROM_START_ADDR_CH_FREQ+(8*i), getChFreq(i));
        HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE, EEPROM_START_ADDR_CH_DCYCLE+(8*i), getChDcycle(i));
        uint8_t drMin = (uint8_t)(getChDrrange(i).charAt(2) - '0');
        uint8_t drMax = (uint8_t)(getChDrrange(i).charAt(0) - '0');
        uint8_t drrange = (drMin << 4) | drMax;
        HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE, EEPROM_START_ADDR_CH_DRRANGE+(8*i), drrange);
        bit_t status = 0;
        if((LMIC.channelMap & (1 << i)) != 0){
            status = 1;
        }
        HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE, EEPROM_START_ADDR_CH_STATUS+(8*i), status);
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
    LMIC_shutdown();
    paused = 1;
    if((LMIC.opmode & OP_TXRXPEND)==0){
        response = "0";
    }else{
        response = "4294967295";
    }
    return response;
}

//resume mac
String WrapMacAt::resume(){
    LMIC_reset();
    paused = 0;
    response = "ok";

    return response;
}

//set device address
String WrapMacAt::setDevAddr(char* devaddr){
    if(strlen(devaddr) == (LORA_ADDR_SIZE*2)){
        this->devAddr = String(devaddr);

        _devaddr = (u4_t)strtol(devaddr,NULL,16);

        devAddrSet = true;
        response = "ok";
    }else{
        response = "invalid_param";
    }

    return response;
}

//set device EUI
String WrapMacAt::setDevEui(char* deveui){
    if(strlen(deveui) == (LORA_EUI_SIZE*2)){
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

//set application EUI
String WrapMacAt::setAppEui(char* appeui){
    if(strlen(appeui) == (LORA_EUI_SIZE*2)){
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

//set network session key
//Keys must be stored in big endian
String WrapMacAt::setNwksKey(char* nwkskey){
    if(strlen(nwkskey) == (LORA_KEY_SIZE*2)){
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

//set application session key
String WrapMacAt::setAppsKey(char* appskey){
    if(strlen(appskey) == (LORA_KEY_SIZE*2)){
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

//set application key
String WrapMacAt::setAppKey(char* appkey){
    if(strlen(appkey) == (LORA_KEY_SIZE*2)){
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

//set power index (1-5)
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

//set value of the downlink frame counter (0-4294967295)
String WrapMacAt::setDnCtr(u4_t fCntDown){
    LMIC.seqnoDn = fCntDown;
    response = "ok";

    return response;
}

//set value of the uplink frame counter (0-4294967295)
String WrapMacAt::setUpCtr(u4_t fCntUp){
    LMIC.seqnoUp = fCntUp;
    response = "ok";

    return response;
}

//set datarate (0-7)
String WrapMacAt::setDr(u1_t dr){
    if(dr < 8){
        LMIC.datarate = dr;
        response = "ok";
    }else{
        response = "invalid_param"; 
    }

    return response;
}

//enable or disable ADR
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

//set battery level (0-255)
String WrapMacAt::setBat(u1_t level){
    LMIC_setBatteryLevel(level);
    return "ok";
}

//set number of retranmission (0-255)
String WrapMacAt::setRetX(u1_t retX){
    LMIC.upRepeat = retX;
    return "ok";
}

//set link check in seconds (0 = disable, else = do check every x seconds)
//TO DO seconds implementation
String WrapMacAt::setLinkChk(u2_t sec){
    if(sec ==0){
        linkchk = 0;
        LMIC_setLinkCheckMode(0);
    }else{
        linkchk = 1;
        LMIC_setLinkCheckMode(1);
        //set timer to generate interupt after x seconds
        //if intterupt of timer set LMIC.adrAckReq to 0
        //LMIC.adrAckReq=0 
    }
    return "ok";
}

//set delay between uplink transmission and first reception slot in ms (0-65535)
String WrapMacAt::setRxDelay1(u2_t rxdelay1){
    LMIC.rxDelay = rxdelay1/1000;
    return "ok";
}

//sets the state of automatic reply to a confirmed downlink (on/off) or when frame pending bit has been set by server
//Automatic replay TO DO LMIC Auto reply on;
String WrapMacAt::setAr(char* state){
    if(strcmp(state, "on")==0){
        ar = 1;
    }else if(strcmp(state, "off")==0){
        ar = 0;
    }
    return "ok";
}

//set the datarate and frequency of the second reception slot
String WrapMacAt::setRx2(u1_t dr, u4_t freq){
    RX2Updated = 1;

    response = "ok";
    
    if(freq >= 863000000 && freq <= 870000000 && dr < 6){
        LMIC.dn2Dr = dr;
        LMIC.dn2Freq = freq; 
    }else{
        response = "invalid_param";
    }

    return response;
}

//set channel frequency for a specific channel (0-15)
String WrapMacAt::setChFreq(u1_t chID, u4_t freq){
    chUpdated = 1;

    response = "ok";

    if(freq >= 863000000 && freq <= 870000000){
        LMIC.channelFreq[chID] = freq+1;
    }else{
        response = "invalid_param";
    }

    return response;
}

//set channel duty cycle for a specific channel (0-15)
String WrapMacAt::setChDCycle(u1_t chID, u2_t dCycle){
    chUpdated = 1;
    u2_t actualDcycle = 100/(dCycle + 1); //in %
    u2_t txcap = 1/actualDcycle;
    u1_t const band = LMIC.channelFreq[chID] & 0x3;
    LMIC.bands[band].txcap = txcap;
    prescalerUpdated = 1;

    return "ok";
}

//set channel data rate range for a specific channel (0-15)
String WrapMacAt::setChDrRange(u1_t chID, u1_t minRange, u1_t maxRange){
    chUpdated = 1;
    LMIC.channelDrMap[chID] = DR_RANGE_MAP(minRange, maxRange);
    return "ok";
}

//set channel status enabled or disabled for a specific channel (0-15)
String WrapMacAt::setChStatus(u1_t chID, char* enable){
    response = "ok";
    chUpdated = 1;
    if(strcmp(enable, "on/r/n")==0){
        LMIC_enableChannel(chID);
    }else if(strcmp(enable, "off/r/n")==0){
        LMIC_disableChannel(chID);
    }else{
        response = "invalid_param";
    }

    return  response;
}

//get device address
String WrapMacAt::getDevAddr(){
    return devAddr;
}

//get device EUI
String WrapMacAt::getDevEui(){
    return devEui;
}

//get application EUI
String WrapMacAt::getAppEui(){
    return appEui;
}

//get data rate
u1_t WrapMacAt::getDr(){
    return LMIC.datarate;
}

//get configured band
u2_t WrapMacAt::getBand(){
    return band;
}

//get power index 
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

//get value of downlink frame counter that will be used for next downlink reception
u4_t WrapMacAt::getDnCtr(){
    return LMIC.seqnoDn;
}

//get value of uplink frame counter that will be used for next uplink transmission
u4_t WrapMacAt::getUpCtr(){
    return LMIC.seqnoUp;
}

//get state of ADR (on/off)
String WrapMacAt::getAdr(){
    String result = "off";
    if(LMIC.adrEnabled==1){
        result = "on";
    }
    return result;
}

//get number of retranmission
u1_t WrapMacAt::getRetX(){
    return LMIC.upRepeat;
}

//get delay between uplink transmission and first reception slot in ms
u2_t WrapMacAt::getRxDelay1(){
    return LMIC.rxDelay*1000;
}

//get delay between uplink transmission and second reception slot in ms
u2_t WrapMacAt::getRxDelay2(){
    return (LMIC.rxDelay*1000)+1000;
}

//get state of automatic reply 
String WrapMacAt::getAr(){
    String result = "off";
    if(ar == 1){
        result = "on";
    }
    return result;
}

//get second receive window parameters
String WrapMacAt::getRx2(u1_t band){

    if(band == 868){
        response = String(LMIC.dn2Dr) + " " + String(LMIC.dn2Freq);
    }else{
        response = "invalid_param";
    }
    return response;
}

//get duty cycle prescaler 
//TO DO
u2_t WrapMacAt::getDcycleps(){
    return 0;
}

//get margin
u1_t WrapMacAt::getMrgn(){
    return LMIC.margin;
}

//get number of gateways received linkchk TO DO 0 if disabled, 1-255 if enabled)
u1_t WrapMacAt::getGwnb(){
    return gwnb;
}   

//get 2-byte hex representing status of module default 0x00
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

//Get channel frequency for given channel ID (0-15)
u4_t WrapMacAt::getChFreq(u1_t chID){
    //for some reason the channel frequency is stored +1
    return LMIC.channelFreq[chID]-1;
}

//Get channel duty cycle for given channel ID (0-15)
u2_t WrapMacAt::getChDcycle(u1_t chID){
    u1_t const band = LMIC.channelFreq[chID] & 0x3;
    u2_t txcap = LMIC.bands[band].txcap;
    u2_t dCycle = 1 / txcap;
    return (100/dCycle) - 1;
}

//get channel data rate range for given channel ID (0-15)
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

// get channel status for given channel ID (0-15)
String WrapMacAt::getChStatus(u1_t chID){
    bit_t enabled = (LMIC.channelMap & (1 << chID));
    String status = "off";
    
    if(enabled != 0){
        status = "on";
    }

    return status;
}

void onEvent (ev_t ev) {
    String event;
    switch(ev){
        case EV_SCAN_TIMEOUT:
            event = "EV_SCAN_TIMEOUT";
            break;
        case EV_BEACON_FOUND:
            event = "EV_BEACON_FOUND";
            break;
        case EV_BEACON_MISSED:
            event = "EV_BEACON_MISSED";
            break;
        case EV_BEACON_TRACKED:
            event = "EV_BEACON_TRACKED";
            break;
        case EV_JOINING:
            event = "EV_JOINING";
            break;
        case EV_JOINED:
            event = "EV_JOINED";
            break;
        case EV_JOIN_FAILED:
            event = "EV_JOIN_FAILED";
            break;
        case EV_REJOIN_FAILED:
            event = "EV_REJOIN_FAILED";
            break;
        case EV_TXCOMPLETE:
            event = "EV_TXCOMPLETE";
            break;
        case EV_TXSTART:
            event = "EV_TXSTART";
            break;
        case EV_TXCANCELED:
            event = "mac_err";
            break;
        case EV_LOST_TSYNC:
            event = "EV_LOST_TSYNC";
            break;
        case EV_RESET:
            event = "EV_RESET";
            break;
        case EV_RXCOMPLETE:
            // data received in ping slot
            event = "EV_RXCOMPLETE";
            break;
        case EV_LINK_DEAD:
            event = "EV_LINK_DEAD";
            break;
        case EV_LINK_ALIVE:
            event = "EV_LINK_ALIVE";
            break;
        case EV_RXSTART:
            /* do not print anything -- it wrecks timing */
            break;
        case EV_JOIN_TXCOMPLETE:
            event = "EV_JOIN_TXCOMPLETE";
            break;
        default:
            event = "Unknown event:";
            break;

        event += "\r\n";
    }
}


