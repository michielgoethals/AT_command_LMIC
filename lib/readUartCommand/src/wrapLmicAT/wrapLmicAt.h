#pragma once

#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>

#include <stdlib.h> 
#include <cstring>

// LoRaWAN LMIC constants
#define LMIC_NSS_PIN 				   10
#define LMIC_RST_PIN 				   5
#define LMIC_DIO0_PIN 				   2
#define LMIC_DIO1_PIN 				   3
#define LMIC_DIO2_PIN 				   4

#define LORA_EUI_SIZE 8
#define LORA_KEY_SIZE 16

#define EEPROM_ADDRESS 0x08080000
#define NET_ID 0x000013

typedef const char * LoraParam;

class WrapLmicAT{
    public:
        //set default parameters
        void begin();
    public:
        //mac commands
        void reset(u2_t band);
        void tx(char* cnf, u1_t portno, char* data);
        void joinOtaa();
        void joinABP();
        void save();
        void forceEnable();
        void pause();
        void resume();
    
    public:    
        //mac set command
        void setDevAddr(LoraParam devaddr); 
        void setDevEui(LoraParam deveui);
        void setAppEui(LoraParam appeui);
        void setNwkskey(LoraParam nwskey);
        void setAppsKey(LoraParam appskey);
        void setAppKey(LoraParam appkey);
        void setPwridx(u1_t pwrIndex); //0-5
        void setDr(u1_t dataRate); //0-8
        void setAdr(char* state); 
        void setBat(u1_t level);
        void setRetX(u1_t retX);
        void setLinkChk(u2_t sec);
        void setRxDelay1(u2_t rxDelay);
        void setAr(char* state);
        void setRx2(u1_t dr, u4_t freq);
        void setChFreq(u1_t chID, u4_t frequency);
        void setChDutyCycle(u1_t chID, u2_t dutyCycle);
        void setChDrRange(u1_t chID, int minRange, int maxRange);
        void setChStatus(u1_t chIDn, char* enable);

        //mac get commands
        String getDevAddr();
        String getDevEui();
        String getAppEui();
        u1_t getDr();
        u2_t getBand();
        u1_t getPwridx();
        String getAdr();
        u1_t getRetX();
        u2_t getRxDelay1(); 
        int getRxDelay2(); 
        void getAr();
        String getRx2(u1_t band);
        u2_t getDcycleps();
        u1_t getMrgn();
        u1_t getGwnb();
        void getSatus();
        u4_t getChFreq(u1_t chID);
        u2_t getChDcycle(u1_t chID);
        void getChdrrange(u1_t chID);
        void getChStatus(u1_t chID);
       
    private:
        //OTAA 
        
        String devEui;
        String appEui;
        String appKey;

        bool devEuiSet = false;
        bool appEuiSet = false;
        bool appKeySet = false;

        //ABP
        String nwksKey;
        String devAddr;
        String appsKey;

        bool nwksKeySet = false;
        bool devAddrSet = false;
        bool appsKeySet = false;
        
        //default values;
        u1_t dr = 5;
        u2_t band = 868;
        u1_t pwrIndex = 1;
        u1_t retX = 7;
        u4_t freq_rx2 = 869525000;
        int dcylceps = 1;
        u1_t mrgn = 255;
        u1_t gwnb = 0;
        
        char tempStr[3] = {0x00, 0x00, 0x00};
};


