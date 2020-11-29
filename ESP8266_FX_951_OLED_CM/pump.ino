#if ((TEMPERATURE_SENSOR_TYPE(DHT11) || TEMPERATURE_SENSOR_TYPE(DHT21) || TEMPERATURE_SENSOR_TYPE(DHT22)) && ESP8266_PID_WITH_FEATURE(HUMIDIFIER))
void smPump(void)
{
  switch (smPumpMode)
  {
    case pAuto:
      {
        autoHumidity();
      }
      break;
    case pForcedOn:
      {
        if (!ISPUMPON)
        {
          PUMP_ON;
          webSocket.broadcastTXT(PUMP_ON_MSG);
          broadcastPumpState();
        }
      }
      break;
    case pForcedOff:
      {
        if (ISPUMPON)
        {
          PUMP_OFF;
          webSocket.broadcastTXT(PUMP_OFF_MSG);
          broadcastPumpState();
        }
      }
      break;
    default:
      {
        DEBUG0_PRINTLN(F("Invalid StateMachine State"));
        smPumpMode = pForcedOff;
      }
      break;

  }
}

void autoHumidity(void)
{
  unsigned long currentMillis = millis();
  switch (smHumidty)
  {
    case pStandby:
      {
        PUMP_OFF;
        if ((currentMillis - lastPumpMillis) > INTERVAL_PUMP_CHECK)
        {
          if (humErr > HUMIDITY_ERROR_LIMIT)
          {
            smHumidty++;
          }
        }
      }
      break;
    case pStart:
      {
        PUMP_ON;
        broadcastPumpState();
        lastPumpMillis = currentMillis;// + DURATION_PUMP_ON;
        smHumidty++;
      }
      break;
    case pRun:
      {
        if ((currentMillis - lastPumpMillis) > INTERVAL_PUMP_CHECK)
        {
          PUMP_OFF;
          broadcastPumpState();
          smHumidty++;
        }
      }
      break;
    case pStop:
      {
        PUMP_OFF;
        broadcastPumpState();
        lastPumpMillis = currentMillis;// + INTERVAL_PUMP_CHECK;
        smHumidty = pStandby;
      }
      break;
    default:
      {
        PUMP_OFF;
        broadcastPumpState();
        lastPumpMillis = currentMillis;// + INTERVAL_PUMP_CHECK;
        smHumidty = pStandby;
      }
      break;
  }
}

void broadcastPumpState (void)
{
  if (ISPUMPHIGH)
  {
    webSocket.broadcastTXT(PUMP_HIGH_MSG);
  }
  else
  {
    webSocket.broadcastTXT(PUMP_LOW_MSG);
  }

  if (ISPUMPON)
  {
    webSocket.broadcastTXT(PUMP_ON_MSG);
  }
  else
  {
    webSocket.broadcastTXT(PUMP_OFF_MSG);
  }
}
#endif
