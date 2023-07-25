/**
 * Author big12boy
 * Initializes the library, sets up the ADC and gets the analog value of all inputs
 */

#include <SPI.h>
#include <MAX11643.h>

#define MAX11643_CHANNELS 16
#define MAX11643_CS_PIN 6
#define MAX11643_EOC_PIN 5

MAX11643 adc(MAX11643_CS_PIN, MAX11643_EOC_PIN, MAX11643_CHANNELS);


void setup() {
  Serial.begin(115200);
  Serial.println(""); Serial.println(""); Serial.println(""); Serial.println("");
  Serial.println("MAX11638EEE DEMO - Start!");
  SPI.begin(); //Needed for the ADC library to work

  adc.setup(MAX11643_CHANNELS-1, MAX11643_SCANMODE_0toN, MAX11643_CLKMODE_INT_INT_AIN, MAX11643_REF_EXT, MAX11643_AVG_COUNT_1, MAX11643_SCAN_COUNT_4); //Use channels 0 up to 15, scan mode 0 to N, internally timed (fast), internal reference, no averages
}

uint8_t dataIn = 0;

void loop() {
  delay(1000);

  Serial.print("Getting data: ");
  adc.triggerAcquisition();

  if(!adc.getData()){
    Serial.println("timed out!");
    return;
  }

  for(int i = 0; i < MAX11643_CHANNELS; i++){
    Serial.print(" CHN");
    Serial.print(i);    
    Serial.print("=");
    dataIn = adc.readChannel(i);
    Serial.print(dataIn);
  }
  Serial.println("");
}