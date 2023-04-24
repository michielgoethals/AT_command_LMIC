#pragma once

#include "wrapMacAt.h"

class WrapSysAt{
private:

public:
    //General
    String sleep();
    String reset(WrapMacAt *macWrapper);
    void eraseFW();
    String factoryRESET();

    //Set commands
    String setNvm(char address, char data);
    String setPinDig(char pinName, u1_t pinState);

    //Get commands
    String getVer();
    String getNvm(char address);
    String getVdd();
    String getHweui();

private:
    String response;
};
