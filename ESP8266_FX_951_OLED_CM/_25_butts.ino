#if ESP8266_PID_WITH_FEATURE(ROTARY_ENCODER)

//Functions called to process button press interrupts
void ICACHE_RAM_ATTR sw0()
{
  DEBUG3_PRINTLN(millis());
  smButtons(0);
}
void ICACHE_RAM_ATTR sw1()
{
  //DEBUG3_PRINTLN(millis());
  smButtons(1);
}
void ICACHE_RAM_ATTR sw2()
{
  //DEBUG3_PRINTLN(millis());
  smButtons(2);
}
void ICACHE_RAM_ATTR sw3()
{
  //DEBUG3_PRINTLN(millis());
  smButtons(3);
}
// create array of pointers to functions
void ICACHE_RAM_ATTR (*switchFunctions[])()
{
  sw0,
  sw1,
  sw2,
  sw3,
};


void smButtons(int i)
{
  switch (smButtonState[i])
  {
    case waitForButtonPress:
      {
        //     DEBUG3_PRINTLN("there");
        if (BUTTONPRESSED)
        {
          smButtonState[i]++;
          buttonTimer[i] = millis() + SHORTPRESSTIME;
          DEBUG5_PRINTLN(millis());
          DEBUG5_PRINT("Button pressed:");
          DEBUG5_PRINTLN(i);
        }
      }
      break;
    case buttonDown:
      {
        if (BUTTONPRESSED)
        {
          if (millis() > buttonTimer[i])
          { //button is down and short press timer has timed out so check for long press
            buttonTimer[i] = millis() + LONGPRESSTIME;
            smButtonState[i]++;
            DEBUG5_PRINTLN(millis());
            DEBUG5_PRINTLN("This may be a long press");
          }
        }
        else
        {
          if (millis() > buttonTimer[i])
          { //the button is up but the short press has timed out so considered a valid short press
            smButtonState[i] = waitForButtonPress;
            doShortPressAction(i);
          }

          else
          { //short press not yet timed out but button is up so not a valid press
            DEBUG5_PRINTLN(millis());
            DEBUG5_PRINT("Not long enough for short press for press:");
            DEBUG5_PRINTLN(i);
            buttonTimer[i] = 0;
            smButtonState[i] = waitForButtonPress;
          }
        }
      }
      break;
    case waitForLong:
      {
        if (BUTTONPRESSED)
        {
          if (millis() > buttonTimer[i])
          {

            smButtonState[i] = waitForButtonUp;
            doLongPressAction(i);
          }
        }
        else
        {
          smButtonState[i] = waitForButtonPress;
          doShortPressAction(i);
        }
      }
      break;
    case waitForButtonUp:
      { //we have done the long press action already. Just waiting for button up before continue to eliminate re-fires
        if (!BUTTONPRESSED)
        {
          smButtonState[i] = waitForButtonPress;
        }
      }
      break;
    default:
      {
        DEBUG5_PRINTLN(F("Invalid StateMachine State"));
        smButtonState[i] = waitForButtonPress;
      }
      break;
  }
}

void doShortPressAction(int i)
{
  buttonTimer[i] = 0;
  DEBUG5_PRINTLN(millis());
  DEBUG5_PRINT("Do Short Press Thing for button");
  DEBUG3_PRINTLN(i);
  ptrSlider++;
  if (ptrSlider == cpsize)
  {
    ptrSlider = sp1;
  }
  updateSlidersReq = TRUE;
}

void doLongPressAction(int i)
{
  buttonTimer[i] = 0;
  DEBUG5_PRINTLN(millis());
  DEBUG5_PRINT("Do Long Press Thing for button:");
  DEBUG5_PRINTLN(i);
  DEBUG5_PRINTLN("Saving EEPROM");
  SaveAppConfig();
}

void procLeftRight(ESPRotary& r)
{
  unsigned long deltaT = millis() -  lastRotaryMillis;
  float stepSize = String(cpstep[ptrSlider]).toFloat();
  lastRotaryMillis = millis();

  if (deltaT < 10)
    deltaT = 10;

  if (deltaT > 500)
  {
    deltaT = 500;
  }
  else
  {
    stepSize *= 4000 / ((float)deltaT);
  }
  {

    if (r.getDirection() == RE_RIGHT)
    {
      AppConfig.PID[ptrSlider] -= stepSize;
    }
    else
    {
      AppConfig.PID[ptrSlider] += stepSize;
    }

    if (AppConfig.PID[ptrSlider] < String(cpmin[ptrSlider]).toFloat()) AppConfig.PID[ptrSlider] = String(cpmin[ptrSlider]).toFloat();
    if (AppConfig.PID[ptrSlider] > String(cpmax[ptrSlider]).toFloat()) AppConfig.PID[ptrSlider] = String(cpmax[ptrSlider]).toFloat();
    updateSlidersReq = 1;
  }
}
#endif
