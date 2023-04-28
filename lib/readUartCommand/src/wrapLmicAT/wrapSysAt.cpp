#include "wrapSysAt.h"

//put system in sleep for a finite number of milliseconds
String WrapSysAt::sleep(){
    
    //return response after sleep is complete
    response = "not_implemented";

    return response;

}

//restore save LoRaWAN configuration from EEPROM
String WrapSysAt::reset(WrapMacAt *macWrapper){
    //restore band
    macWrapper->reset(*(u2_t*)EEPROM_START_ADDR_BAND);

    //restore deveui
    char deveui[LORA_EUI_SIZE*2];
    for(u1_t i = 0; i < LORA_EUI_SIZE; i++){
        sprintf(deveui+i*2, "%02X", *((char*)EEPROM_START_ADDR_DEVEUI+i));
    }
    macWrapper->setDevEui(deveui);

    //restore appeui
    char appeui[LORA_EUI_SIZE*2];
    for (u1_t i = 0; i < LORA_EUI_SIZE; i++){
        sprintf(appeui+i*2, "%02X", *((char*)EEPROM_START_ADDR_APPEUI+i));
    }
    macWrapper->setAppEui(appeui);


    //restore appkey
    char appkey[LORA_KEY_SIZE*2];
    for(u1_t i = 0; i < LORA_KEY_SIZE; i++){
        sprintf(appkey+i*2, "%02X", *((char*)(EEPROM_START_ADDR_APPKEY+i)));    
    }
    macWrapper->setAppKey(appkey);

    //restore nwkskey
    char nwkskey[LORA_KEY_SIZE*2];
    for(u1_t i = 0; i < LORA_KEY_SIZE; i++){
        sprintf(nwkskey+i*2, "%02X", *((char*)(EEPROM_START_ADDR_NWKSKEY+i)));        
    }
    macWrapper->setNwkskey(nwkskey);

    //restore appskey
    char appskey[LORA_KEY_SIZE*2];
    for(u1_t i = 0; i < LORA_KEY_SIZE; i++){
        sprintf(appskey+i*2, "%02X", *((char*)(EEPROM_START_ADDR_APPSKEY+i)));       
    }
    macWrapper->setAppsKey(appskey);

    //restore devaddr
    char devaddr[LORA_ADDR_SIZE*2];
    for(u1_t i = 0; i < LORA_ADDR_SIZE; i++){
        sprintf(devaddr+i*2, "%02X", *((char*)EEPROM_START_ADDR_DEVADDR-i-1+LORA_ADDR_SIZE));
    }
    macWrapper->setDevAddr(devaddr);

    //TO DO SET CHANNELS

    return  "STM reset and restarted version:"+getVer();
}

//remove firmware from flash and prepare for new firmware
void WrapSysAt::eraseFW(){
    //HAL_FLASHEx_Erase(0x8000000, FLASH_TYPEERASE_PAGES, 128);
}

//reset user and data eeprom to default values
String WrapSysAt::factoryRESET(){
    HAL_FLASHEx_DATAEEPROM_Erase(DATA_EEPROM_BASE);
    return "STM32 EEPROM restored to default values";
}


//set data (00-FF) of a user eeprom adress(300-3FF)
String WrapSysAt::setNvm(char* address, char* data){
    u2_t _address = strtol(address, NULL, 16);
    u2_t _data = strtol(data, NULL, 16);
    
    if(_address < 0x300 || _address > 0x3FF){
        response = "invalid_param";
    }else if(_data < 0x00 || _data > 0xFF){
        response = "invalid_param";
    }else{
        HAL_FLASHEx_DATAEEPROM_Unlock();
        HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE, EEPROM_START_NVM_ADDR+_address, _data);
        HAL_FLASHEx_DATAEEPROM_Lock();

        response = "ok";
    }
    return response;
}

//set unused pin to digital output and drive high (1) or low (0)
String WrapSysAt::setPinDig(char* pinName, u1_t pinState){
    if(pinState != 0 && pinState != 1){
        response = "invalid_param";
    }else{
        for (u1_t i = 0; i < MAX_GPIO_PINS; i++){
            if(String(pinName) == pinNameList[i]){
                pinMode(pinList[i], OUTPUT);
                digitalWrite(pinList[i], pinState);
                response = "ok";
            }else{
                response = "invalid_param";
            }
        }
    } 
    return response; 
}

/*
String WrapSysAt::setPinMode(char* pinName, char* pMode){
    for(u1_t i = 0; i < MAX_PIN_MODES; i++){
        if(String(pMode) == pinModesList[i]){
            for(u1_t j = 0; j < MAX_GPIO_PINS; j++){
                if(String(pinName) == pinNameList[j]){
                    pinMode(pinList[j], pinModes[i]);
                    response = "ok";
                }else{
                    response = "invalid_param";
                }
            }
        }else{
            response = "invalid_param";
        }
    }
    return response; 
}
*/

//get version of the firmware
String WrapSysAt::getVer(){
    return "0.0.1";
}

//get the hex value of the user eeprom address (300-3FF)
String WrapSysAt::getNvm(char* address){
    u2_t _address = strtol(address, NULL, 16);

    if(_address < 0x300 || _address > 0x3FF){
        response = "invalid_param";
    }else{
        char data[1];
        sprintf(data, "%02X", *((char*)EEPROM_START_NVM_ADDR+_address));
        response = data;
    }
    return response;
}

//do an ADC conversion and return the result in mV (0-3600)
String WrapSysAt::getVdd(){
    return String(analogRead(A0)*3.3*1000/4096);
}

//get hweui of radio module
//TO DO
String WrapSysAt::getHweui(){
    //u1_t hweui;
    //hal_spi_read(0x80, &hweui, 8);
    return "not_implemented";

}

//maybe implement in other hardware design not enough analog pins om 48pin stm32l0
/*
String WrapSysAt::getPinDig(char* pinName){
    for (u1_t i = 0; i < MAX_GPIO_PINS; i++){
        if(String(pinName) == pinNameList[i]){
            int pin = pinList[i];
            response = (String)digitalRead(pin);
        }else{
            response = "invalid_param";
        }
    }
    return response;   
}


String WrapSysAt::getPinAna(char* pinName){
    for (u1_t i = 0; i < MAX_GPIO_PINS; i++){
        if(String(pinName) == pinNameList[i]){
            int pin = pinList[i];
            response = (String)analogRead(pin);
        }else{
            response = "invalid_param";
        }
    }
    return response;   
}
*/