/* ADAFRUIT FEATHER M0 Lora WEATHER STATION - https://learn.adafruit.com/adafruit-feather-m0-radio-with-lora-radio-module/overview
  Logs sensor data and timestamp via SPI to SD card in Adalogger featherwing
  sends sensor data over to a Lora Radio
  stack adafruit adalogger featherwing on m0 lora feather - https://learn.adafruit.com/adafruit-adalogger-featherwing
  connect BME280 via I2C - https://learn.adafruit.com/adafruit-bme280-humidity-barometric-pressure-temperature-sensor-breakout
  connect INA3221 via I2C - http://www.switchdoc.com/ina3221-breakout-board/
*/

#include <SPI.h> //needed to communicate with the Lora Radio and SD Card
#include <Wire.h> //needed for I2C communication with the BME280 sensor and INA3221
#include <RH_RF95.h> //the Lora radio
#include <Adafruit_BME280.h> // needed for the BME280 sensor
#include <SD.h> //needed for the SD card part of the adalogger
#include <RTClib.h> //needed for the RTC chip in the adalogger

#include <math.h>

/* what pins to use to communicate with the Lora Radio on the M0 Feather  */
#define RFM95_CS 8 //SPI CS for radio
#define RFM95_RST 4
#define RFM95_INT 3

#define SD_CS 10 // the chip select pin that drives the SD Card SPI
#define WindSensorPin (6) // The pin location of the anemometer sensor 
#define Offset 0;


// Must match RX's freq
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// BME 280
Adafruit_BME280 bme;
#define SEALEVELPRESSURE_HPA (1031.0) //this is a setting to calibrate elevation. Get from NWS for your location

// SD CARD LOGGING
File logfile;


// Real Time Clock
RTC_PCF8523 rtc;


// Weather Vane
volatile unsigned long Rotations; // cup rotation counter used in interrupt routine 
volatile unsigned long ContactBounceTime; // Timer to avoid contact bounce in interrupt routine 
float WindSpeed; // speed miles per hour 

int VaneValue;// raw analog value from wind vane
int Direction;// translated 0 - 360 direction
int CalDirection;// converted value with offset applied
int LastValue;



// VARIABLES FOR LOGGING DATA
float temp = 0;
float pressure = 0;
float altitude = 0;
float humidity = 0;

/*float busvoltage1 = 0;
float current_mA1 = 0;
float busvoltage2 = 0;
float current_mA2 = 0;
float busvoltage3 = 0;
float current_mA3 = 0;
*/

String payload; //to hold the sensor measurements that will be logged and tx'ed

void setup()
{
  initializeSerial();
  initializeLoraRadio();
  initializeBME280();
  initializeRTC();
  initializeSDCard();
  initializeWeatherVane();

}


void loop()
{
  readTheSensors();
  payload = preparePacket();
  logThings(payload);
  sendPacket(payload);
  waitForAckFromReceiver();
  delay(1000);
}



void error(char *str)
{
  Serial.print("error: ");
  Serial.println(str);
  while (1);
}

