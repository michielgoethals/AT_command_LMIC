#pragma once

/* #include "./LMIC/lmic.h"
#include "LMIC/hal/hal.h"
#include <SPI.h>
*/

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

//typedef const char * responses_t;

//char* resonses_t = ["ok", "invalid_param", "not_joined", "no_free_ch", "silent", "frame_counter_err_rejoin", "busy", "mac_paused", "invalid_data_len];

class WrapLmicAT{
    public:
        //mac commands
        void reset(int band);
        void tx(bool cnf, int portno, char* data);
        void joinOtaa();
        void JoinABP();
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
        void setAppsKey(LoraParam appkey);
        void setAppkey(LoraParam appskey);
        void setPwridx(int pwrIndex); //0-5
        void setDr(int dataRate); //0-8
        void setAdr(bool on); 
        void setBat(__uint8_t level);
        void setRetX(int retX);
        void setLinkChk(__uint16_t sec);
        void setRxDelay1(__uint16_t rxDelay);
        void setAr(bool on);
        void setRx2(int dataRate, int frequency);
        void setChFreq(int chID, int frequency);
        void setChDutyCycle(int chID, int dutyCycle);
        void setChDrRange(int chID, int minRange, int maxRange);
        void setChStatus(int chIDn, bool on);

        //mac get commands
        char* getDevAddr();
        char* getDevEui();
        char* getAppEui();
        int getDr();
        int getBand();
        int getPwridx();
        bool getAdr();
        int getRetX();
        int getRxDelay1(int rxdelay); 
        int getRxDelay2(int rxdelay); 
        bool getAr();
        int getRx2(int band);
        int getDcycleps();
        int getMrgn();
        int getGwnb();
        char* getSatus();
        int getChFreq(int chID);
        int getChDcycle(int chID);
        int getChdrrange(int chID);
        bool getChStatus(int chID);
       
       
    

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

        //static osjob_t sendjob;
        const unsigned TX_INTERVAL = 60;
};


