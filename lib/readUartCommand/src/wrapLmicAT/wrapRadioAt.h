#pragma once

#include <lmic/oslmic_types.h>

class WrapRadioAt{
    public:
        void rx(u2_t windowSize);
        void tx(char* data);
        void cw(char* state);

        void setBt(char* gfBT);
        void setMod(char* mode);
        void setFreq(u4_t freq);
        void setPwr(s1_t pwrout);
        void setSf(char* spreadingFactor);
        void setAfcBw(float autoFreqBand);
        void setRxBw(float rxBandWidth);
        void setBitRate(u2_t fskBitRate);
        void setFdev(u2_t freqDev);
        void setPrLen(u2_t preamble);
        void setCrc(char* crcHeader);
        void setIqi(char* iqInvert);
        void setCr(char* codingRate);
        void setWdt(u4_t watchDog);
        void setSync(char* syncWord);
        void setBw(u2_t bandWidth);

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
};