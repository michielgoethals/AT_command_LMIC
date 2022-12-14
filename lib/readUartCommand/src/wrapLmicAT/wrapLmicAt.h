#pragma once

/* #include "./LMIC/lmic.h"
#include "LMIC/hal/hal.h"
#include <SPI.h>  */

#include <stdlib.h> 

// LoRaWAN LMIC constants
#define LMIC_NSS_PIN 				   10
#define LMIC_RST_PIN 				   LMIC_UNUSED_PIN
#define LMIC_DIO0_PIN 				   LMIC_UNUSED_PIN
#define LMIC_DIO1_PIN 				   LMIC_UNUSED_PIN
#define LMIC_DIO2_PIN 				   LMIC_UNUSED_PIN

#define LORA_EUI_SIZE 8
#define LORA_KEY_SIZE 16

typedef const char * LoraParam;

class WrapLmicAT{
    public:
        //mac commands
        void reset(int band);
        void tx(char* cnf, int portno, char* data);
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
        void setPwridx(int pwrIndex); //0-5
        void setDr(int dataRate); //0-8
        void setAdr(char* state); 
        void setBat(int level);
        void setRetX(int retX);
        void setLinkChk(int sec);
        void setRxDelay1(int rxDelay);
        void setAr(char* state);
        void setRx2(int dataRate, int frequency);
        void setChFreq(int chID, int frequency);
        void setChDutyCycle(int chID, int dutyCycle);
        void setChDrRange(int chID, int minRange, int maxRange);
        void setChStatus(int chIDn, bool state);

        //mac get commands
        char* getDevAddr();
        char* getDevEui();
        char* getAppEui();
        int getDr();
        int getBand();
        int getPwridx();
        char* getAdr();
        int getRetX();
        int getRxDelay1(); 
        int getRxDelay2(); 
        char* getAr();
        int getRx2(int band);
        int getDcycleps();
        int getMrgn();
        int getGwnb();
        char* getSatus();
        void getChFreq(int chID);
        void getChDcycle(int chID);
        void getChdrrange(int chID);
        void getChStatus(int chID);
       
        //void printHex2(unsigned v);
        //void onEvent(ev_t ev);
        //void do_send(osjob_t* j);
    
    private:
        //OTAA

        bool devEuiSet = false;
        bool appEuiSet = false;
        bool appKeySet = false;

        //ABP
        bool nwksKeySet = false;
        bool devAddrSet = false;
        bool appsKeySet = false;

        int dr = 5;
        int band = 868;
        int pwrIndex = 1;
        //char* adr_state = "off";
        int retX = 7;
        int rxdelay1 = 1000;
        int rxdelay2 = 2000;
        //char* ar_state = "off";
        int freq_band = 3869525000;
        int dcylceps = 1;
        int mrgn = 255;
        int gwnb = 0;
        int status = 0x0000;

        int linkchk = 0;

        //static osjob_t sendjob;
        const unsigned TX_INTERVAL = 60;
};


