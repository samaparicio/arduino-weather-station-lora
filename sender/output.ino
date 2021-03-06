/* Sample packet
    {"d":"2065/7/15 31:11:4","t":"23.28","p":"1024.09","a":"56.69","h":"23.23","s":"45.00","di":"280", "wh":"N", "r":"2.3"}
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

String preparePacket() {

  String packet = "";

  // fetch the time from the RTC clock
  DateTime now = rtc.now();
  String date = printDate(now);

  packet.concat("{\"d\":\"");
  packet.concat(date);
  packet.concat("\",\"t\":\"");
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
  packet.concat("\",\"s\":\"");
  packet.concat(windSpeed);
  packet.concat("\",\"di\":\"");
  packet.concat(windDirection);
  packet.concat("\",\"wh\":\"");
  packet.concat(windHeading);
  packet.concat("\",\"r\":\"");
  packet.concat(rainPrecipitationRate);
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


