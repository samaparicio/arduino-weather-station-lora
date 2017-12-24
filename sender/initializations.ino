void initializeBME280(void) {

  //initialize the BME280 sensor
  if (!bme.begin()) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }
  delay(100); //lets sensor stretch after being initialized

}
