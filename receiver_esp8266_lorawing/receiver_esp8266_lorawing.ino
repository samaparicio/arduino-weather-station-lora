/*
   Lora --> MQTT --> Adafruit.io gateway
   Stack an Adafruit Radio Featherwing on top of a Feather Huzzah ESP8266
   Packets like so: {"d": "2065/7/15 31:11:4","t":"23.28","p":"1024.09","a":"56.69","h":"23.23","s":"45.00","di":"280"}
   come from the Lora radio, JSON parsed and values sent to separate feeds in IO
*/

#include <SPI.h>
#include <RH_RF95.h>

#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#include <ArduinoJson.h>

// pinouts for ESP w/featherwing (you will need to solder these)
#define RFM95_CS  2    // "E"
#define RFM95_RST 16   // "D"
#define RFM95_INT 15   // "B"


// must match RX's freq
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// Blinky on receipt 0 for Huzzah 13 for most other Feathers
#define LED 0


// Adafruit IO and Wi-Fi Access Point info
#define WLAN_SSID       "SubDudeIOT"
#define WLAN_PASS       "ringoR0cks"
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "samap"
#define AIO_KEY         "244638ccc709f6f278aaf700a3e406822a7c4a7d"

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;

// Store the MQTT server, username, and password in flash memory.
// This is required for using the Adafruit MQTT library.
const char MQTT_SERVER[]     = AIO_SERVER;
const char MQTT_USERNAME[]   = AIO_USERNAME;
const char MQTT_PASSWORD[]   = AIO_KEY;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, AIO_SERVERPORT, MQTT_USERNAME, MQTT_PASSWORD);

/****************************** Feeds ***************************************/


const char TEMP_FEED[]  = AIO_USERNAME "/feeds/weather.temperature";
Adafruit_MQTT_Publish tempfeed = Adafruit_MQTT_Publish(&mqtt, TEMP_FEED);

const char PRES_FEED[]  = AIO_USERNAME "/feeds/weather.pressure";
Adafruit_MQTT_Publish presfeed = Adafruit_MQTT_Publish(&mqtt, PRES_FEED);

const char HUMI_FEED[]  = AIO_USERNAME "/feeds/weather.humidity";
Adafruit_MQTT_Publish humifeed = Adafruit_MQTT_Publish(&mqtt, HUMI_FEED);

const char WSPEED_FEED[]  = AIO_USERNAME "/feeds/weather.wind-speed";
Adafruit_MQTT_Publish wspeedfeed = Adafruit_MQTT_Publish(&mqtt, WSPEED_FEED);

const char WDIR_FEED[]  = AIO_USERNAME "/feeds/weather.wind-direction";
Adafruit_MQTT_Publish wdirfeed = Adafruit_MQTT_Publish(&mqtt, WDIR_FEED);

const char PRECI_FEED[]  = AIO_USERNAME "/feeds/weather.precipitation";
Adafruit_MQTT_Publish precifeed = Adafruit_MQTT_Publish(&mqtt, PRECI_FEED);

const char WHEAD_FEED[] = AIO_USERNAME "/feeds/weather.wind-heading";
Adafruit_MQTT_Publish wheadfeed = Adafruit_MQTT_Publish(&mqtt, WHEAD_FEED);

const char ALTI_FEED[] = AIO_USERNAME "/feeds/weather.altitude";
Adafruit_MQTT_Publish altifeed = Adafruit_MQTT_Publish(&mqtt, ALTI_FEED);



// Counter to help keep Adafruit IO connection alive
uint32_t connectCounter = 0;

void setup()
{
  initializeSerial();
  initializeLora();
  initializeWifi();
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
      //RH_RF95::printBuffer("Received: ", buf, len);
      //Serial.print("Got: ");
      Serial.println((char*)buf);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);
      delay(10);

      /*
         Sample packet:
         {"d":"2065/5/13 19:16:6","t":"23.99","p":"1018.82","a":"100.17","h":"30.93","s":"0.00","di":"118","wh":"SE","r":"32.07"}
         Use https://arduinojson.org/assistant/ to size up
             d = date
             t = temperature
             p = pressure
             a = altitude
             h = humidity
             s = wind speed
             di = wind direction
             wh = wind heading
             r = rain precipitation
      */

      /*StaticJsonBuffer<235> jsonBuffer;

        JsonObject& json = jsonBuffer.parseObject((const char *) buf);

        if (!json.success()) {
        Serial.println("json parse failed");
        return;
        }

      */

      const size_t bufferSize = JSON_OBJECT_SIZE(9) + 100;
      DynamicJsonBuffer jsonBuffer(bufferSize);

      //const char* json = "{\"d\":\"2065/5/13 19:33:2\",\"t\":\"24.10\",\"p\":\"1018.83\",\"a\":\"100.04\",\"h\":\"30.85\",\"s\":\"0.00\",\"di\":\"137\",\"wh\":\"SE\",\"r\":\"32.07\"}";

      JsonObject& root = jsonBuffer.parseObject((const char *) buf);



      const char* d = root["d"]; // "2065/7/15 31:11:4"
      const char* t = root["t"]; // "23.28"
      const char* p = root["p"]; // "1024.09"
      const char* a = root["a"]; // "56.69"
      const char* h = root["h"]; // "23.23"
      const char* s = root["s"]; // "45.00"
      const char* di = root["di"]; // "45.00"
      const char* wh = root["wh"]; // "SE"
      const char* r = root["r"]; // "32.07"

      bool publishResults = true;

      if (!tempfeed.publish(t))
        publishResults = false;
      delay(10);

      if (!presfeed.publish(p))
        publishResults = false;
      delay(10);

      if (!humifeed.publish(h))
        publishResults = false;
      delay(10);

      if (!wspeedfeed.publish(s))
        publishResults = false;
      delay(10);

      if (!wdirfeed.publish(di))
        publishResults = false;
      delay(10);

      if (!wheadfeed.publish(wh))
        publishResults = false;
      delay(10);

      if (!precifeed.publish(r))
        publishResults = false;
      delay(10);

      if (!altifeed.publish(p))
        publishResults = false;
      delay(10);

      if (publishResults)
      {
        Serial.println("Data published to Adafruit IO.");
      } else {
        Serial.println("FAILED to publish data to Adafruit IO!");
      }

      // Send packet back
      delay(200); // may or may not be needed
      rf95.send(buf, len);
      rf95.waitPacketSent();
      Serial.println("Sent a reply");
      digitalWrite(LED, LOW);

      delay(500);

    }
    else
    {
      Serial.println("Receive failed");
    }
  }
}

