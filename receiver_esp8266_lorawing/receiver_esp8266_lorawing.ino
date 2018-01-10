// Feather9x_RX
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client (receiver)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example Feather9x_TX

#include <SPI.h>
#include <RH_RF95.h>

#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// for ESP w/featherwing 
#define RFM95_CS  2    // "E"
#define RFM95_RST 16   // "D"
#define RFM95_INT 15   // "B"


// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// Blinky on receipt 0 for Huzzah 13 for most other Feathers
#define LED 0


// Adafruit IO and Wi-Fi Access Point info
#define WLAN_SSID       "your ssid here"
#define WLAN_PASS       "your password here"
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "your Adafruit IO user name here"
#define AIO_KEY         "your Adafruit IO private key here"

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;

// Store the MQTT server, username, and password in flash memory.
// This is required for using the Adafruit MQTT library.
const char MQTT_SERVER[] PROGMEM    = AIO_SERVER;
const char MQTT_USERNAME[] PROGMEM  = AIO_USERNAME;
const char MQTT_PASSWORD[] PROGMEM  = AIO_KEY;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, AIO_SERVERPORT, MQTT_USERNAME, MQTT_PASSWORD);

/****************************** Feeds ***************************************/

// *** These are just examples! Change these to your own feed names. ***

const char TEMP_FEED[] PROGMEM = AIO_USERNAME "/feeds/LoRaIOXtemp";
Adafruit_MQTT_Publish tempfeed = Adafruit_MQTT_Publish(&mqtt, TEMP_FEED);

const char BATT_FEED[] PROGMEM = AIO_USERNAME "/feeds/LoRaIOXbatt";
Adafruit_MQTT_Publish battfeed = Adafruit_MQTT_Publish(&mqtt, BATT_FEED);

const char TEMP_FEED2[] PROGMEM = AIO_USERNAME "/feeds/LoRaIOXtemp2";
Adafruit_MQTT_Publish tempfeed2 = Adafruit_MQTT_Publish(&mqtt, TEMP_FEED2);

const char BATT_FEED2[] PROGMEM = AIO_USERNAME "/feeds/LoRaIOXbatt2";
Adafruit_MQTT_Publish battfeed2 = Adafruit_MQTT_Publish(&mqtt, BATT_FEED2);

/****************************************************************************/

// Counter to help keep Adafruit IO connection alive
uint32_t connectCounter = 0;

void setup()
{
  pinMode(LED, OUTPUT);
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  // comment this line out if not plugged to serial monitor
  //while (!Serial);
  Serial.begin(115200);
  delay(100);

  Serial.println("Feather LoRa RX Test!");

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

 // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());
  Serial.println("");

  MQTT_connect();
  
}

void loop()
{
  if (rf95.available())
  {
    // Should be a message for us now
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if (rf95.recv(buf, &len))
    {
      digitalWrite(LED, HIGH);
      RH_RF95::printBuffer("Received: ", buf, len);
      Serial.print("Got: ");
      Serial.println((char*)buf);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);
      delay(10);
      // Send a reply
      delay(200); // may or may not be needed
      //uint8_t data[] = "Yo back";
      //rf95.send(data, sizeof(data));
      rf95.send(buf, len);

      /* MQTT publishing code goes here 
       *  
       *          
       *          bool publishResults = true;

        if (RFM_Package[2] == 0x20) // We check Node ID to determine which feeds to send the data to
        { 
          char pubTemp[7];
          dtostrf(temp, 6, 2, pubTemp);   // Don't send raw doubles. They will not display correctly
          if (!tempfeed.publish(pubTemp)) // when graphed in Adafruit IO (currently)
            publishResults = false;
          delay(10);
          char pubBatt[6];
          dtostrf(batt, 4, 2, pubBatt);
          if (!battfeed.publish(pubBatt))
            publishResults = false;
          delay(10);
        } else if (RFM_Package[2] == 0x21) { // Different node ID, upload to different feeds
          char pubTemp[7];
          dtostrf(temp, 6, 2, pubTemp);
          if (!tempfeed2.publish(pubTemp))
            publishResults = false;
          delay(10);
          char pubBatt[6];
          dtostrf(batt, 4, 2, pubBatt);
          if (!battfeed2.publish(pubBatt))
            publishResults = false;
          delay(10);
        }
      
        if (publishResults)
        {
          Serial.println("Data published to Adafruit IO.");
        } else {
          Serial.println("FAILED to publish data to Adafruit IO!");
        }
       */
      
      rf95.waitPacketSent();
      Serial.println("Sent a reply");
      digitalWrite(LED, LOW);
    }
    else
    {
      Serial.println("Receive failed");
    }
  }
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect()
{
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
  }
  Serial.println("MQTT Connected!");
}
