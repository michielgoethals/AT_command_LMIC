#pragma once

#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>

#include <stdlib.h> 
#include <cstring>

// LoRaWAN LMIC constants
#define LMIC_NSS_PIN 				    PA4  //pin14 QFN48  //20 QFP64
#define LMIC_RST_PIN 				    PB14 //pin27 QFN48  //35 QFP64
#define LMIC_DIO0_PIN 				    PA10 //pin31 QFN48  //43 QFP64
#define LMIC_DIO1_PIN 				    PA11 //pin32 QFN48  //44 QFP64
#define LMIC_DIO2_PIN 				    PA12 //pin33 QFN48  //45 QFP64

#define LORA_ADDR_SIZE                  4
#define LORA_EUI_SIZE                   8
#define LORA_KEY_SIZE                   16

#define EEPROM_START_ADDR_BAND          0x08080000
#define EEPROM_START_ADDR_DEVEUI        0x08080004
#define EEPROM_START_ADDR_APPEUI        0x0808000C
#define EEPROM_START_ADDR_APPKEY        0x08080014
#define EEPROM_START_ADDR_NWKSKEY       0x08080024
#define EEPROM_START_ADDR_APPSKEY       0x08080034
#define EEPROM_START_ADDR_DEVADDR       0x08080044
#define EEPROM_START_ADDR_FCNTUP        0x08080048
#define EEPROM_START_ADDR_FCNTDOWN      0x0808004C

/* #define EEPROM_START_ADDR_CH_FREQ    0x08080048
#define EEPROM_START_ADDR_CH_DCYCLE     0x0808004C
#define EEPROM_START_ADDR_CH_DRRANGE    0x0808004D
#define EEPROM_START_ADDR_CH_STATUS     0x0808004F
#define EEPROM_START_ADDR_FCNTUP        0x08080052
#define EEPROM_START_ADDR_FCNTDOWN      0x08080056 */

#define NET_ID 0x13

class WrapMacAt{
    public:
        //set default parameters
        void begin();
    public:
        //mac commands
        String reset(u2_t band);
        String tx(char* cnf, u1_t portno, char* data);
        String joinOtaa();
        String joinABP();
        String save();
        String forceEnable();
        String pause();
        String resume();
    
    public:    
        //mac set command
        String setDevAddr(char* devaddr); 
        String setDevEui(char* deveui);
        String setAppEui(char* appeui);
        String setNwkskey(char* nwskey);
        String setAppsKey(char* appskey);
        String setAppKey(char* appkey);
        String setPwridx(u1_t pwrIndex); //0-5
        String setDr(u1_t dataRate); //0-8
        String setAdr(char* state); 
        void setBat(u1_t level);
        void setRetX(u1_t retX);
        void setLinkChk(u2_t sec);
        void setRxDelay1(u2_t rxDelay);
        void setAr(char* state);
        void setRx2(u1_t dr, u4_t freq);
        void setChFreq(u1_t chID, u4_t frequency);
        void setChDCycle(u1_t chID, u2_t Cycle);
        void setChDrRange(u1_t chID, u1_t minRange, u1_t maxRange);
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
        u2_t getRxDelay2(); 
        String getAr();
        String getRx2(u1_t band);
        u2_t getDcycleps();
        u1_t getMrgn();
        u1_t getGwnb();
        u2_t getSatus();
        u4_t getChFreq(u1_t chID);
        u2_t getChDcycle(u1_t chID);
        String getChDrrange(u1_t chID);
        String getChStatus(u1_t chID);

        //sys commands
        void sysReset();
        
    private:
        //OTAA 
        
        String devEui = "0000000000000000";
        String appEui = "0000000000000000";

        bool devEuiSet = false;
        bool appEuiSet = false;
        bool appKeySet = false;

        //ABP
        String devAddr = "00000000";
        
        bool nwksKeySet = false;
        bool devAddrSet = false;
        bool appsKeySet = false;
        
        //default values;
        u1_t dr = 5;
        u2_t band = 868;
        u1_t pwrIndex = 1;
        u1_t retX = 7;
        u4_t freq_rx2 = 869525000;
        u2_t dcylceps = 1; 
        u1_t mrgn = 255;
        u1_t gwnb = 0;
        bit_t adr = 0;
        bit_t ar = 0;
        bit_t silent = 0;
        bit_t paused = 0;
        bit_t linkchk = 0;
        
        //status register
        u2_t status = 0b0000000000000000;

        //settings updated bits
        bit_t chUpdated = 0;
        bit_t pwrUpdated = 0;
        bit_t nbRepUpdated = 0;
        bit_t prescalerUpdated = 0;
        bit_t RX2Updated = 0;
        bit_t TXUpdated = 0;
        
        char tempStr[3] = {0x00, 0x00, 0x00};

        String response;
};


