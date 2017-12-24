String preparePacket() {

  String packet = "";

  packet.concat("{\"t\":\"");
  packet.concat(temp);
  packet.concat("\",");
  packet.concat("\"p\":\"");
  packet.concat(pressure);
  packet.concat("\",");
  packet.concat("\"a\":\"");
  packet.concat(altitude);
  packet.concat("\",");
  packet.concat("\"h\":\"");
  packet.concat(humidity);
  packet.concat("\"}");

  return packet;

}

void sendPacket(String thisPayload) {
  
  unsigned int payloadLength = thisPayload.length() + 1; //+1 is for the null terminator - https://stackoverflow.com/questions/7383606/converting-an-int-or-string-to-a-char-array-on-arduino
  char payloadAsChar[payloadLength];
  thisPayload.toCharArray(payloadAsChar, payloadLength);
  rf95.send((uint8_t *)payloadAsChar, payloadLength);
  delay(10);
  rf95.waitPacketSent();
  Serial.println(thisPayload);

}

