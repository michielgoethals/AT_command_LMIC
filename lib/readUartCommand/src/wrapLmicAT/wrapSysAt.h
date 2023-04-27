#pragma once

#include "wrapMacAt.h"

#define MAX_GPIO_PINS 18

class WrapSysAt{

    public:
        //General
        String sleep();
        String reset(WrapMacAt *macWrapper);
        void eraseFW();
        String factoryRESET();

        //Set commands
        String setNvm(char* address, char* data);
        String setPinDig(char* pinName, u1_t pinState);

        //Get commands
        String getVer();
        String getNvm(char address);
        String getVdd();
        String getHweui();

    private:
        String response;
        String pinNameList[MAX_GPIO_PINS] = {"gpio0", "gpio1", "gpio2", "gpio3", "gpio4", "gpio5", "gpio6", "gpio7", "gpio8", "gpio9", 
        "gpio10", "gpio11", "gpio12", "gpio13", "uart_cts", "uart_rts", "test0", "test1"};
        int pinList[MAX_GPIO_PINS]; //TO DO: link pinNameList to pinList

};
