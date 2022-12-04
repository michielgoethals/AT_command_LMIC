#pragma once

#include "./LMIC/lmic.h"
#include <stdlib.h> 

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
    private:
        bool devEuiSet = false;
        bool appEuiSet = false;
        bool appKeySet = false;
        char tempStr[3] = {0x00, 0x00, 0x00};
};


