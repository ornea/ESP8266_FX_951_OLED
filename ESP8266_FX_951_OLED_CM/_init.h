/*This code included in setup initialises Serial, checks AppConfig struct < 511 bytes,
  initialises AppConfig struct with values from EEPROM
  Attempts to connect Wifi
  Starts MDNS
*/

int WiFiTimeout = 0;

Serial.begin(SerialSpeed);

#if ESP8266_PID_WITH_FEATURE(SSD1306)
// Initialize OLED display //klarsys github lib
// display.begin();
// display.print("Hello World");

//ThingPulse, Daniel Eichhorn  2018 by Fabrice
display.init();
display.flipScreenVertically();
//display.setFont(ArialMT_Plain_10);
#endif

for (uint8_t t = 4; t > 0; t--)
{
  Serial.println(F("[SETUP] WAIT ..\n"));
  Serial.flush();
  delay(100);
}
////Serial.println(*outputHeading);//prints first element
////Serial.println(F("SizeOf AppConfig:"));
////Serial.println(sizeof(AppConfig));
if (sizeof(AppConfig) > 511)
{
  while (1);
}
OutAppConfig();//Print Pre EEPROM Config

// Initialize application related NV variables
InitAppConfig();
OutAppConfig();
////Serial.println();
////Serial.print(F("Hostname: "));

if (AppConfig.EpHostname == "notset")
{
  sprintf(AppConfig.EpHostname, "ESP_%06X", ESP.getChipId());
}
////Serial.println(AppConfig.EpHostname);
////Serial.print(F("Build Date: "));
////Serial.print(__DATE__);
////Serial.print(F("   Time: "));
////Serial.print(__TIME__);
////Serial.print(F("   Board: "));
////Serial.println(ARDUINO_BOARD);

WiFi.disconnect();
//WiFi.setAutoConnect(false);
WiFi.persistent(false);  //thid solved a wierd  problem with a cisco router used as dhcp server
WiFi.mode(WIFI_STA);    // As End Point Only (ie wifi Station/Client)
WiFi.hostname(AppConfig.EpHostname);
if (AppConfig.Flags.bUseStaticIP)
{
  WiFi.config(AppConfig.EpIP, AppConfig.EpDNS, AppConfig.EpGW, AppConfig.EpMASK);
}

for (uint8_t ap_list_index = 0; (ap_list_index < numAccessPoints); ap_list_index++)
{
  ////Serial.println(sizeof(AppConfig.access_point));
//  //Serial.println();
//  //Serial.print(F("Trying WAP:"));
//  //Serial.print(AppConfig.access_point[ap_list_index][0]);

#if ESP8266_PID_WITH_FEATURE(SSD1306)
  display.clear();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
  display.drawString(display.getWidth() / 2, display.getHeight() / 4,  "Trying SSID");
  display.drawString(display.getWidth() / 2, display.getHeight() / 2,  AppConfig.access_point[ap_list_index][indx_SSID]);
  display.display();
#endif

  WiFi.begin(AppConfig.access_point[ap_list_index][indx_SSID], AppConfig.access_point[ap_list_index][indx_KEY]);
  WiFiTimeout = 0;
  while ((WiFi.status() != WL_CONNECTED) && (++WiFiTimeout != 15) )
  {
    delay(500);
//    //Serial.print(F("."));
  }
  if (WiFi.status() == WL_CONNECTED)
  {
//    //Serial.println(F("Connected OK"));
    break;
  }
}
if (WiFi.status() != WL_CONNECTED) //WiFiTimeout == 10)
{
  yield();
 // //Serial.println(F("Huston we have a problem. Trying to create Access Point"));
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);     // As Access Point Only

  //bool softAPConfig(IPAddress local_ip, IPAddress gateway, IPAddress subnet);
  WiFi.softAPConfig(AppConfig.ApIP, AppConfig.ApGW, AppConfig.ApMASK);
  WiFi.softAP(AppConfig.ApSSID);
  delay(3500);
}
////Serial.println();
////Serial.print(F("IP Address:"));
////Serial.println(WiFi.localIP());

#if ESP8266_PID_WITH_FEATURE(SSD1306)
display.clear();
display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
display.setFont(ArialMT_Plain_10);
display.drawString(display.getWidth() / 2, display.getHeight() / 2, "Wifi Connected:\n" + WiFi.localIP().toString() + "\nHostname:\n" + AppConfig.EpHostname + ".local/");
display.display();
delay(2500);
#endif
#if ESP8266_PID_WITH_FEATURE(mDNS)
if (MDNS.begin(AppConfig.EpHostname))
{
  //Serial.println(F("MDNS responder started"));
  MDNS.addService("esp", "tcp", 8080);
}
#endif
