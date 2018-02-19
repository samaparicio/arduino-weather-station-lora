void readTheSensors() {
  temp = bme.readTemperature();
  pressure = bme.readPressure() / 100.0F;
  altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  humidity = bme.readHumidity();
  readwindSpeed();
  readWindDirection();
}

void readwindSpeed() {

  rotations = 0; // Set rotations count to 0 ready for calculations

  //interrupts(); // Enables interrupts
  delay (3000); // Wait 3 seconds to average
  //noInterrupts(); // Disable interrupts

  // convert to mp/h using the formula V=P(2.25/T)
  // V = P(2.25/3) = P * 0.75

  windSpeed = rotations * 0.75;

}

/* ISR that is called when the wind speed sensor activates. Increments the rotation count */
void windSpeedRotation () {
  if ((millis() - contactBounceTime) > 15 ) { // debounce the switch contact.
    rotations++;
    contactBounceTime = millis();
  }
    Serial.println("wind speed ISR");

}


void readWindDirection() {
  //read a voltage from the anemometer sensor line and convert it to a direction
  rawVaneReading = analogRead(WINDDIRECTIONPIN);
  rawWindDirection = map(rawVaneReading, 0, 1023, 0, 360);
  windDirection = rawWindDirection + rawWindDirectionOffset;

  if (windDirection > 360)
    windDirection = windDirection - 360;

  if (windDirection < 0)
    windDirection = windDirection + 360;

  windHeading = getHeading(windDirection);

  //TODO is this really necessary?
  delay(500);
}

// Converts compass direction to heading
String getHeading(int direction) {
  if (direction < 22)
    return "N";
  else if (direction < 67)
    return  "NE";
  else if (direction < 112)
    return "E";
  else if (direction < 157)
    return "SE";
  else if (direction < 212)
    return "S";
  else if (direction < 247)
    return "SW";
  else if (direction < 292)
    return "W";
  else if (direction < 337)
    return "NW";
  else
    return "N";
}

/* ISR triggered by swings of the rain bucket because it is full */
void rainBucketSwing() {
  noInterrupts();
  unsigned long currentTime = millis();

  //debounce
  if ((currentTime - lastTimeBucketSwung) < minimumInterval) {
    interrupts();
    return;
  }

  unsigned long intervalBetweenBucketSwings = currentTime - lastTimeBucketSwung;

  //update the last time it swung to now, for next interrupt
  lastTimeBucketSwung = currentTime;


  //here would go the code to calculate the rainfall
  // https://github.com/CurtisIreland/solinst-array/blob/master/Arduino/RainCounter/RainCounter.ino

  rainPrecipitationRate = 914400.0 / intervalBetweenBucketSwings;
  Serial.println("rain bucket swung");

  interrupts();
}

