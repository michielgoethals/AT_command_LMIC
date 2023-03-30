#include "wrapSysAt.h"

void WrapSysAt::sysReset(WrapMacAt *macWrapper){
    macWrapper->macReset(*(u2_t*)EEPROM_START_ADDR_BAND);
    macWrapper->setDevEui((char*)EEPROM_START_ADDR_DEVEUI);
    macWrapper->setAppEui((char*)EEPROM_START_ADDR_APPEUI);
    macWrapper->setAppKey((char*)EEPROM_START_ADDR_APPKEY);
    macWrapper->setNwkskey((char*)EEPROM_START_ADDR_NWKSKEY);
    macWrapper->setAppsKey((char*)EEPROM_START_ADDR_APPSKEY);
    macWrapper->setNwkskey((char*)EEPROM_START_ADDR_NWKSKEY);
    macWrapper->setDevAddr((char*)EEPROM_START_ADDR_DEVADDR);
}