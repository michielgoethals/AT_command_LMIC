#pragma once

#include "wrapMacAt.h"

#define MAX_GPIO_PINS 18
#define MAX_PIN_MODES 3

#define EEPROM_START_NVM_ADDR           0x08080400

class WrapSysAt{
    //general commands
    public:
        String sleep();
        String reset(WrapMacAt *macWrapper);
        void eraseFW();
        String factoryRESET();

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
        String response;
        String pinModes[MAX_PIN_MODES] = {"digout", "digin", "ana"};
        String pinNameList[MAX_GPIO_PINS] = {"gpio0", "gpio1", "gpio2", "gpio3", "gpio4", "gpio5", "gpio6", "gpio7", "gpio8", "gpio9", 
        "gpio10", "gpio11", "gpio12", "gpio13", "uart_cts", "uart_rts", "test0", "test1"};
        int pinList[MAX_GPIO_PINS]; //TO DO: link pinNameList to pinList

};
