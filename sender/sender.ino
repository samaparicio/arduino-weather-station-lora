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
#include <math.h> // used by the weather vane

// Lora Radio on the M0 Feather
#define RFM95_CS 8 //SPI CS for radio
#define RFM95_RST 4
#define RFM95_INT 3
#define RF95_FREQ 915.0 // Must match RX's freq
RH_RF95 rf95(RFM95_CS, RFM95_INT); // Singleton instance of the radio driver

// WIND AND RAIN SENSORS
// for interrupts on M0, safe choices are A1 to A4 based on this https://github.com/arduino/ArduinoCore-samd/issues/136#issuecomment-232248144
// at some point I used "6" instead of A1 and it didn't work. A2 didn't work either messed with radio.
#define WINDSPEEDPIN (A1) // The pin location of the anemometer sensor 
#define RAINBUCKETPIN (A3) // The pin location of the rain bucket
#define WINDDIRECTIONPIN (A4) // The pin location of the weather vane
int rawWindDirection; // translated 0 - 360 direction
#define rawWindDirectionOffset 0; // if the weather vane needs to be calibrated for 'true north'

// BME 280
Adafruit_BME280 bme;
#define SEALEVELPRESSURE_HPA (1031.0) //this is a setting to calibrate elevation. Get from NWS for your location

// SD CARD LOGGING
File logfile;
#define SD_CS 10 // the chip select pin that drives the SD Card SPI

// Real Time Clock
RTC_PCF8523 rtc;

// Weather Vane
volatile unsigned long rotations; // cup rotation counter used in interrupt routine 
volatile unsigned long contactBounceTime; // Timer to avoid contact bounce in interrupt routine 
int rawVaneReading; // raw voltage value from wind vane

// Rain Bucket
const int minimumInterval = 500; //minimum interval between bucket swings. Debounces the switch
volatile unsigned long lastTimeBucketSwung = millis();


// VARIABLES FOR LOGGING SENSOR DATA
float temp = 0;
float pressure = 0;
float altitude = 0;
float humidity = 0;
float windSpeed = 0.0; // speed miles per hour 
int windDirection = 0; // 0 to 359 degrees
String windHeading = "N"; // N, NW, SE, etc
double rainPrecipitationRate = 0.0; // mm/hr of rainfall between rain bucket swings



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
  initializeWeatherVane();
  initializeRTC();
  initializeSDCard();
  initializeRainBucket();
}


void loop()
{
  readTheSensors();
  payload = preparePacket();
  logThings(payload);
  Serial.println("Sending packet");
  sendPacket(payload);
  Serial.println("Waiting for receiver");
  waitForAckFromReceiver();
  delay(20000);
}



void error(char *str)
{
  Serial.print("error: ");
  Serial.println(str);
  while (1);
}

