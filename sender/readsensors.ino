void readTheSensors() {
  temp = bme.readTemperature();
  pressure = bme.readPressure() / 100.0F;
  altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);
  humidity = bme.readHumidity();
  readWindSpeed();
  readWindDirection();
}

void readWindSpeed() {

  Rotations = 0; // Set Rotations count to 0 ready for calculations

  //interrupts(); // Enables interrupts
  delay (3000); // Wait 3 seconds to average
  //noInterrupts(); // Disable interrupts

  // convert to mp/h using the formula V=P(2.25/T)
  // V = P(2.25/3) = P * 0.75

  WindSpeed = Rotations * 0.75;

}

// This is the function that the interrupt calls to increment the rotation count
void isr_rotation () {

  if ((millis() - ContactBounceTime) > 15 ) { // debounce the switch contact.
    Rotations++;
    ContactBounceTime = millis();
  }

}


void readWindDirection() {
  VaneValue = analogRead(A4);
  Direction = map(VaneValue, 0, 1023, 0, 360);
  CalDirection = Direction + Offset;

  if (CalDirection > 360)
    CalDirection = CalDirection - 360;

  if (CalDirection < 0)
    CalDirection = CalDirection + 360;

  // Only update the display if change greater than 2 degrees.
  if (abs(CalDirection - LastValue) > 5)
  {
    //Serial.print(VaneValue); Serial.print("\t\t");
    //Serial.print(CalDirection); Serial.print("\t\t");
    getHeading(CalDirection);
    LastValue = CalDirection;
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


