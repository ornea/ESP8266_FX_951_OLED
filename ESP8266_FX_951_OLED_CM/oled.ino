#if ESP8266_PID_WITH_FEATURE(SSD1306)
void updateOLED(void)
{
  char buffers [10];
  if ((millis() - lastDisplayUpdateMillis) > 50)
  {
    lastDisplayUpdateMillis = millis();// + 50;
    display.clear();

    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(ArialMT_Plain_10);
    if (ISHEATERON)
    {
      display.drawString(display.getWidth() / 2, 0, "HEATING");
    }
    else
    {
      display.drawString(display.getWidth() / 2, 0, "COOLING");
    }
    sprintf(buffers, "PV=%06.2f",  currentHeat); // / INTERVAL_1_SEC );
    display.setFont(ArialMT_Plain_24);
    display.drawString(display.getWidth() / 2, 64 - 24 - 17 - 16  , buffers);
    display.setFont(ArialMT_Plain_16);

    sprintf(buffers, "%%pow=%06.2f",  (MV / 255) * 100); // / INTERVAL_1_SEC );
    display.drawString(display.getWidth() / 2, 64 - 17 - 16, buffers);

    sprintf(buffers, "%s=%06.2f",  cptag[ptrSlider], AppConfig.PID[ptrSlider]); // / INTERVAL_1_SEC );
    display.drawString(display.getWidth() / 2, 64 - 17, buffers);
    display.display();

  }
}
#endif
