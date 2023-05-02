#pragma once

#include "wrapMacAt.h"

#define RegFifo                                    0x00 // common
#define RegOpMode                                  0x01 // common       see below
#define FSKRegBitrateMsb                           0x02 //              -
#define FSKRegBitrateLsb                           0x03 //              -
#define FSKRegFdevMsb                              0x04 //              -
#define FSKRegFdevLsb                              0x05 //              -
#define RegFrfMsb                                  0x06 // common       FSK: 1272: 915; 1276: 434 MHz
#define RegFrfMid                                  0x07 // common       ditto
#define RegFrfLsb                                  0x08 // common       ditto
#define RegPaConfig                                0x09 // common       see below, many diffs
#define RegPaRamp                                  0x0A // common       see below: bits 6..4 are diff
#define RegOcp                                     0x0B // common       -
#define RegLna                                     0x0C // common       bits 4..0 are diff.
#define FSKRegRxConfig                             0x0D //              -
#define LORARegFifoAddrPtr                         0x0D
#define FSKRegRssiConfig                           0x0E //              -
#define LORARegFifoTxBaseAddr                      0x0E
#define FSKRegRssiCollision                        0x0F //              -
#define LORARegFifoRxBaseAddr                      0x0F
#define FSKRegRssiThresh                           0x10 //              -
#define LORARegFifoRxCurrentAddr                   0x10
#define FSKRegRssiValue                            0x11 //              -
#define LORARegIrqFlagsMask                        0x11
#define FSKRegRxBw                                 0x12 //              -
#define LORARegIrqFlags                            0x12
#define FSKRegAfcBw                                0x13 //              -
#define LORARegRxNbBytes                           0x13
#define FSKRegOokPeak                              0x14 //              -
#define LORARegRxHeaderCntValueMsb                 0x14
#define FSKRegOokFix                               0x15 //              -
#define LORARegRxHeaderCntValueLsb                 0x15
#define FSKRegOokAvg                               0x16 //              -
#define LORARegRxPacketCntValueMsb                 0x16
#define LORARegRxpacketCntValueLsb                 0x17
#define LORARegModemStat                           0x18
#define LORARegPktSnrValue                         0x19
#define FSKRegAfcFei                               0x1A //              -
#define LORARegPktRssiValue                        0x1A
#define FSKRegAfcMsb                               0x1B //              -
#define LORARegRssiValue                           0x1B
#define FSKRegAfcLsb                               0x1C //              -
#define LORARegHopChannel                          0x1C
#define FSKRegFeiMsb                               0x1D //              -
#define LORARegModemConfig1                        0x1D
#define FSKRegFeiLsb                               0x1E //              -
#define LORARegModemConfig2                        0x1E
#define FSKRegPreambleDetect                       0x1F //              -
#define LORARegSymbTimeoutLsb                      0x1F
#define FSKRegRxTimeout1                           0x20 //              -
#define LORARegPreambleMsb                         0x20
#define FSKRegRxTimeout2                           0x21 //              -
#define LORARegPreambleLsb                         0x21
#define FSKRegRxTimeout3                           0x22 //              -
#define LORARegPayloadLength                       0x22
#define FSKRegRxDelay                              0x23 //              -
#define LORARegPayloadMaxLength                    0x23
#define FSKRegOsc                                  0x24 //              -
#define LORARegHopPeriod                           0x24
#define FSKRegPreambleMsb                          0x25 //              -
#define LORARegFifoRxByteAddr                      0x25
#define FSKRegPreambleLsb                          0x26 //              -
#define LORARegModemConfig3                        0x26
#define FSKRegSyncConfig                           0x27 //              -
#define LORARegFeiMsb                              0x28
#define FSKRegSyncValue1                           0x28 //              -
#define LORAFeiMib                                 0x29
#define FSKRegSyncValue2                           0x29 //              -
#define LORARegFeiLsb                              0x2A
#define FSKRegSyncValue3                           0x2A //              -
#define FSKRegSyncValue4                           0x2B //              -
#define LORARegRssiWideband                        0x2C
#define FSKRegSyncValue5                           0x2C //              -
#define FSKRegSyncValue6                           0x2D //              -
#define FSKRegSyncValue7                           0x2E //              -
#define FSKRegSyncValue8                           0x2F //              -
#define LORARegIffReq1                             0x2F
#define FSKRegPacketConfig1                        0x30 //              -
#define LORARegIffReq2                             0x30
#define FSKRegPacketConfig2                        0x31 //              -
#define LORARegDetectOptimize                      0x31
#define FSKRegPayloadLength                        0x32 //              -
#define FSKRegNodeAdrs                             0x33 //              -
#define LORARegInvertIQ                            0x33
#define FSKRegBroadcastAdrs                        0x34 //              -
#define FSKRegFifoThresh                           0x35 //              -
#define FSKRegSeqConfig1                           0x36 //              -
#define LORARegHighBwOptimize1                     0x36
#define FSKRegSeqConfig2                           0x37 //              -
#define LORARegDetectionThreshold                  0x37
#define FSKRegTimerResol                           0x38 //              -
#define FSKRegTimer1Coef                           0x39 //              -
#define LORARegSyncWord                            0x39
#define FSKRegTimer2Coef                           0x3A //              -
#define LORARegHighBwOptimize2                     0x3A
#define FSKRegImageCal                             0x3B //              -
#define FSKRegTemp                                 0x3C //              -
#define FSKRegLowBat                               0x3D //              -
#define FSKRegIrqFlags1                            0x3E //              -
#define FSKRegIrqFlags2                            0x3F //              -
#define RegDioMapping1                             0x40 // common
#define RegDioMapping2                             0x41 // common
#define RegVersion                                 0x42 // common
#define RegTcxo                                    0x4B // common       different addresses, same bits
#define RegPaDac                                   0x4D // common       differnet addresses, same bits

// spread factors and mode for RegModemConfig2
#define SX1272_MC2_FSK  0x00
#define SX1272_MC2_SF7  0x70
#define SX1272_MC2_SF8  0x80
#define SX1272_MC2_SF9  0x90
#define SX1272_MC2_SF10 0xA0
#define SX1272_MC2_SF11 0xB0
#define SX1272_MC2_SF12 0xC0

// sx1276 RegModemConfig1
#define SX1276_MC1_BW_125           0x70
#define SX1276_MC1_BW_250           0x80
#define SX1276_MC1_BW_500           0x90
#define SX1276_MC1_CR_4_5           0x02
#define SX1276_MC1_CR_4_6           0x04
#define SX1276_MC1_CR_4_7           0x06
#define SX1276_MC1_CR_4_8           0x08

// transmit power configuration for RegPaConfig
#define SX1276_PAC_PA_SELECT_PA_BOOST 0x80
#define SX1276_PAC_PA_SELECT_RFIO_PIN 0x00
#define SX1276_PAC_MAX_POWER_MASK     0x70

// the bits to change for max power.
#define SX127X_PADAC_POWER_MASK       0x07
#define SX127X_PADAC_POWER_NORMAL     0x04
#define SX127X_PADAC_POWER_20dBm      0x07

// convert milliamperes to equivalent value for
// RegOcp; delivers conservative value.
#define SX127X_OCP_MAtoBITS(mA)                 \
    ((mA) < 45   ? 0 :                          \
     (mA) <= 120 ? ((mA) - 45) / 5 :            \
     (mA) < 130  ? 0xF :                        \
     (mA) < 240  ? ((mA) - 130) / 10 + 0x10 :   \
                   27)

// bit in RegOcp that enables overcurrent protect.
#define SX127X_OCP_ENA                     0x20

// sx1276 RegModemConfig2
#define SX1276_MC2_RX_PAYLOAD_CRCON        0x04

// Constants for radio registers
#define OPMODE_LORA      0x80
#define OPMODE_MASK      0x07
#define OPMODE_SLEEP     0x00
#define OPMODE_STANDBY   0x01
#define OPMODE_FSTX      0x02
#define OPMODE_TX        0x03
#define OPMODE_FSRX      0x04
#define OPMODE_RX        0x05
#define OPMODE_RX_SINGLE 0x06
#define OPMODE_CAD       0x07

// FSK opmode bits
// bits 6:5 are the same for 1272 and 1276
#define OPMODE_FSK_SX127x_ModulationType_FSK            (0u << 5)
#define OPMODE_FSK_SX127X_SETUP     (OPMODE_FSK_SX127x_ModulationType_FSK)

#define MAX_PWR_VALUES      19
#define MAX_BW_VALUES       21

class WrapRadioAt{
    //general commands
    public:
        String rx(u2_t windowSize);
        String tx(char* data);
        String cw(char* state);
    
    //set commands
    public:  
        String setBt(char* gfBT);
        String setMod(char* mode);
        String setFreq(u4_t freq);
        String setPwr(s1_t pwrout);
        String setSf(char* spreadingFactor);
        String setAfcBw(float autoFreqBand);
        String setRxBw(float rxBandWidth);
        String setBitRate(u2_t fskBitRate);
        String setFdev(u2_t freqDev);
        String setPrLen(u2_t preamble);
        String setCrc(char* crcHeader);
        String setIqi(char* iqInvert);
        String setCr(char* codingRate);
        String setWdt(u4_t watchDog);
        String setSync(char* syncWord);
        String setBw(u2_t bandWidth);
    
    //get commands
    public:
        String getBt();
        String getMod();
        u4_t getFreq();
        s1_t getPwr();
        String getSf();
        float getAfcBw();
        float getRxBw();
        u2_t getBitRate();
        u2_t getFdev();
        u2_t getPrLen();
        String getCrc();
        String getIqi();
        String getCr();
        u4_t getWdt();
        u2_t getBw();
        s1_t getSnr();
    
    private:
        void writeReg(u1_t addr, u1_t data);
        u1_t readReg(u1_t addr);
        void writeBuf(u1_t addr, xref2u1_t buf, u1_t len);
        void readBuf(u1_t addr, xref2u1_t buf, u1_t len);
        void requestModuleActive(bit_t state);
        void writeOpmode(u1_t mode);
        void opmode(u1_t mode);
        void opmodeLora();
        void opmodeFSK();

    //attributes
    private:
        u1_t rPaConfig;
        u1_t mc1 = 0, mc2 = 0, mc3 = 0;
        s1_t pwrList[MAX_PWR_VALUES] = {-3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
        float bwList[MAX_BW_VALUES] = {250, 125, 62.5, 31.3, 15.6, 7.8, 3.9, 200, 100, 50, 25, 12.5, 6.3, 3.1, 166.7, 83.3, 41.7, 20.8, 10.4, 5.2, 2.6};
        u1_t bwMantList[MAX_BW_VALUES] = {16, 20, 24, 16, 20, 24, 16, 20, 24, 16, 20 ,24, 16, 20, 24, 16, 20, 24, 16, 20, 24}; //from datasheet sx1276
        u1_t bwExpList[MAX_BW_VALUES] = {1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4 ,4, 5, 5, 5, 6, 6, 6, 7, 7, 7}; //from datasheet sx1276
        String response;
};