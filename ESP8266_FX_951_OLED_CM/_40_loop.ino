//ESP8266 crashes unless these are called regularly
void doservers(void)
{
  server.handleClient();
  ArduinoOTA.handle();
  webSocket.loop();
  //r.loop();
  yield();
}
unsigned long oldermicros = 0;
unsigned long loopTime = 0;
//int setPointIndex=0;
void loop()
{
  char buffers [10];
  unsigned char k ;
  doservers(); //do housekeeping
  currentMillis = millis();
  // DEBUG0_PRINT(F("Loop Time in mS:"));
  //DEBUG4_PRINTLN(currentMillis - oldermillis);
  loopTime = micros() - oldermicros;
  oldermicros = micros();


#if ESP8266_PID_WITH_FEATURE(SSD1306)
  updateOLED();
#endif
  if (updateHTML && (currentMillis > holdDownTimer))
  {
    updateHTML = FALSE;
    sendButtonText();
    sendControlParams();
    sendPIDData();

#if ((TEMPERATURE_SENSOR_TYPE(DHT11) || TEMPERATURE_SENSOR_TYPE(DHT21) || TEMPERATURE_SENSOR_TYPE(DHT22)) && ESP8266_PID_WITH_FEATURE(HUMIDIFIER))
    broadcastPumpState();
#endif
    DEBUG0_PRINTLN("Level 0");
    DEBUG1_PRINTLN("Level 1");
    DEBUG2_PRINTLN("Level 2");
    DEBUG3_PRINTLN("Level 3");
    DEBUG4_PRINTLN("Level 4");
    DEBUG5_PRINTLN("Level 5");

  }
  //////////////////////////////////////////////////////////////////////////////////////////////////////////

  if (updateButtonsReq)
  {
    updateButtonsReq = FALSE;
    sendButtonText();
  }
  //////////////////////////////////////////////////////////////////////////////////////////////////////////

  if (updateSlidersReq)
  {
    updateSlidersReq = FALSE;
    sendControlParams();
  }

  if (updatePIDDataReq)
  {
    updatePIDDataReq = FALSE;
    sendPIDData();
  }
#if ESP8266_PID_WITH_FEATURE(ROTARY_ENCODER)

  for (int i = 0; i < NBR_BUTTS; i++)
  {
    smButtons(i);
  }
#endif
  //////////////////////////////////////////////////////////////////////////////////////////////////////////
#if ((TEMPERATURE_SENSOR_TYPE(DHT11) || TEMPERATURE_SENSOR_TYPE(DHT21) || TEMPERATURE_SENSOR_TYPE(DHT22)) && ESP8266_PID_WITH_FEATURE(HUMIDIFIER))
  //Do  pump state machine
  smPump();
#endif
  if ((currentMillis - lastPIDmillis) > (AppConfig.PID[tc] * 1000))
  {
   // k = (EEPROM.read(512 + setPointIndex++));
   // DEBUG1_PRINT(F("Index: ")); DEBUG1_PRINT(setPointIndex); DEBUG1_PRINT(F("Char: ")); DEBUG1_PRINT(k); DEBUG1_PRINTLN(char(k));
	//AppConfig.PID[sp1td] = 0;
	//AppConfig.PID[sp2td] = 0;
	//AppConfig.PID[sp1] = k;
	//AppConfig.PID[sp2] = k;
	
    smHeat(); //go and do the heating profile state machine thing.  Update MV while there

    if (isnan(currentHeat))
    {
      DEBUG4_PRINTLN("Setting MV = 0 as reading is NaN");
      MV = 0;
    }

    sendPIDData();
    sendButtonText();
    sendControlParams();
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////////
  switch (AppConfig.Mode)
  {
    case run_pid:
      {
        PWM = MV;
      }
      break;
    case forced_on:
      {
        PWM = 255;
      }
      break;
    case forced_off:
      {
        PWM = 0;
      }
      break;
    case manual_mv:
      {
        PWM = AppConfig.PID[mmv];
      }
      break;
  }

  //////////////////////////////////////////////////////////////////////////////////////////////////////////
  if ((currentMillis - last_heartbeat_Millis) > 1000) //INTERVAL_1_HOUR)
  {
    String strDebug = DEBUG_PREAMBLE;
    webSocket.broadcastTXT(HEARTBEAT_MSG);
    last_heartbeat_Millis = currentMillis;// + 1000;// 60000;

    strDebug += loopTime;
    //strDebug += ":micros:";
    //strDebug += micros();
    webSocket.broadcastTXT(strDebug);


    /*
        AppConfig.PID[mmv]++;

        if(AppConfig.PID[mmv] > 255)
        {
          AppConfig.Mode = forced_off;
    	  buttonStates[heatmode][0] = AppConfig.Mode;
          //Serial.println("forced off");
          AppConfig.PID[mmv] = 0;
        }
    */
    //get_controller_ip();
    //    sendStatus();
  }
  if(updateHeaterStateHTML)
  {
    updateHeaterStateHTML = FALSE;
    broadcastHeaterState();
  }
    
}

void sendPIDData(void)
{
  char buffers [200];
 
  //   String buffers;
  //const char PIDformatString[]{ "KEY_PAIR_PREAMBLE"",ticks=%09d,PV=%06.2f,err=%06.2f,pterm=%06.2f,iterm=%06.2f,dterm=%06.2f,MV=%06.2f,hum=%06.2f,errHum=%06.2f,state=%s,clock=%09d,timer=%9d"};
#if TEMPERATURE_SENSOR_TYPE(DHT11) || TEMPERATURE_SENSOR_TYPE(DHT21) || TEMPERATURE_SENSOR_TYPE(DHT22)
  sprintf(buffers, PIDformatString, ticks, currentHeat, error, pTerm, iTerm, dTerm, MV, currentHumidity, humErr , strState[smHeater].c_str(), millis(), countdowntimer);//,loopTime); // / INTERVAL_1_SEC );
#else
  sprintf(buffers, PIDformatString, ticks, currentHeat, error, pTerm, iTerm, dTerm, MV, strState[smHeater].c_str(), millis(), countdowntimer);//,loopTime); // / INTERVAL_1_SEC );

#endif
  webSocket.broadcastTXT(buffers);
  DEBUG4_PRINTLN(buffers);
}

void sendButtonText(void)
{
  String strButtonTags = BUTTON_PREAMBLE;

  for (int i = 0; i < numOfHTMLButts; i++)
  {
    strButtonTags += ",";
    strButtonTags += strButtStates[i][buttonStates[i][0]];
  }
  webSocket.broadcastTXT(strButtonTags);
}

void copyProfile(void)
{
  for (int i = 0; i < sizeof(AppConfig.PID) / sizeof(AppConfig.PID[0]) ; i++)
  {
    AppConfig.PID[i] = profiles[buttonStates[0][0]][i];
  }
}

void sendControlParams(void)
{
  char buffers [200];
  char strFloat [6];
  int i = 0;

  strcpy(buffers, CONTROL_PARAMS_PREAMBLE);
  for (i = 0; i < sizeof(AppConfig.PID) / sizeof(AppConfig.PID[0]) ; i++)
  {
    sprintf(strFloat, ",%3.2f", AppConfig.PID[i]);
    strcat(buffers, strFloat);
  }

  sprintf(strFloat, ",%d", ptrSlider);
  strcat(buffers, strFloat);

  DEBUG4_PRINT(F("CONTROL PARAMS:"));
  DEBUG4_PRINTLN(buffers);
  webSocket.broadcastTXT(buffers);
}

void broadcastHeaterState(void)
{
  
  if (ISHEATERHIGH)
  {
    webSocket.broadcastTXT(HEATER_HIGH_MSG);
  }
  else
  {
    webSocket.broadcastTXT(HEATER_LOW_MSG);
  }
  if (ISHEATERON)
  {
    webSocket.broadcastTXT(HEATER_ON_MSG);
  }
  else
  {
    webSocket.broadcastTXT(HEATER_OFF_MSG);
  }
}

ICACHE_RAM_ATTR void endOfSpace()
{

  if (PWM == 0)
  {
    HEATER_OFF;
    updateHeaterStateHTML = TRUE;
///    webSocket.broadcastTXT(HEATER_OFF_MSG);
  }
  else
  {
    HEATER_ON;
    updateHeaterStateHTML = TRUE;
///    webSocket.broadcastTXT(HEATER_ON_MSG);
    if (PWM != 255)
    {
      endOfMarkTicker.attach_ms((uint32_t)(INTERVAL_250HZ_PERIOD * PWM), endOfMark);
      DEBUG1_PRINT("The interval in mS:");
      DEBUG1_PRINTLN(INTERVAL_250HZ_PERIOD * PWM);
    }
  }
///  broadcastHeaterState();
  DEBUG1_PRINTF("settings heap size: %u\n", ESP.getFreeHeap());
  DEBUG1_PRINTLN();
}

ICACHE_RAM_ATTR void endOfMark()
{
  endOfMarkTicker.detach();
  HEATER_OFF;
  updateHeaterStateHTML = TRUE;
///  webSocket.broadcastTXT(HEATER_OFF_MSG);
///  broadcastHeaterState();
}
