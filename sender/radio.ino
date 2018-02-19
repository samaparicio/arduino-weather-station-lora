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
      Serial.println("Got reply: ");
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


void sendPacket(String thisPayload) {

  unsigned int payloadLength = thisPayload.length() + 1; //+1 is for the null terminator - https://stackoverflow.com/questions/7383606/converting-an-int-or-string-to-a-char-array-on-arduino
  char payloadAsChar[payloadLength];
  thisPayload.toCharArray(payloadAsChar, payloadLength);

  Serial.println("About to send");
  rf95.send((uint8_t *)payloadAsChar, payloadLength);
  Serial.println("coming back from send");
  delay(10);
  
  rf95.waitPacketSent();
  Serial.println("reached past waitpacketsend");
}
