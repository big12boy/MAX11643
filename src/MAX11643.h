#include <Arduino.h>
#include <SPI.h>

#define MAX11643_SCANMODE_0toN 0
#define MAX11643_SCANMODE_Nto8 1
#define MAX11643_SCANMODE_Nto16 1
#define MAX11643_SCANMODE_Navg 2
#define MAX11643_SCANMODE_Nonce 3

#define MAX11643_CLKMODE_INT_INT_CVST 0
#define MAX11643_CLKMODE_INT_EXT_CVST 1
#define MAX11643_CLKMODE_INT_INT_AIN 2
#define MAX11643_CLKMODE_EXT_EXT_AIN 3

#define MAX11643_REF_INT_DELAY 0
#define MAX11643_REF_EXT 1
#define MAX11643_REF_INT_FAST 2

#define MAX11643_AVG_COUNT_1 0
#define MAX11643_AVG_COUNT_4 1
#define MAX11643_AVG_COUNT_8 2
#define MAX11643_AVG_COUNT_16 3
#define MAX11643_AVG_COUNT_32 4

#define MAX11643_SCAN_COUNT_4 0
#define MAX11643_SCAN_COUNT_8 1
#define MAX11643_SCAN_COUNT_12 2
#define MAX11643_SCAN_COUNT_16 3

#define MAX11643_SPI_SPEED 1000000
#define MAX11643_SPI_BITORDER MSBFIRST
#define MAX11643_SPI_MODE SPI_MODE0


class MAX11643 {
	public:
		MAX11643(uint16_t _pinCS, uint16_t _pinEOC, uint8_t _chnCount);
		MAX11643(uint16_t _pinCS, uint16_t _pinEOC, uint16_t _pinCVST, uint8_t _chnCount);
		void setup(uint8_t _channelSelectionOrN = 7, uint8_t _scanMode = MAX11643_SCANMODE_0toN, uint8_t _clkMode = 2, uint8_t _refMode = MAX11643_REF_INT_FAST, uint8_t _numAvg = MAX11643_AVG_COUNT_1, uint8_t _cntScans = MAX11643_SCAN_COUNT_4);
		void reset(bool fullReset = false);
		void triggerAcquisition();
		bool getData(bool waitForEOCLow = true);
		uint8_t readChannel(uint8_t chnNum = 0);

	private:
		uint8_t transferMAX(uint8_t valIn);
		void triggerUsingCVST(uint8_t delayus = 2);
		uint8_t adcData[16];
		uint16_t pinCS = 0;
		uint16_t pinEOC = 0;
		uint16_t pinCVST = 0;
		uint8_t chnCount = 0;

		uint8_t channelSelectionOrN = 0;
		uint8_t scanMode = 0;
		uint8_t clkMode = 0;
		uint8_t refMode = 0;
		uint8_t numAvg = 0;
		uint8_t cntScans = 0;

		uint8_t regValConversion = 0;
		uint8_t regValSetup = 0;
		uint8_t regValAveraging = 0;
};
