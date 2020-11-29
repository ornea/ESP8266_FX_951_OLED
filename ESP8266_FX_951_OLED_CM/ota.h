//**************
//************** Init Over The Air Updates
//**************
ArduinoOTA.setHostname(AppConfig.EpHostname);
//Serial.println("Well here we are");
ArduinoOTA.onStart([]() {
  String type;
  if (ArduinoOTA.getCommand() == U_FLASH)
    type = "sketch";
  else // U_SPIFFS
    type = "filesystem";

  // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
  HEATER_OFF; //lets turn off the HEATER_PIN while we update
  pinMode(HEATER_PIN, INPUT);

#if ESP8266_PID_WITH_FEATURE(SSD1306)
  display.clear();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
  display.drawString(display.getWidth() / 2, display.getHeight() / 2 - 10, "OTA Update");
  display.display();

#endif
  //Serial.print(F("Start updating "));
  //Serial.println(type);
});

ArduinoOTA.onEnd([]() {
  pinMode(HEATER_PIN, INPUT);
  //Serial.println(F("\nEnd"));
#if ESP8266_PID_WITH_FEATURE(SSD1306)

  display.clear();
  display.drawXbm(0, 0, Logo_width, Logo_height, Logo_bits);
  display.display();
#endif
  /*
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
    display.drawString(display.getWidth() / 2, display.getHeight() / 2, "Restart");
    display.display();
  */
});
ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
  pinMode(HEATER_PIN, INPUT);
  //Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
#if ESP8266_PID_WITH_FEATURE(SSD1306)

  display.drawProgressBar(4, 32, 120, 8, progress / (total / 100) );
  display.display();
#endif
});
ArduinoOTA.onError([](ota_error_t error) {
  pinMode(HEATER_PIN, INPUT);
  //Serial.printf("Error[%u]: ", error);
  if (error == OTA_AUTH_ERROR) Serial.println(F("Auth Failed"));
  else if (error == OTA_BEGIN_ERROR) Serial.println(F("Begin Failed"));
  else if (error == OTA_CONNECT_ERROR) Serial.println(F("Connect Failed"));
  else if (error == OTA_RECEIVE_ERROR) Serial.println(F("Receive Failed"));
  else if (error == OTA_END_ERROR) Serial.println(F("End Failed"));
});
ArduinoOTA.begin();
//Serial.print(F("IP address: "));
//Serial.println(WiFi.localIP());
//**************
