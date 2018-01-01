String preparePacket() {

  String packet = "";

  // fetch the time from the RTC clock
  DateTime now = rtc.now();
  String date = printDate(now);

  packet.concat("{\"d\":\"");
  packet.concat(date);
  packet.concat("\",\"t:\"");
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

void logThings(String packet) {
  Serial.println("Logging in SD:");
  Serial.println(packet);

  logfile.print(packet);
  logfile.println();
  logfile.flush();

}

String printDate(DateTime d)
{
  String date = "";
  date.concat(d.year());
  date.concat("/");
  date.concat(d.month());
  date.concat("/");
  date.concat(d.day());
  date.concat(" ");
  date.concat(d.hour());
  date.concat(":");
  date.concat(d.minute());
  date.concat(":");
  date.concat(d.second());
  return date;
}


