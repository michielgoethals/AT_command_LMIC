#pragma once

#include "wrapMacAt.h"

#define MAX_GPIO_PINS 18
#define MAX_PIN_MODES 3

#define EEPROM_START_NVM_ADDR           0x08080400

class WrapSysAt{
    //general commands
    public:
        String sleep();
        void reset();
        void eraseFW();
        void factoryRESET(WrapMacAt *macWrapper);

    //set commands
    public:
        String setNvm(char* address, char* data);
        String setPinDig(char* pinName, u1_t pinState);
        String setPinMode(char* pinName, char* pinMode);

    //get commands
    public:
        String getVer();
        String getNvm(char* address);
        String getVdd();
        String getHweui();
        String getPinDig(char* pinName);
        String getPinAna(char* pinName);

    //attributes
    private:
        String version = "0.0.1";
        String response;
        
        String pinModesList[MAX_PIN_MODES] = {"digout", "digin", "ana"};
        int pinModes[MAX_PIN_MODES]= {OUTPUT, INPUT, INPUT};
        
        String pinNameList[MAX_GPIO_PINS] = {"gpio0", "gpio1", "gpio2", "gpio3", "gpio4", "gpio5", "gpio6", "gpio7", "gpio8", "gpio9", 
        "gpio10", "gpio11", "uart_cts", "uart_rts", "test0", "test1"};
        int pinList[MAX_GPIO_PINS] = {PB0,PB1,PB2,PB12,PB13,PB14,PB15,PA8,PA9,PA10,PA11,PA12, /*gpio0-11 respectivly*/
        PA0,PA1,PB10,PB11}; //uart_cts, uart_rts, test0, test1 respectivly

};
