#include "MAX11643.h"

MAX11643::MAX11643(uint16_t _pinCS, uint16_t _pinEOC, uint8_t _chnCount){
    pinCS = _pinCS;
    pinEOC = _pinEOC;
    chnCount = _chnCount;
}
MAX11643::MAX11643(uint16_t _pinCS, uint16_t _pinEOC, uint16_t _pinCVST, uint8_t _chnCount){
    pinCS = _pinCS;
    pinEOC = _pinEOC;
    pinCVST = _pinCVST;
    chnCount = _chnCount;
}

void MAX11643::setup(uint8_t _channelSelectionOrN, uint8_t _scanMode, uint8_t _clkMode, uint8_t _refMode, uint8_t _numAvg, uint8_t _cntScans) {
    //Confirm parameters are within range
    if(_channelSelectionOrN > chnCount || _scanMode > 3 || _clkMode > 3 || _refMode > 3 || _numAvg > 3 || _cntScans > 3 ){
        return;
    }

    //Transfer to internal variables for later use
    channelSelectionOrN = _channelSelectionOrN;
    scanMode = _scanMode;
    clkMode = _clkMode;
    refMode = _refMode;
    numAvg = _numAvg;
    cntScans = _cntScans;

    //Reset adcData storage variable
    for (int i = 0; i < 16; i++){
        adcData[i] = 0;
    }

    //Setup CS pin mode and disable chip
    pinMode(pinCS, OUTPUT);
    digitalWrite(pinCS, HIGH);

    //Setup CVST pin if requested
    if(clkMode == MAX11643_CLKMODE_INT_INT_CVST || clkMode == MAX11643_CLKMODE_INT_EXT_CVST){
        pinMode(pinCVST, OUTPUT);
        digitalWrite(pinCVST, HIGH);
    }

    //Setup EOC pin to be input with pullup, interrupt needs to be attached in the main file
    pinMode(pinEOC, INPUT_PULLUP);

    delay(1); // Wait for CS and EOC to rise to normal operating levels

    //Converts values to register entrees
    regValConversion = 1 << 7 | channelSelectionOrN << 3 | scanMode << 1;
    regValSetup = 1 << 6 | clkMode << 4 | refMode << 2;
    bool avgOn = numAvg > 0;
    regValAveraging = 1 << 5 | avgOn << 4 | numAvg << 2 | cntScans;

    //Reset chip completely
    reset(true);

    //Send values to the ADC
    transferMAX(regValConversion);
    transferMAX(regValSetup);
    transferMAX(regValAveraging);
}

uint8_t regValReset = 0b00010000; //Fully reset chip
uint8_t regValResetVal = 0b00011000; //Only clear adc output registers
void MAX11643::reset(bool fullReset) {
    if(fullReset) transferMAX(regValReset);
    else transferMAX(regValResetVal);
}

void MAX11643::triggerAcquisition() {
    switch(clkMode) {
        case MAX11643_CLKMODE_INT_INT_CVST:
            triggerUsingCVST(1);
            break;

        case MAX11643_CLKMODE_INT_EXT_CVST:
            uint8_t delayus;
            if(refMode == MAX11643_REF_INT_DELAY) delayus = 70; //If powered down, startup delay is needed
            else delayus = 2;
            triggerUsingCVST(delayus);
            break;

        case MAX11643_CLKMODE_INT_INT_AIN:
            transferMAX(regValConversion);
            break;

        case MAX11643_CLKMODE_EXT_EXT_AIN:
            transferMAX(regValConversion);
            if(refMode == MAX11643_REF_INT_DELAY) delayMicroseconds(65);
            getData(false);
            break;

        default:
            break;
    }
}


bool MAX11643::getData(bool waitForEOCLow) {
    uint8_t valueIn = 0, valA = 0, valB = 0, valX = 0, cnt = 0;
    
    if(waitForEOCLow){
        while(digitalRead(pinEOC) && cnt < 100){
            delayMicroseconds(10);
            cnt++;
        } 
        if(cnt >= 100) return false;
    }
    
    for(int i = 0; i < 16; i ++){
        valA = transferMAX(valX);  //First byte (MSB) of the ADC value x
        valB = transferMAX(valX);  //Last byte (LSB) of the ADC value x
        valueIn = ((valA & 0b00001111) << 4) | ((valB & 0b11110000) >> 4); //convert separate bytes to single adc value
        adcData[i] = valueIn;
    }
    return true;
}

uint8_t MAX11643::readChannel(uint8_t chnNum) {
    return adcData[chnNum];
}

uint8_t MAX11643::transferMAX(uint8_t chnNum) {
    uint8_t retVal = 0;
    SPI.beginTransaction(SPISettings(MAX11643_SPI_SPEED, MAX11643_SPI_BITORDER, MAX11643_SPI_MODE));
    digitalWrite(pinCS, LOW);
    retVal = SPI.transfer(chnNum);
    digitalWrite(pinCS, HIGH);
    SPI.endTransaction();
    return retVal;
}

void MAX11643::triggerUsingCVST(uint8_t delayus) {
    digitalWrite(pinCVST, LOW);
    delayMicroseconds(delayus);
    digitalWrite(pinCVST, HIGH);
}
