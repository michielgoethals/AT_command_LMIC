#include "wrapSysAt.h"

int counter = 0;
u4_t sleepTime = 0;

void WrapSysAt::begin(RTC_HandleTypeDef *rtc){
    hrtc = *rtc;
}

//put system in sleep for a finite number of milliseconds
String WrapSysAt::sleep(u4_t ms){
    response = "ok";

    sleepTime = ms;
    
    counter = ms/32767;

    //Wakeup Time Base = 16 /(~32.768KHz )  = 0.488 ms
    //Wakeup Time = 0.488 ms  * WakeUpCounter
    //WakeUpCounter = Time to sleep in ms / 0.488 ms

    if(counter == 0){
        HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, ms/WAKE_UP_BASE_TIME, RTC_WAKEUPCLOCK_RTCCLK_DIV16);
    }else{
        //set wake up timer to max value
        HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 0xFFFF, RTC_WAKEUPCLOCK_RTCCLK_DIV16);
    }

    //Suspend Tick increment to prevent wakeup by Systick interrupt.
    HAL_SuspendTick();

    HAL_PWR_EnableSleepOnExit();
        
    //Enter Sleep Mode , wake up is done once RTC wake up interrupt is generated
    HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);

    HAL_ResumeTick();

    return response;
}

//restore save LoRaWAN configuration from EEPROM
void WrapSysAt::reset(){
    NVIC_SystemReset();
}

//remove firmware from flash and prepare for new firmware
void WrapSysAt::eraseFW(){
    //set all flash memory to 0xFF
    //set SWD-pins default

    //HAL_FLASHEx_Erase(0x8000000, FLASH_TYPEERASE_PAGES, 128);
}

//reset user and data eeprom to default values
void WrapSysAt::factoryRESET(WrapMacAt *macWrapper){
    HAL_FLASHEx_DATAEEPROM_Unlock();
    
    for (uint32_t Address = DATA_EEPROM_BASE; Address <=DATA_EEPROM_END; Address += 4) {
        HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD, Address, 0);
    }
   
    HAL_FLASHEx_DATAEEPROM_Lock();
    
    macWrapper->reset(868);
    macWrapper->setDevEui((char*)defaultEui.c_str());
    macWrapper->setAppEui((char*)defaultEui.c_str());
    macWrapper->setAppKey((char*)defaultKey.c_str());
    macWrapper->setDevAddr((char*)defaultAddr.c_str());
    macWrapper->setNwksKey((char*)defaultKey.c_str());
    macWrapper->setAppsKey((char*)defaultKey.c_str());
    macWrapper->setUpCtr(0);
    macWrapper->setDnCtr(0);
    LMICbandplan_setSessionInitDefaultChannels();
    //save default configuration to EEPROM
    macWrapper->save();

    //perform a software reset
    reset();
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

//set unsed pin to digital output and drive high (1) or low (0)
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

//get version of the firmware
String WrapSysAt::getVer(){
    return version;
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
    //return String(analogRead(A0)*3.3*1000/4096);
    return "not_implemeted";
}

//reads the preprogrammed EUI node address from the RN2483
//can be used to set the DevEUI
//???
String WrapSysAt::getHweui(){
    //u1_t hweui;
    //hal_spi_read(0x80, &hweui, 8);
    return "not_implemented";

}