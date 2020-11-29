
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length)
{
  DEBUG2_PRINTLN();
  DEBUG2_PRINT(F("clientid="));
  DEBUG2_PRINTLN(num);
  DEBUG2_PRINT (F("Type="));
  DEBUG2_PRINTLN (type);
  DEBUG2_PRINT(F("length="));
  DEBUG2_PRINT(length);

  switch (type)
  {
    case WStype_DISCONNECTED:
      {
        DEBUG2_PRINTF("[%u] Disconnected!\n", num);
      }
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        DEBUG2_PRINTF("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        DEBUG2_PRINTLN();

        // send message to client
        webSocket.sendTXT(num, "Connected");
        updateHTML = TRUE;
        holdDownTimer = millis() + INTERVAL_HOLD_DOWN;
      }
      break;

    case WStype_TEXT:
      {
        DEBUG2_PRINTF("[%u] get Text: %s\n", num, payload);
        DEBUG2_PRINTLN();
        String spayload = (char*)payload;
        DEBUG2_PRINTLN(spayload);

        if (payload[0] == BUTTON_PREAMBLE[0]) //we have a button eg "*idReset,button"
        {
          String id = spayload.substring(1, spayload.indexOf(','));//.toInt() ;

          String idButton = spayload.substring(spayload.length() - 2, spayload.length());
          int intID = idButton.toInt();
          DEBUG2_PRINT("idButton->");
          DEBUG2_PRINT(idButton);
          DEBUG2_PRINTLN("<-");

          DEBUG2_PRINT("INT id :");
          DEBUG2_PRINTLN(intID);

          buttonStates[intID][0]++;
          if (buttonStates[intID][0] == buttonStates[intID][1]) //index 1 holds the total number of possible states.  If we have rotated this far point back to the begining button option
          {
            buttonStates[intID][0] = 0;
          }

          DEBUG2_PRINT("Button:");
          DEBUG2_PRINTLN(strButtStates[intID][buttonStates[intID][0]]);

          updateButtonsReq = TRUE;

          switch (intID)  //Switch on HTML button ID.  Far left is obviously 0
          {
            case profile: 
              {
                if (buttonStates[intID][0] == 0) // Recall saved profile  I think this is a good idea.
                {
                  EEPROM.begin(900+512); delay(10);
                  DEBUG2_PRINTLN(F("Signature verified. Initialising Config From EEPROM"));
                  EEPROM.get(0, AppConfig);
                  EEPROM.end();
                  buttonStates[heaterlogic][0] = AppConfig.Flags.bIsHeaterOutputInverted ;
#if TEMPERATURE_SENSOR_TYPE(DHT11) || TEMPERATURE_SENSOR_TYPE(DHT21) || TEMPERATURE_SENSOR_TYPE(DHT22)
                  buttonStates[pumplogic][0] = AppConfig.Flags.bIsPumpOutputInverted ;
#endif
                  buttonStates[heatmode][0] = AppConfig.Mode;
                  buttonStates[scale][0] = AppConfig.Flags.bIsCelsius;
                }
                else
                {
                  copyProfile();
                }
                updateSlidersReq = TRUE;
              }
              break;
            case scale:
              {
                updateSlidersReq = TRUE; //forces a scale units change on SP1 and SP2
                updatePIDDataReq = TRUE;// sendPIDData();
              }
              break;
            case heatmode:
              {
                AppConfig.Mode = buttonStates[intID][0];
              }
              break;
            case heaterlogic:
              {
                AppConfig.Flags.bIsHeaterOutputInverted = buttonStates[intID][0];//!AppConfig.Flags.bIsHeaterOutputInverted;
              }
              break;
#if TEMPERATURE_SENSOR_TYPE(DHT11) || TEMPERATURE_SENSOR_TYPE(DHT21) || TEMPERATURE_SENSOR_TYPE(DHT22)
            case pumpmode:
              {
                smPumpMode = buttonStates[intID][0];
              }
              break;
            case pumplogic:
              {
                AppConfig.Flags.bIsPumpOutputInverted = buttonStates[intID][0];//!AppConfig.Flags.bIsPumpOutputInverted;
              }
              break;
#endif
            case savestate:
              {
                SaveAppConfig();
              }
              break;
            case resetrestart:
              {
                smHeater = standby ;
              }
              break;
            default:
              {
                Serial.println("Invalid profile");
              }
              break;
          }

        }

        if (payload[0] == RANGE_PREAMBLE[0])
        {
          String strId = (spayload.substring(spayload.indexOf('=') - 2, spayload.indexOf('=')));//.toInt();
          int intId = strId.toInt();
          float rangeValue = (spayload.substring(spayload.indexOf('=') + 1, spayload.length())).toFloat();
          DEBUG2_PRINT("We have a slider update:");
          DEBUG2_PRINT(cptag[intId]);
          DEBUG2_PRINTLN(rangeValue);
          AppConfig.PID[intId] = rangeValue;
          updateHTML = TRUE;
          holdDownTimer = millis() + INTERVAL_HOLD_DOWN;

          if (intId == tc) //Is this a time constant update
          {
            lastPIDmillis = millis();// + (AppConfig.PID[tc] * 1000);
          }
        }
      }
      break;
    default:
      {
          Serial.print("Invalid websocket connection type:");
          Serial.println(type);
      }
      break;
  }
}
