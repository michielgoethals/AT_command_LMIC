#include "wrapRadioAt.h"

//private functions from radio.c LMIC library
void WrapRadioAt::writeReg (u1_t addr, u1_t data ) {
    hal_spi_write(addr | 0x80, &data, 1);
}

u1_t WrapRadioAt::readReg (u1_t addr) {
    u1_t buf[1];
    hal_spi_read(addr & 0x7f, buf, 1);
    return buf[0];
}

void WrapRadioAt::writeBuf (u1_t addr, xref2u1_t buf, u1_t len) {
    hal_spi_write(addr | 0x80, buf, len);
}

void WrapRadioAt::readBuf (u1_t addr, xref2u1_t buf, u1_t len) {
    hal_spi_read(addr & 0x7f, buf, len);
}

void WrapRadioAt::requestModuleActive(bit_t state) {
    ostime_t const ticks = hal_setModuleActive(state);

    if (ticks)
        hal_waitUntil(os_getTime() + ticks);
}

void WrapRadioAt::writeOpmode(u1_t mode) {
    u1_t const maskedMode = mode & OPMODE_MASK;
    if (maskedMode != OPMODE_SLEEP)
        requestModuleActive(1);
    writeReg(RegOpMode, mode);
    if (maskedMode == OPMODE_SLEEP)
        requestModuleActive(0);
}

void WrapRadioAt::opmode (u1_t mode) {
    writeOpmode((readReg(RegOpMode) & ~OPMODE_MASK) | mode);
}

void WrapRadioAt::opmodeLora() {
    u1_t u = OPMODE_LORA;
    writeOpmode(u);
}

void WrapRadioAt::opmodeFSK() {
    u1_t u = OPMODE_FSK;
    writeOpmode(u);
}

bit_t WrapRadioAt::readOpmode(){
    u1_t opmodeRegValue = readReg(RegOpMode);
    //get MSB bit 7
    return (opmodeRegValue >> 7);
}

float WrapRadioAt::calculateBw(u1_t regBwValue){
    u1_t rxBwMant = (regBwValue >> 3) & 0x1F; //bit 4-3
    u1_t rxBwExp = regBwValue & 0x07; //bit 2-0

    float rxBw = 0;

    for(u1_t i = 0; i < MAX_BW_VALUES; i++){
        if(rxBwMant == bwMantList[i] && rxBwExp == bwExpList[i]){
            rxBw = bwList[i];
            break;
        }
    }
    return rxBw;
}

//number of symbols (for LoRa) or time out (for FSK) that receiver will be opened (0-65535)
//0 is Continuous reception mode
String WrapRadioAt::rx(u2_t windowSize){
    response = "ok";
    if((LMIC.opmode & OP_SHUTDOWN)==0){
        if(windowSize==0){
            //set opmode to continuous reception mode
            opmode(OPMODE_RX);
        }else{
            //set opmode to single reception mode
            opmode(OPMODE_RX_SINGLE);
        }
    }else{
        response = "busy";
    } 
    return response;
}

//hex value representing data to be transmitted
//from 0-255 bytes (for LoRa) and 0-64 bytes (for FSK)
String WrapRadioAt::tx(char* data){
    //make sure mac functionality is paused
    response = "ok";
    if((LMIC.opmode & OP_SHUTDOWN)== 0){
        // write data to fifo register
        writeBuf(RegFifo, (u1_t*)data, strlen(data));

        //check which mode selected
        //if LoRa
        if(readOpmode() == 1){
            if(strlen(data) <= 255){
                opmode(OPMODE_TX);
            }else{
                response = "invalid_param";
            }
        }else if(readOpmode() == 0){
            if(strlen(data) <= 64){
                opmode(OPMODE_TX);
            }else{
                response = "invalid_param";
            }
        }else{
            response = "invalid_param";
        }
    }else{
        response = "busy";
    }
    return response;
}

//enable or disbale continuous wave mode
//allows user to put transceiver in transmission mode to observe the generated signal
String WrapRadioAt::cw(char* state){
    response = "ok";
    if(strcmp(state, "on") == 0){
        // Only for testing
        // set ModemConfig2 (sf, TxContinuousMode=1, AgcAutoOn=1 SymbTimeoutHi=00)
        mc2 |= 0x8;
        writeReg(LORARegModemConfig2, mc2);
    }else if(strcmp(state, "off") == 0){
        NVIC_SystemReset();
    }else{
        response = "invalid_param";
    }
    return response;
}

//enables GFSK gaussian baseband data shaping (none, 1.0, 0.5, 0.3))
//modifies the data shaping applied to FSK transmissions
String WrapRadioAt::setBt(char* gfBT){
    response = "ok";
    //bit 0-3 is ramp time: default is 40us (0x09) = 1001
    //bit 4 is reserverd = 0
    //bit 5-6 = 00 = none
    //bit 5-6 = 01 = 1.0
    //bit 5-6 = 10 = 0.5
    //bit 5-6 = 11 = 0.3
    //bit 7 is unused = 0
    if(strcmp(gfBT, "none") == 0){
        //0000 1001 = 0x09
        writeReg(RegPaRamp, 0x09);
    }else if(strcmp(gfBT, "1.0") == 0){
        //0010 1001 = 0x29
        writeReg(RegPaRamp, 0x29);
    }else if(strcmp(gfBT, "0.5") == 0){
        //0100 1001 = 0x49
        writeReg(RegPaRamp, 0x49);
    }else if(strcmp(gfBT, "0.3") == 0){
        //0110 1001 = 0x69
        writeReg(RegPaRamp, 0x69);
    }else{
        response = "invalid_param";
    }
    return response;        
}

//changing the modulation mode (LoRa or FSK)
String WrapRadioAt::setMod(char* mode){
    response = "ok";

    if(strcmp(mode, "lora") == 0){ 
        opmodeLora();
    }else if(strcmp(mode, "fsk") == 0){
        opmodeFSK();      
    }else{
        response = "invalid_param";
    }
    return response;
}

//set communication frequency of the radio transceiver (863000000-870000000)
String WrapRadioAt::setFreq(u4_t freq){
    response = "ok";
    if(freq >= 863000000 && freq <= 870000000){
        //Fstep = Fxtal / 2^19 = 32 Mhz / 2^19 = 61.03515625 Hz
        //set frequency: Freq = Fstep * Fregister => Fregister = Freq / Fstep
        //Frequency to set in register is 24 bits = freq * 2^19 / 32Mhz
        uint64_t frf = ((uint64_t)freq << 19) / 32000000;
        writeReg(RegFrfMsb, (u1_t)(frf>>16)); //get MSB bits(16-23)
        writeReg(RegFrfMid, (u1_t)(frf>> 8)); //get Mid bits(8-15)
        writeReg(RegFrfLsb, (u1_t)(frf>> 0)); //get LSB bits(0-7)
    }else{
        response = "invalid_param";
    }
    return response;
}

//set transmission output power (-3-15)
String WrapRadioAt::setPwr(s1_t pwrout){
    //RegPaConfig
    //bit 7 PaSelect = 1: use PA_BOOST pin max. 20dBm
    //bit 7 PaSelect = 0: use RFO pin max. 14dBm
    //bit 6-4 MaxPower: Pmax = 10.8+0.6*MaxPower [dBm]
    //bit 3-0 OutputPower: Pout = 17-(15-OutputPower) if PaSelect = 1
    //bit 3-0 OutputPower: Pout = PMax-(15-OutputPower) if PaSelect = 0

    u1_t rPaConfig = 0;
    u1_t rPaDac = SX127X_PADAC_POWER_NORMAL;
    u1_t rOcp = SX127X_OCP_MAtoBITS(100);

    response = "ok";

    if(pwrout >= -3 && pwrout <= 15){
        rPaConfig = pwrout | SX1276_PAC_MAX_POWER_MASK;

        writeReg(RegPaConfig, rPaConfig);
        writeReg(RegPaDac, (readReg(RegPaDac) & ~SX127X_PADAC_POWER_MASK) | rPaDac);
        writeReg(RegOcp, rOcp | SX127X_OCP_ENA);

    }else{
        response = "invalid_param";
    }
    return response;
}

// set spreading factor (sf7 - sf12)
String WrapRadioAt::setSf(char* spreadingFactor){
    response = "ok";

    //regModemConfig2
    // bits 7-4 spreading factor
    // bits 7-4 0x07 = 7 = sf7
    // bits 7-4 0x08 = 8 = sf8
    // bits 7-4 0x09 = 9 = sf9
    // bits 7-4 0x10 = 10 = sf10
    // bits 7-4 0x11 = 11 = sf11
    // bits 7-4 0x12 = 12 = sf12

    //clear bits 7-4
    mc2 &= 0x0F;
  
    if(strcmp(spreadingFactor, "sf7") == 0){
        mc2 |= SX1272_MC2_SF7;
    }else if(strcmp(spreadingFactor, "sf8") == 0){
        mc2 |= SX1272_MC2_SF8;
    }else if(strcmp(spreadingFactor, "sf9") == 0){
        mc2 |= SX1272_MC2_SF9;
    }else if(strcmp(spreadingFactor, "sf10") == 0){
        mc2 |= SX1272_MC2_SF10;
    }else if(strcmp(spreadingFactor, "sf11") == 0){
        mc2 |= SX1272_MC2_SF11;
    }else if(strcmp(spreadingFactor, "sf12") == 0){
        mc2 |= SX1272_MC2_SF12;
    }else{
        response = "invalid_param";
    }
    writeReg(LORARegModemConfig2, mc2);
    return response;
}

// set automatic frequency correction bandwidth in kHz
// parameters can be 250, 125, 62.5, 31.3, 15.6, 7.8, 3.9,
// 200, 100, 50, 25, 12.5, 6.3, 3.1, 
// 166.7, 83.3, 41.7, 20.8, 10.4, 5.2, 2.6
String WrapRadioAt::setAfcBw(float autoFreqBand){
   //RxBw = Fxtal / RxBwMant * 2^(RxBwExp+2)
    //RxBWMantAfc (bit 4-3) and RxBWExpAfc (bit 2-0)

    for(int i = 0; i < MAX_BW_VALUES; i++){
        if(autoFreqBand == bwList[i]){
            //set auto frequency correction bandwidth
            i = MAX_BW_VALUES;
            response = "ok";
            u1_t rxBwMant = bwMantList[i];
            u1_t rxBwExp = bwExpList[i];
            u1_t rxBwvalue = (rxBwMant << 3) | rxBwExp;
            writeReg(FSKRegAfcBw, rxBwvalue);
        }else{
            response = "invalid_param";
        }
    }
    return response;
}

// set receiving signal bandwidth in kHz
String WrapRadioAt::setRxBw(float rxBandWidth){
    //RxBw = Fxtal / RxBwMant * 2^(RxBwExp+2)
    //RxBWMant (bit 4-3) and RxBWExp (bit 2-0)
  
    for(int i = 0; i < MAX_BW_VALUES; i++){
        if(rxBandWidth == bwList[i]){
            //set auto frequency correction bandwidth
            i = MAX_BW_VALUES;
            response = "ok";
            u1_t rxBwMant = bwMantList[i];
            u1_t rxBwExp = bwExpList[i];
            u1_t rxBwValue = (rxBwMant << 3) | rxBwExp;
            writeReg(FSKRegRxBw, rxBwValue);
        }else{
            response = "invalid_param";
        }
    }
    return response;
}

// set FSK bit rate value in bits per second (0 - 65535)
String WrapRadioAt::setBitRate(u2_t fskBitRate){
    response = "ok";
    
    //fskBitRate = Fxtal / BitRateRegValue+ BitRateFrac/16 (BitRateFrac = 0x00 default)
    //fskBitRate = Fxtal / BitRateRegValue
    //BitRateRegValue = Fxtal / fskBitRate = 32Mhz / fskBitRate

    //set bit rate
    u2_t bitRateRegValue = 32000000 / fskBitRate;
    writeReg(FSKRegBitrateMsb, (u1_t)(bitRateRegValue >> 8)); //get the 8 most significant bits
    writeReg(FSKRegBitrateLsb, (u1_t)(bitRateRegValue >> 0)); //get the 8 least significant bits
   
    return response;
}

//set frequency deviation in kHz (0 - 65535)
String WrapRadioAt::setFdev(u2_t freqDev){
    response = "ok";

    //Fdev = Fstep * FdevRegister(15:0) 
    //FdevRegister(15:0) = Fdev / Fstep = freqDev / Fxtal / 2^19 = freqDev / 61.03515625
    
    // set frequency deviation
    u2_t fdevRegValue = freqDev / 32000000 / 2^19;
    writeReg(FSKRegFdevMsb, (u1_t)(fdevRegValue >> 8)); //get the 8 most significant bits
    writeReg(FSKRegFdevLsb, (u1_t)(fdevRegValue >> 0)); //get the 8 least significant bits

    return  response;
}

//set preamble length (0 - 65535)
String WrapRadioAt::setPrLen(u2_t preamble){
    response = "ok";

    writeReg(LORARegPreambleMsb, (u1_t)(preamble >> 8)); //get the 8 most significant bits
    writeReg(LORARegPreambleLsb, (u1_t)(preamble >> 0)); //get the 8 least significant bits

    //write also preamble length for FSK????
    writeReg(FSKRegPreambleMsb, (u1_t)(preamble >> 8)); //get the 8 most significant bits
    writeReg(FSKRegPreambleLsb, (u1_t)(preamble >> 0)); //get the 8 least significant bits

    return  response;
}

//set CRC header type (on, off)
String WrapRadioAt::setCrc(char* crcHeader){
    response = "ok";

    if(strcmp(crcHeader, "on") == 0){
        mc2 |= SX1276_MC2_RX_PAYLOAD_CRCON;
    }else if(strcmp(crcHeader, "off") == 0){
        //clear bit 2
        mc2 &= ~SX1276_MC2_RX_PAYLOAD_CRCON;
    }else{
        response = "invalid_param";
    }
    writeReg(LORARegModemConfig2, mc2);
    return  response;
}

//set IQ inversion (on, off)
String WrapRadioAt::setIqi(char* iqInvert){
    response = "ok";

    //bit 6 = 1: invert I and Q signals in RX path
    //bit 0 = 1: invert I and Q signals in TX path

    if (strcmp(iqInvert, "on")==0){
        writeReg(LORARegInvertIQ, (1<<6));
        writeReg(LORARegInvertIQ, (1<<0));
    }else if (strcmp(iqInvert, "off")==0){
        writeReg(LORARegInvertIQ, (0<<6));
        writeReg(LORARegInvertIQ, (0<<0));
    }else{
        response = "invalid_param";
    }
    return  response;
}

//set coding rate (4/5, 4/6, 4/7, 4/8)
String WrapRadioAt::setCr(char* codingRate){
    response = "ok";

    //clear bits 3-1
    mc1 &= 0xF1;

    if (strcmp(codingRate, "4/5") == 0){
        mc1 |= SX1276_MC1_CR_4_5;
    }else if(strcmp(codingRate, "4/6") == 0){
        mc1 |= SX1276_MC1_CR_4_6;
    }else if(strcmp(codingRate, "4/7") == 0){
        mc1 |= SX1276_MC1_CR_4_7;
    }else if(strcmp(codingRate, "4/8") == 0){
        mc1 |= SX1276_MC1_CR_4_8;
    }else{
        response = "invalid_param";
    }
    writeReg(LORARegModemConfig1, mc1);
    return response;
}

//set time-out lenght for the watch dog timer in ms (0 - 4294967295)
// 0 is disabled 
String WrapRadioAt::setWdt(u4_t watchDog){
    response = "ok";
    return response;  
}

//set sync word during communication
//for lora one byte is used
//for fsk up to eight bytes can be used
String WrapRadioAt::setSync(char* syncWord){
    response = "ok";
    u1_t nbOfBytes = strlen(syncWord); // max 8 bytes

    //auto restart off (bit 7-6) = 1
    //preamble polarity default (bit 5) = 0xAA, 
    //disable sync word generation enable (bit 4) = 0, 
    // reserved (bit 3) = 0
    //syncSize (bit 2-0)= nbOfBytes
    u1_t regSyncConfigValue = 11000000 | nbOfBytes; // no auto restart, preamble 0xAA, disable, fill FIFO, nbOfBytes sync
    writeReg(FSKRegSyncConfig, regSyncConfigValue); 

    // set sync word LoRa
    writeReg(LORARegSyncWord, syncWord[0]);

    // set sync value FSK
    for (u1_t i = 0; i < nbOfBytes; i++)
    {
        writeReg(FSKRegSyncValue1 + i, syncWord[i]);
    }
    return response;
}   

// set operating radio bandwith in kHz (125, 250, 500)
String WrapRadioAt::setBw(u2_t bandWidth){
    response = "ok";

    //clear bits 7-4
    mc1 &= 0x0F;

    if(bandWidth == 125){
        mc1 |= SX1276_MC1_BW_125;
        //set bandwidth to 125 kHz
    }else if(bandWidth == 250){
        mc1 |= SX1276_MC1_BW_250;
        //set bandwidth to 250 kHz
    }else if(bandWidth == 500){
        mc1 |= SX1276_MC1_BW_500;
        //set bandwidth to 500 kHz
    }else{
        response = "invalid_param";
    }
    writeReg(LORARegModemConfig1, mc1);
    return response;
}

//get data shaping configuration
String WrapRadioAt::getBt(){
    u1_t bt = readReg(RegPaRamp);
    String btString;
    
    if(bt == 0x09){
        btString = "none";
    }else if(bt == 0x29){
        btString = "1.0";
    }else if(bt == 0X49){
        btString = "0.5";
    }else if(bt == 0x69){
        btString = "0.3";
    }else{
        btString = "no_valid_bt_found";
    }

    return btString;
}

//get current operation mode of the radio lora of fsk
String WrapRadioAt::getMod(){
    String mode;
    if(readOpmode() == 1){
        mode = "lora";
    }else if(readOpmode() == 0){
        mode = "fsk";
    }else{
        mode = "no_valid_mode_found";
    }

    return mode;
}

// get current frequency of the radio
u4_t WrapRadioAt::getFreq(){
    u4_t freq24_bit = (readReg(RegFrfMsb) << 16) | (readReg(RegFrfMid) << 8) | readReg(RegFrfLsb);

    u4_t freq = freq24_bit * 32000000 / 2^19;

    return freq;
}

//get current power level settings of the radio
s1_t WrapRadioAt::getPwr(){

    u1_t pwrReg = readReg(RegPaConfig);

    s1_t pwr = pwrReg & 0x0F;
    
    return pwr;
}

//get current spreading factor
String WrapRadioAt::getSf(){
    u2_t sfConfig = readReg(LORARegModemConfig2) & 0xF0;

    String sf;

    if(sfConfig == SX1272_MC2_SF7){
        sf = "7";
    }else if(sfConfig == SX1272_MC2_SF8){
        sf = "8";
    }else if(sfConfig == SX1272_MC2_SF9){
        sf = "9";
    }else if(sfConfig == SX1272_MC2_SF10){
        sf = "10";
    }else if(sfConfig == SX1272_MC2_SF11){
        sf = "11";
    }else if(sfConfig == SX1272_MC2_SF12){
        sf = "12";
    }else{
        sf = "no_valid_sf";
    }
    return sf;
}

//get current automatic frequency correction bandwidth
float WrapRadioAt::getAfcBw(){
    u1_t afcBwvalue = readReg(FSKRegAfcBw);
    float afcBw = calculateBw(afcBwvalue); 
    return afcBw;
}

//get current receiving signal bandwidth in kHz
float WrapRadioAt::getRxBw(){
    u1_t rxBwvalue = readReg(FSKRegRxBw);
    float rxBw = calculateBw(rxBwvalue);
    return rxBw;
}

//get current bit rate
u2_t WrapRadioAt::getBitRate(){
    u1_t bitRateMsb = readReg(FSKRegBitrateMsb);
    u1_t bitRateLsb = readReg(FSKRegBitrateLsb);

    u2_t bitRate = (bitRateMsb << 8) | bitRateLsb;

    return bitRate;
}

//get current frequency deviation
u2_t WrapRadioAt::getFdev(){
    u1_t fdevMsb = readReg(FSKRegFdevMsb);
    u1_t fdevLsb = readReg(FSKRegFdevLsb);

    u2_t fdev = (fdevMsb << 8) | fdevLsb;

    return fdev;
}

//get current preamble length
u2_t WrapRadioAt::getPrLen(){
    u1_t prMsb = readReg(LORARegPreambleMsb);
    u1_t prLSB = readReg(LORARegPreambleLsb);

    u2_t prLength = (prMsb << 8) | prLSB;

    return prLength;
}

// get current CRC header type (on, off)
String WrapRadioAt::getCrc(){
    u1_t crcConfig = readReg(LORARegModemConfig2) & 0x04;
    String crc;

    if (crcConfig==SX1276_MC2_RX_PAYLOAD_CRCON){
        crc = "on";
    }else{
        crc = "off";
    }
    return crc;
}

//get current IQ inversion (on, off)
String WrapRadioAt::getIqi(){
    //get bit 6 and 0 of register 0x33
    u1_t iqi = readReg(LORARegInvertIQ) & 0x41;

    if(iqi == 0x41){
        response =  "on";
    }else{
        response = "off";
    }
    
    return response;
}

//get current coding rate (4/5, 4/6, 4/7, 4/8)
String WrapRadioAt::getCr(){
    u1_t crConfig = readReg(LORARegModemConfig1) & 0x0E;
    String cr;

    if (crConfig == SX1276_MC1_CR_4_5){
        cr = "4/5";
    }else if(crConfig == SX1276_MC1_CR_4_6){
        cr = "4/6";
    }else if(crConfig == SX1276_MC1_CR_4_7){
        cr = "4/7";
    }else if(crConfig == SX1276_MC1_CR_4_8){
        cr = "4/8";
    }else{
        cr = "not_found";
    }
    return cr;
}

//get current watch dog timer time-out lenght in ms
u4_t WrapRadioAt::getWdt(){
    return 0;
}

// get bandwidth of the radio
u2_t WrapRadioAt::getBw(){
    u1_t bwConfig = readReg(LORARegModemConfig1) & 0xF0;
    u2_t bw;

    if (bwConfig == SX1276_MC1_BW_125){
        bw = 125;
    }else if(bwConfig == SX1276_MC1_BW_250){
        bw = 250;
    }else if(bwConfig == SX1276_MC1_BW_500){
        bw = 500;
    }else{
        bw = 0;
    }
    return bw;
}

//get snr of last received packet
s1_t WrapRadioAt::getSnr(){
    return readReg(LORARegPktSnrValue);
}