/*void sendHeartBeat(void)
{
  IPAddress ipaddr;
  char buffers [200];
  //char ipaddr[50];
  ipaddr = WiFi.localIP();//.toString();
  ////Serial.println(F("Sending HeartBeat ... " );

  sprintf(buffers, "%shn=%s&ip=%03d.%03d.%03d.%03d&ticks=%010d&heat=%06.2f&Humidity=%06.2f&pid=%03d&err=%06.2f&iterm=%06.2f&dTerm=%06.2f&output=%06.2f&ms=%010d",
          URL_Heartbeat, AppConfig.EpHostname, ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3], ticks, currentHeat, currentHumidity, MV, error, iTerm, dTerm, getPinMode(HEATER_PIN), millis());

  http.begin(buffers);
  //Serial.print(buffers);
  int httpCode = http.GET();
  //Serial.print(F(" Http Resp: "));
  //Serial.println(httpCode);
  //  if (httpCode == HTTP_CODE_OK)
   // {
   ///   String payload = http.getString();
   //   //Serial.println(payload);
  //  }
  
  http.end();
}
*/
