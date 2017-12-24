/* ADAFRUIT FEATHER 32u4 Lora WEATHER STATION
  pinout https://learn.adafruit.com/system/assets/assets/000/030/918/original/microcomputers_2771_pinout_v1_0.png?1457305552
  Logs sensor data to logger
  sends sensor data over to a Lora Radio
  stack adafruit adalogger featherwing on 32u4 feather - https://www.adafruit.com/products/2922
  connect BME280 via I2C - https://www.adafruit.com/products/2652
  connect INA3221 via I2C - http://www.switchdoc.com/ina3221-breakout-board/
*/

#include <SPI.h> //needed to communicate with the Lora Radio
#include <Wire.h> //needed for I2C communication
#include <RH_RF95.h> //the Lora radio
#include <Adafruit_BME280.h> // needed for the BME280 sensor

/* what pins to use to communicate with the Lora Radio on the 32u4 Feather  */
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 7

// Must match RX's freq!
#define RF95_FREQ 434.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// BME 280
Adafruit_BME280 bme;
#define SEALEVELPRESSURE_HPA (1017.0) //this is a setting to calibrate elevation. Get from NWS for your location

// VARIABLES FOR LOGGING DATA
float temp = 0;
float pressure = 0;
float altitude = 0;
float humidity = 0;
float busvoltage1 = 0;
float current_mA1 = 0;
float busvoltage2 = 0;
float current_mA2 = 0;
float busvoltage3 = 0;
float current_mA3 = 0;
int16_t packetnum = 0;  // packet counter, we increment per xmission
String payload;

void setup()
{
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  while (!Serial);
  Serial.begin(9600);
  delay(100);

  Serial.println("Feather LoRa TX Test!");

  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);

  }
  Serial.println("LoRa radio init OK!");

  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);



  initializeBME280();

}


void loop()
{
  readTheSensors();
  payload = preparePacket();
  sendPacket(payload);
  waitForAckFromReceiver();
  delay(1000);
}


void waitForAckFromReceiver() {
  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  Serial.println("\nWaiting for reply..."); delay(10);
  if (rf95.waitAvailableTimeout(1000))
  {
    // Should be a reply message for us now
    if (rf95.recv(buf, &len))
    {
      Serial.print("Got reply: ");
      Serial.println((char*)buf);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);
    }
    else
    {
      Serial.println("Receive failed");
    }
  }
  else
  {
    Serial.println("No reply, is there a listener around?");
  }
}

