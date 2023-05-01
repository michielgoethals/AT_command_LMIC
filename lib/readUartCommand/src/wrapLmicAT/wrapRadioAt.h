#pragma once

#include <lmic/oslmic_types.h>
#include <lmic/radio.c>

#include "wrapMacAt.h"

#define MAX_PWR_VALUES      19
#define MAX_BW_VALUES       21

class WrapRadioAt{
    //general commands
    public:
        String rx(u2_t windowSize);
        String tx(char* data);
        String cw(char* state);
    
    //set commands
    public:  
        String setBt(char* gfBT);
        String setMod(char* mode);
        String setFreq(u4_t freq);
        String setPwr(s1_t pwrout);
        String setSf(char* spreadingFactor);
        String setAfcBw(float autoFreqBand);
        String setRxBw(float rxBandWidth);
        String setBitRate(u2_t fskBitRate);
        String setFdev(u2_t freqDev);
        String setPrLen(u2_t preamble);
        String setCrc(char* crcHeader);
        String setIqi(char* iqInvert);
        String setCr(char* codingRate);
        String setWdt(u4_t watchDog);
        String setSync(char* syncWord);
        String setBw(u2_t bandWidth);
    
    //get commands
    public:
        void getBt();
        void getMod();
        void getFreq();
        void getPwr();
        void getSf();
        void getAfcBw();
        void getRxBw();
        void getBitRate();
        void getFdev();
        void getPrLen();
        void getCrc();
        void getIqi();
        void getCr();
        void getWdt();
        void getBw();
        void getSnr();

    //attributes
    private:
        u1_t rPaConfig;
        u1_t mc1 = 0, mc2 = 0, mc3 = 0;
        s1_t pwrList[MAX_PWR_VALUES] = {-3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
        float bwList[MAX_BW_VALUES] = {250, 125, 62.5, 31.3, 15.6, 7.8, 3.9, 200, 100, 50, 25, 12.5, 6.3, 3.1, 166.7, 83.3, 41.7, 20.8, 10.4, 5.2, 2.6};
        u1_t bwMantList[MAX_BW_VALUES] = {16, 20, 24, 16, 20, 24, 16, 20, 24, 16, 20 ,24, 16, 20, 24, 16, 20, 24, 16, 20, 24}; //from datasheet sx1276
        u1_t bwExpList[MAX_BW_VALUES] = {1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4 ,4, 5, 5, 5, 6, 6, 6, 7, 7, 7}; //from datasheet sx1276
        String response;
};