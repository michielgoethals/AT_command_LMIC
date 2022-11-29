#pragma once

#include "./LMIC/lmic.h"

typedef const char * LoraParam;

//typedef const char * responses_t;

//enum resonses_t{ok, invalid_param, not_joined, no_free_ch, silent, frame_counter_err_rejoin, busy, mac_paused, invalid_data_len};
//enum at_res_trans_t{}

class Wrapper{
    void setDevEui(LoraParam deveui);
    void setAppEui(LoraParam appeui);
    void setNwskey(LoraParam nwskey);
    void setAppskey(LoraParam appskey);
    void macReset(int band);
    void joinOtaa(void);
    void joinABP(void);
};


