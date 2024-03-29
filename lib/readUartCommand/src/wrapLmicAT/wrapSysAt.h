#pragma once

#include "wrapMacAt.h"

#define MAX_GPIO_PINS 19
#define MAX_PIN_MODES 3

#define EEPROM_START_NVM_ADDR           0x08080400

#define WAKE_UP_BASE_TIME 0.488 //0.488ms = 16/32,768kHz

extern int counter;
extern u4_t sleepTime;

class WrapSysAt{
    //general commands
    public:
        void begin(RTC_HandleTypeDef *rtc);
        String sleep(u4_t ms);
        void reset();
        void eraseFW();
        void factoryRESET(WrapMacAt *macWrapper);

    //set commands
    public:
        String setNvm(char* address, char* data);
        String setPinDig(char* pinName, u1_t pinState);
        //maybe for next version
        //String setPinMode(char* pinName, char* pinMode);

    //get commands
    public:
        String getVer();
        String getNvm(char* address);
        String getVdd();
        String getHweui();
        //maybe for next version
        //String getPinDig(char* pinName);
        //String getPinAna(char* pinName);

    //attributes
    private:
        String version = "0.0.1";
        String response;

        RTC_HandleTypeDef hrtc;
        
        //String pinModesList[MAX_PIN_MODES] = {"digout", "digin", "ana"};
        //int pinModes[MAX_PIN_MODES]= {OUTPUT, INPUT, INPUT};
        
        //String pinNameList[MAX_GPIO_PINS] = {"gpio0", "gpio1", "gpio2", "gpio3", "gpio4", "gpio5", "gpio6", "gpio7", "gpio8", "gpio9", 
        //"gpio10", "gpio11", "gpio12", "gpio13", "gpio14", "uart_cts", "uart_rts", "test0", "test1"};
        //int pinList[MAX_GPIO_PINS] = {PB0,PB1,PB2,PB10,PB11,PB12,PB13,PB14,PB15,PA7,PA8,PA9,PA10,PA11,PA12, /*gpio0-14 respectivly*/
        //PA0,PA1,PA14,PA13}; //uart_cts, uart_rts, test0 (SWDCLK), test1 (SWDIO) respectivly

        String pinNameList[MAX_GPIO_PINS] = {"gpio6", "gpio7", "gpio8", "gpio9", "gpio10", "gpio11", "uart_cts", "uart_rts", "test0", "test1"};
        int pinList[10] = {PA8, PA9, PA10, PA11, PB10, PB11, PA0, PA1, PA14, PA13}; //gpio6-11, uart_cts, uart_rts, test0 (SWDCLK), test1 (SWDIO) respectivly
};
