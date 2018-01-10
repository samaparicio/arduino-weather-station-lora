void readTheSensors() {
  temp = bme.readTemperature();
  pressure = bme.readPressure() / 100.0F;
  altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  humidity = bme.readHumidity();
  readwindSpeed();
  readWindwindDirection();
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

// This is the function that the interrupt calls to increment the rotation count
void isr_rotation () {

  if ((millis() - contactBounceTime) > 15 ) { // debounce the switch contact.
    rotations++;
    contactBounceTime = millis();
  }

}


void readWindwindDirection() {
  rawVaneReading = analogRead(A4);
  windDirection = map(rawVaneReading, 0, 1023, 0, 360);
  windroseDirection = windDirection + Offset;

  if (windroseDirection > 360)
    windroseDirection = windroseDirection - 360;

  if (windroseDirection < 0)
    windroseDirection = windroseDirection + 360;

  // Only update the display if change greater than 2 degrees.
  if (abs(windroseDirection - LastValue) > 5)
  {
    //Serial.print(rawVaneReading); Serial.print("\t\t");
    //Serial.print(windroseDirection); Serial.print("\t\t");
    getHeading(windroseDirection);
    LastValue = windroseDirection;
  }

  delay(500);
}

// Converts compass direction to heading
void getHeading(int direction) {
  if (direction < 22)
    Serial.println("N");
  else if (direction < 67)
    Serial.println("NE");
  else if (direction < 112)
    Serial.println("E");
  else if (direction < 157)
    Serial.println("SE");
  else if (direction < 212)
    Serial.println("S");
  else if (direction < 247)
    Serial.println("SW");
  else if (direction < 292)
    Serial.println("W");
  else if (direction < 337)
    Serial.println("NW");
  else
    Serial.println("N");
}


