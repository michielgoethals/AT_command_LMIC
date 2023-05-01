#include "wrapRadioAt.h"

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

        //check wich mode selected
        if((readReg(RegOpMode) & ~OPMODE_MASK) == OPMODE_LORA){
            if(strlen(data) <= 255){
                opmode(OPMODE_TX);
            }else{
                response = "invalid_param";
            }
        }else if((readReg(RegOpMode) & ~OPMODE_MASK) == OPMODE_FSK_SX127X_SETUP){
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
    //default ramp time is 40us
    if(strcmp(gfBT, "none") == 0){
        writeReg(RegPaRamp, 0x09);
    }else if(strcmp(gfBT, "1.0") == 0){
        writeReg(RegPaRamp, 0x1D);
    }else if(strcmp(gfBT, "0.5") == 0){
        writeReg(RegPaRamp, 0x29);
    }else if(strcmp(gfBT, "0.3") == 0){
        writeReg(RegPaRamp, 0x39);
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
    u1_t rPaConfig;
    u1_t rPaDac;
    u1_t rOcp;

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
    response = "ok";

    //RxBw = Fxtal / RxBwMant * 2^(RxBwExp+2)
    //RxBWMantAfc (bit 4-3) and RxBWExpAfc (bit 2-0)

    for(int i = 0; i < MAX_BW_VALUES; i++){
        if(autoFreqBand == bwList[i]){
            //set auto frequency correction bandwidth
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
    response = "ok";

    //RxBw = Fxtal / RxBwMant * 2^(RxBwExp+2)
    //RxBWMantAfc (bit 4-3) and RxBWExpAfc (bit 2-0)
  
    for(int i = 0; i < MAX_BW_VALUES; i++){
        if(rxBandWidth == bwList[i]){
            //set auto frequency correction bandwidth
            u1_t rxBwMant = bwMantList[i];
            u1_t rxBwExp = bwExpList[i];
            u1_t rxBwvalue = (rxBwMant << 3) | rxBwExp;
            writeReg(FSKRegRxBw, rxBwvalue);
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

    if (iqInvert == "on"){
        writeReg(LORARegInvertIQ, readReg(LORARegInvertIQ)|(1<<6));
    }else if (iqInvert == "off"){
        writeReg(LORARegInvertIQ, readReg(LORARegInvertIQ) & ~(1<<6));
    }else{
        response = "invalid_param";
    }

    return  response;
}

//set coding rate (4/5, 4/6, 4/7, 4/8)
String WrapRadioAt::setCr(char* codingRate){
    response = "ok";
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

}

//get current operation mode of the radio lora of fsk
String WrapRadioAt::getMod(){

}

// get current frequency of the radio
u4_t WrapRadioAt::getFreq(){

}

//get current power level settings of the radio
s1_t WrapRadioAt::getPwr(){

}

//get current spreading factor
String WrapRadioAt::getSf(){

}

//
float WrapRadioAt::getAfcBw(){

}

float WrapRadioAt::getRxBw(){

}

u2_t WrapRadioAt::getBitRate(){

}

u2_t WrapRadioAt::getFdev(){

}

u2_t WrapRadioAt::getPrLen(){

}

String WrapRadioAt::getCrc(){

}

String WrapRadioAt::getIqi(){

}

String WrapRadioAt::getCr(){

}

u4_t WrapRadioAt::getWdt(){

}

// get bandwidth of the radio
u2_t WrapRadioAt::getBw(){

}

//get snr of last received packet
s1_t WrapRadioAt::getSnr(){
    return readReg(LORARegPktSnrValue);
}