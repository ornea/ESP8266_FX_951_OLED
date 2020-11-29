/*
   PID.ino

*/

// resistance at 25 degrees C
#define THERMISTORNOMINAL 5000
// temp. for nominal resistance (almost always 25 C)
#define TEMPERATURENOMINAL 25
// The beta coefficient of the thermistor (usually 3000-4000)
#define BCOEFFICIENT 4216 //3950
// the value of the 'other' resistor
#define SERIESRESISTOR 5000

// What pin to connect the sensor to
#define THERMISTORPIN A0

#if TEMPERATURE_SENSOR_TYPE(DHT11)
#include "DHT.h"
#define DHTPIN D5
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
#endif
#if TEMPERATURE_SENSOR_TYPE(DHT21)
#include "DHT.h"
#define DHTPIN D5
#define DHTTYPE DHT21
DHT dht(DHTPIN, DHTTYPE);
#endif
#if TEMPERATURE_SENSOR_TYPE(DHT22)
#include "DHT.h"
#define DHTPIN D5
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
#endif

int totalfails;

#if TEMPERATURE_SENSOR_TYPE(MAX6675)
#define ktcSO  D6  //12// 2
#define ktcCS  D7  //13// 12
#define ktcCLK D5  //14//  13 WAS D5
#include <max6675.h>
MAX6675 ktc(ktcCLK, ktcCS, ktcSO);
#endif


//This function computes the new value for the ON time of the system
//This is the return value from this function
float computePID(float spPID)
{
  static float lastError = 0;
  float reading;// = analogRead(THERMISTORPIN);
  //static float lastReading = 0;
  static float output = 0;

  int NaNCount = 0;
  currentHeat = 0.0 / 0.0; //make it NaN
  while (isnan(currentHeat) && (NaNCount++ < 4))
  {
    doservers();
    //#ifdef USE_DHT
#if TEMPERATURE_SENSOR_TYPE(DHT11) || TEMPERATURE_SENSOR_TYPE(DHT21) || TEMPERATURE_SENSOR_TYPE(DHT22)
    dht.begin();

    currentHeat = dht.readTemperature() + AppConfig.PID[cal];
    ////Serial.print("DHT Temp:");
    ////Serial.println(dht.readTemperature());
    currentHumidity = dht.readHumidity();
    humErr = AppConfig.PID[sphum] - currentHumidity ;
    //    //Serial.print("HumErr:");
    //   //Serial.println(humErr);
#endif
#if TEMPERATURE_SENSOR_TYPE(ADC_4DIODES)// TEMPERATURE_SENSOR_TYPE_ADC_4DIODES
    sensorValue = analogRead(sensorPin);
    currentHeat = (sensorValue * AppConfig.ADCEqCoefA) + AppConfig.ADCEqCoefB;// ADC_4DIODES2CELCIUS(sensorValue);
#endif

#if TEMPERATURE_SENSOR_TYPE(ADC_THERMISTOR)
    sensorValue = analogRead(sensorPin);
    //currentHeat = (sensorValue * 0.0148) //volts conversion

    float reading;// = analogRead(THERMISTORPIN);

    //    //Serial.print("Analog reading ");
    //  //Serial.println(sensorValue);
    reading = ((5153 * 1000) / sensorValue) - 4951;

    float steinhart;
    steinhart = reading / THERMISTORNOMINAL;     // (R/Ro)
    steinhart = log(steinhart);                  // ln(R/Ro)
    steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
    steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
    steinhart = 1.0 / steinhart;                 // Invert
    steinhart -= 273.15;                         // convert to C

    currentHeat = steinhart;
#endif

#if TEMPERATURE_SENSOR_TYPE(ADC_T12)
    pinMode(HEATER_PIN, INPUT);
    sensorValue = analogRead(sensorPin);
    sensorValue = analogRead(sensorPin);
    delay(10);
    sensorValue = analogRead(sensorPin);
    pinMode(HEATER_PIN, OUTPUT);
    
    if (sensorValue == 1024 || (sensorValue  > (lastReading + 250)) || ((sensorValue + 250) < lastReading ))
    {
      if(lastReading != 0)
      {
        sensorValue = lastReading;
      }
    }

    lastReading = sensorValue;
    //Serial.print("ADC:");
    //Serial.println(sensorValue); 
   // sensorValue = runningFloatAverage(sensorValue);
    //Serial.print("ADC AVG:");
    //Serial.println(sensorValue); 

    //reading =T12ADCtoC(sensorValue);
    reading = ((sensorValue*AppConfig.T12ADCtoCCoeffA) + AppConfig.T12ADCtoCCoeffB);
    //reading = ((sensorValue*0.426704) - 19.88317);
    //reading = (sensorValue*0.426704) - 19.88317;
    currentHeat = reading;
#endif


#if TEMPERATURE_SENSOR_TYPE(MAX6675)//#ifdef USE_MAX6675
    currentHeat = ktc.readCelsius(); //readIronTemp(0, 1,spPID); //get the current temp of the iron
#endif
    if (isnan(currentHeat))
    {
      totalfails++;
      lastYieldMillis = millis();
      //Serial.println(NaNCount);
      //Serial.print(F("Read Attempt No:"));

      while ((millis() - lastYieldMillis) < 1000)
      {
        doservers();
      }
    }
  }

  if (isnan(currentHeat))// || isnan(currentHumidity) )
  {
    //Serial.println(F("Suspect prope is disconnected - all set to zero"));
    iTerm = 0;
    lastError = 0;
    return 0;
  }
  else
  {
    //add cal offset
    currentHeat += AppConfig.PID[cal];

    //Calc the error
    error =  spPID - currentHeat; //calculate the error term
    pTerm = AppConfig.PID[kp] * error;

    //Calc Intergration minmise windup
    if(output !=  AppConfig.PID[mvmax])
    {
      iTerm += (error * AppConfig.PID[ki]);
    }
    //Windup limiting
    if (iTerm > AppConfig.PID[mvmax])
      iTerm = AppConfig.PID[mvmax];
    else if (iTerm < 0)
      iTerm = 0; //cap at 0 since we cant force the iron to cool itself :)

    //Calc Differential
    dTerm = (error - lastError) * AppConfig.PID[kd];

    //Summation
    output = pTerm  + iTerm + dTerm;
    if (output < AppConfig.PID[mvmin])
    {
      output = AppConfig.PID[mvmin];
    }

    if (output > AppConfig.PID[mvmax])
    {
      output = AppConfig.PID[mvmax];
      //I do the following other wise dTerm has no effect when iTerm has woundup or pTerm is maxed out and I think it should.
      //it causes the MV to back off a little as PV approaches the SP particularly after a system upset i.e startup
      //      if (dTerm < 0)
      //        output += dTerm;
    }
    lastError = error;
    if (error < 0)
    {
      // output = 0;  //option to cut power to help speed up cooling on overshoot
    }
    /*
      For Debug below
      //Serial.println();
      //Serial.print(F("Heat °C: "));
      //Serial.print(currentHeat);
      //Serial.print(F(" \t"));

      //Serial.print(F("Humidity: "));
      //Serial.print(currentHumidity);
      //Serial.print(F(" %\t"));

      //Serial.print(F("ERROR: "));
      //Serial.print(error);
      //Serial.print(F(" \t"));

      //Serial.print(F("pTerm: "));
      //Serial.print(pTerm);
      //Serial.print(F(" \t"));

      //Serial.print(F("ITerm: "));
      //Serial.print(iTerm);
      //Serial.print(F(" \t"));

      //Serial.print(F("dTerm: "));
      //Serial.print(dTerm);
      //Serial.print(F(" \t"));

      //Serial.print(F("PID: "));
      //Serial.print(output);
      //Serial.print(F(" \t"));
      //Serial.print(F("Millis: "));
      //Serial.print(millis());
    */
    return output;
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
void smHeat(void)
{

  lastPIDmillis = currentMillis;// + (AppConfig.PID[tc] * 1000);
  ticks += 1; //increment PID loop execution counter

  switch (smHeater) //Switch on statemachine variable
  {
    case standby: //Initial state, zero all PID variables
      {
        MV = 0;
        //AppConfig.PID[mmv] = 0;
        pTerm = 0;
        iTerm = 0;
        dTerm = 0;
        smHeater++;
        countdowntimer = AppConfig.PID[sp1td] * INTERVAL_1_MIN;

      }
      break;
    case stage1vectoring:   //Target SP1 then transition state when PV within +/- deadzone of SP1
      {
        countdowntimer = AppConfig.PID[sp1td] * INTERVAL_1_MIN;
        MV = computePID(AppConfig.PID[sp1]);
        if ( (AppConfig.PID[sp1] < (currentHeat + deadzone)) && (AppConfig.PID[sp1] > (currentHeat - deadzone)))
        {
          smMillis = currentMillis;
          smHeater++;
        }
      }
      break;
    case stage1sustain: //Maintain SP1 for duration set by sp1td
      { countdowntimer = AppConfig.PID[sp1td] * INTERVAL_1_MIN;

        MV = computePID(AppConfig.PID[sp1]);
        if (AppConfig.PID[sp1td] != 0) //if 0 stay here for ever
        {
          countdowntimer = (AppConfig.PID[sp1td] * INTERVAL_1_MIN) - (currentMillis - smMillis);
          if ((currentMillis - smMillis) > AppConfig.PID[sp1td]*INTERVAL_1_MIN)
          {
            smHeater++;
          }
        }
      }
      break;
    case stage2vectoring: //Target SP2 then transition state when PV within +/- deadzone of SP2
      {
        countdowntimer = AppConfig.PID[sp2td] * INTERVAL_1_MIN;
        MV = computePID(AppConfig.PID[sp2]);
        if ( (AppConfig.PID[sp2] < (currentHeat + deadzone)) && (AppConfig.PID[sp2] > (currentHeat - deadzone)))
        {
          smMillis = currentMillis;
          smHeater++;
        }
      }
      break;
    case stage2sustain: //Maintain SP1 for duration set by sp2td
      {
        countdowntimer = (AppConfig.PID[sp2td] * INTERVAL_1_MIN);
        MV = computePID(AppConfig.PID[sp2]);
        if (AppConfig.PID[sp2td] != 0) //if 0 stay here for ever
        {
          countdowntimer = (AppConfig.PID[sp2td] * INTERVAL_1_MIN) - (currentMillis - smMillis);
          if ((currentMillis - smMillis) > AppConfig.PID[sp2td]*INTERVAL_1_MIN)
          {
            smHeater++;
          }
        }
      }
      break;
    case cyclecomplete: //turn off output and wait until someone presses Reset/Restart
      {
        countdowntimer = 0;
        float tempMV = computePID(AppConfig.PID[sp1]); //will still want to do readings
        pTerm = 0;
        iTerm = 0;
        dTerm = 0;
        MV = 0;
      }
      break;
/*	case complexProfile:
	  {
		//get next profile
		MV = computePID(complexProfileData[setPointIndex++]);
		sp1 and sp2 = complexProfileData[setPointIndex++];
AppConfig.PID[sp2] =
		if(setPointIndex == sizeof(complexProfileData))
		{
			smHeat = cyclecomplete;
		]
	call update ControlParams
	  }
	  break:
*/
    default: //should never be here
      {
        DEBUG4_PRINT(F("Invalid StateMachine State"));
      }
      break;
  }
}
