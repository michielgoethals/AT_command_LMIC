#pragma once

#include "./LMIC/lmic.h"
#include "LMIC/hal/hal.h"
#include <SPI.h>

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
        //OTAA
        void setDevEui(LoraParam deveui);
        void setAppEui(LoraParam appeui);
        void setAppKey(LoraParam appkey);
        void joinOtaa(void);
        //ABP
        void setDevAddr(LoraParam devaddr); 
        void setNwskey(LoraParam nwskey);
        void setAppskey(LoraParam appskey);
        void joinABP(void);
        
        void macReset(int band);

        void printHex2(unsigned v);
        void onEvent(ev_t ev);
        void do_send(osjob_t* j);
    
    private:
        //OTAA

        bool devEuiSet = false;
        bool appEuiSet = false;
        bool appKeySet = false;

        //ABP
        bool nwksKeySet = false;
        bool devAddrSet = false;
        bool appsKeySet = false;

        static osjob_t sendjob;
        const unsigned TX_INTERVAL = 60;
};


