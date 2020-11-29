/*
   ToDo
Debugging random crashes 
Turned off OLED ROTARY HUMIDIFIER HTTPCLIENT,ESP8266mDNS

   Never block for more than ~ 1.5s or WDT

   RC522 MODULE    Uno/Nano     MEGA
  IRQ             N/A          N/A  BR
  NSS             D10          D53  WH/BR
  SCK             D13          D52  GR
  MOSI            D11          D51  WH/GR
  MISO            D12          D50  OR
  GND             GND          GND  WH/OR
  RST             D9           D5   BL
  3.3V            3.3V         3.3V WH/BL

  WEMOS
  IRQ             N/A WH/BL (Blue)
  NSS (SDA)       D8  WH/GR (Black)
  SCK             D5  GR  (Yellow)
  MOSI            D7  WH/OR (Red)
  MISO            D6  BLUE (Green)
  GND             GND OR   (Orange)
  RST             D0  WH/BR (Brown)
  3.3V            VCC BR  (Grey)

  H0 Blue  { 53, 5},
  H1 Pink  { 49, 6},
  H2 Silver  { 48, 7 },
  H3 Red    {47, 8}
  Test ok with Mega
  Tested OK with 6m of cat5

  //Normal run
  //Vcc 4.85v
  //D4 3.36V
  //D3 3.36v
  //D2 84mv
  //D1 84mV
  //RX 3.36V
  //TX 3.36V
  //Res 3.36V
  //A0 60mV
  //D0 84mV
  //D5 84mV
  //D6 84mV
  //D7 85mV
  //D8 85mV
  //3.3v 3.36V

	pins defined C:\data\arduino-1.8.9\portable\packages\esp8266\hardware\esp8266\2.5.2\variants\d1_mini
	need to select LOLIN(WEMOS)D1R2 & mini
  //Pins good for input D0,D1,D2,D5,D6,D7
  //D0 GPIO16 dont care
  //D0 while held in reset High

  //D1 GPIO05 Dont Care
  //D1 while held in reset Low  

  //D2 GPIO04 Dont Care
  //D2 while held in reset Low
  
  //D3 GPIO00 can not be low during a reset, but must be low during a reflash
  //D3 GPIO00 can be high during a reset but must be low during a reflash
  //D3 GPIO00 can be used with wemos relay shield
  //D3 pull up
  //D3 while held in reset High
  
  //D4 GPIO02 can not be low during a reset
  //D4 GPIO02 can be high during a reset and reflash
  //D4 GPIO02 cant be used with wemos relay shield. Will Not reset
  //D4 pull up
  //D4 wemos LED_BUILTIN HIGH is OFF
  //D4 while held in reset High

  //D5 GPIO14 Dont Care
  //D5 GPIO14 SPI Clock
  //D5 while held in reset High
  
  //D6 GPIO12 Dont Care
  //D6 GPIO12 SPI MISO
  //D6 while held in reset High
  
  //D7 GPIO13 (Serial.swap() -> RX) Dont Care
  //D7 GPIO13 SPI MOSI
  //D7 while held in reset High
  
  //D8 GPIO15 (Serial.swap() -> TX) can not be high during a reset or a reflash
  //D8 GPIO15 (Serial.swap() -> TX) can be low during a reset and reflash
  //D8 pull down
  //D8 while held in reset Low
  
  //TX GPIO01

  //RX GPIO03
  https://www.youtube.com/watch?v=vudfIq6PQMw //FX-951 reverse  engineering
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
//#include <ESP8266mDNS.h>
//#include <ESP8266HTTPClient.h>//Used to send heartbeats
//#include <WiFiUdp.h>
#include <ArduinoOTA.h> //Update Flash Over The Air
#include <EEPROM.h>     //Saves various settings 
#include "ESPRotary.h" //https://github.com/LennartHennigs/ESPRotary
//#include <Wire.h>
#include <Ticker.h>
//#include "OLED.h" //klarsys github lib
#include "SSD1306Wire.h"  //ThingPulse, Daniel Eichhorn  2018 by Fabrice Weinberg
#include "logo.h"

/* definition to expand macro then apply to pragma message */
#define VALUE_TO_STRING(x) #x
#define VALUE(x) VALUE_TO_STRING(x)
#define VAR_NAME_VALUE(var) #var "="  VALUE(var)
//usage: #pragma message(VAR_NAME_VALUE(USE_DHT))

#define HOSTNAME "solder02"

#define TEMPERATURE_SENSOR_TYPE(X) TEMPERATURE_SENSOR_TYPE_PRIVATE_DEFINITION_##X()
#define TEMPERATURE_SENSOR_TYPE_PRIVATE_DEFINITION_MAX6675() 0  //Used with Thermocouple
#define TEMPERATURE_SENSOR_TYPE_PRIVATE_DEFINITION_DHT11() 0	//DHT11
#define TEMPERATURE_SENSOR_TYPE_PRIVATE_DEFINITION_DHT21() 0	// DHT 21 (AM2301)
#define TEMPERATURE_SENSOR_TYPE_PRIVATE_DEFINITION_DHT22() 0	// DHT 22  (AM2302), AM2321
#define TEMPERATURE_SENSOR_TYPE_PRIVATE_DEFINITION_ADC_THERMISTOR() 0//10k Thermistor Steinhart coefficients in pid.ino
#define TEMPERATURE_SENSOR_TYPE_PRIVATE_DEFINITION_ADC_4DIODES() 0 //Uses Vf The forward Volt Drop of 4 diodes  coef in conf.ino
#define TEMPERATURE_SENSOR_TYPE_PRIVATE_DEFINITION_ADC_T12() 1 //HAKKO T12 thermocouple tip with GL328 opamp 
//test example use - #if TEMPERATURE_SENSOR_TYPE(MAX6675) ... #endif

#define ESP8266_PID_WITH_FEATURE(X) ESP8266_PID_WITH_FEATURE_PRIVATE_DEFINITION_##X()
#define ESP8266_PID_WITH_FEATURE_PRIVATE_DEFINITION_SSD1306() 1 //OLED Display
#define ESP8266_PID_WITH_FEATURE_PRIVATE_DEFINITION_ROTARY_ENCODER() 1 //Rotary Encoder with button, requires 3 pins.
#define ESP8266_PID_WITH_FEATURE_PRIVATE_DEFINITION_HUMIDIFIER() 1 //Water Injection system to increase humidty.
#define ESP8266_PID_WITH_FEATURE_PRIVATE_DEFINITION_mDNS() 0 //
// test example ise - #if ESP8266_PID_WITH_FEATURE(HUMIDIFIER) ... #endif

//Define I/O based on features selected above
#if TEMPERATURE_SENSOR_TYPE(ADC_4DIODES) || TEMPERATURE_SENSOR_TYPE(ADC_THERMISTOR) || TEMPERATURE_SENSOR_TYPE(ADC_T12)
#define sensorPin A0
#endif

#define HEATER_PIN D0  	//turns element on/off

#if ESP8266_PID_WITH_FEATURE(SSD1306)
#define SCL D1 			//OLED display
#define SDA D2			//OLED display
#endif



#if ESP8266_PID_WITH_FEATURE(ROTARY_ENCODER)
#define ROTARY_BUTTON D3
#define encoder0Pin_B   D6 //ky-040 dt  pin,             add 100nF/0.1uF capacitors between pin & ground!!!
#define encoder0Pin_A   D7 //ky-040 clk pin, interrupt & add 100nF/0.1uF capacitors between pin & ground!!!
#endif
#define PUMP_PIN  D8

//#define ktcSO  D6  //12// 2
//#define ktcCS  D7  //13// 12
//#define ktcCLK D5  //14//  13 WAS D5

#define SerialSpeed 115200
#define INTERVAL_1_DAY 60 * 60 * 24 *1000 //86400000///24  // 1day => 24*60*60*1000
#define INTERVAL_1_HOUR 60 *60 * 1000
#define INTERVAL_1_MIN 60 * 1000
#define INTERVAL_1_SEC 1000
#define INTERVAL_HOLD_DOWN 100
#define INTERVAL_50HZ_PERIOD 20 //20mS
#define INTERVAL_1000HZ_PERIOD 1 
#define INTERVAL_250HZ_PERIOD 4 


#define INTERVAL_PUMP_CHECK INTERVAL_1_MIN * 10
#define DURATION_PUMP_ON INTERVAL_1_SEC * 5
#define HUMIDITY_ERROR_LIMIT 5

//float deadzone = 1.3;     //change profile when PV is +/- deadzone within current SP.
#define deadzone 1.3

#if ESP8266_PID_WITH_FEATURE(ROTARY_ENCODER)
#define NBR_BUTTS 1
#define LONGPRESSTIME  500
#define SHORTPRESSTIME  100
volatile byte  buttonPins[NBR_BUTTS] = { ROTARY_BUTTON};
volatile long buttonTimer[NBR_BUTTS] = { 0};

enum buttonStatus {
  waitForButtonPress,
  buttonDown,
  waitForLong,
  waitForButtonUp,
  delayafterlong,
};

volatile int smButtonState[NBR_BUTTS] = { waitForButtonPress};

#define BUTTONPRESSED !digitalRead(buttonPins[i])

boolean buttonActive[NBR_BUTTS] = { false/*,false,false,false*/};
boolean longPressActive[NBR_BUTTS] = { false/*,false,false,false*/};
#endif

#define HEATER_OFF_MSG "hOff"
#define HEATER_ON_MSG "hOn"

#define HEATER_HIGH_MSG "hHigh"
#define HEATER_LOW_MSG "hLow"

#define PUMP_OFF_MSG "pOff"
#define PUMP_ON_MSG "pOn"

#define PUMP_HIGH_MSG "pHigh"
#define PUMP_LOW_MSG "pLow"

#define HEARTBEAT_MSG "HBM"

#define HEATER_ON digitalWrite(HEATER_PIN, !buttonStates[heaterlogic][0]);\
  digitalWrite(LED_BUILTIN,LOW)
#define HEATER_OFF digitalWrite(HEATER_PIN, buttonStates[heaterlogic][0]);\
  digitalWrite(LED_BUILTIN,HIGH)
#define ISHEATERON digitalRead(HEATER_PIN) ^ buttonStates[heaterlogic][0]
#define ISHEATERHIGH digitalRead(HEATER_PIN)

#if ESP8266_PID_WITH_FEATURE(HUMIDIFIER)
#define PUMP_ON digitalWrite(PUMP_PIN, !buttonStates[pumplogic][0])
#define PUMP_OFF digitalWrite(PUMP_PIN, buttonStates[pumplogic][0])
#define ISPUMPON digitalRead(PUMP_PIN) ^ buttonStates[pumplogic][0]
#define ISPUMPHIGH digitalRead(PUMP_PIN)

#endif

#define degf(C)  ((C*9/5)+32)//(0°C × 9/5) + 32 = 32°F
 //reading = (sensorValue*0.426704) - 19.88317;
//#define T12ADCtoC(ADC) ((ADC*0.426704) - 19.88317)
//#define T12ADCtoC(ADC) ((ADC*T12ADCtoCCoeffA) + T12ADCtoCCoeffB)
//#define ADC_4DIODES2CELCIUS(adc) (adc * (-0.3024913737)) + 229.82687237
#define tostr(X) " (X+0) "

#define BUTTON_PREAMBLE "*"

#define RANGE_PREAMBLE "^"

#define STRING_PREAMBLE "@"

#define DEBUG_PREAMBLE "#"

#define KEY_PAIR_PREAMBLE "&"

#define CONTROL_PARAMS_PREAMBLE "~"

#define TRUE true
#define FALSE false

//#define DEBUGLEVEL 1
#define DEBUG0_PRINT(...)
#define DEBUG0_PRINTLN(...) //Least verbose most important
#define DEBUG0_PRINTF(...)
#define DEBUG1_PRINT(...)
#define DEBUG1_PRINTLN(...) //Least verbose most important
#define DEBUG1_PRINTF(...)
#define DEBUG2_PRINT(...)
#define DEBUG2_PRINTLN(...)
#define DEBUG2_PRINTF(...)
#define DEBUG3_PRINT(...)
#define DEBUG3_PRINTLN(...)
#define DEBUG3_PRINTF(...)
#define DEBUG4_PRINT(...)
#define DEBUG4_PRINTLN(...)
#define DEBUG4_PRINTF(...)
#define DEBUG5_PRINT(...)
#define DEBUG5_PRINTLN(...) //Most verbose least important
#define DEBUG5_PRINTF(...)

#ifdef DEBUGLEVEL
#define DEBUG0_PRINT(...) Serial.print(__VA_ARGS__)
#define DEBUG0_PRINTLN(...) Serial.println(__VA_ARGS__)
#define DEBUG0_PRINTF(...) Serial.printf(__VA_ARGS__)
#endif

#if DEBUGLEVEL > 0
#define DEBUG1_PRINT(...) Serial.print(__VA_ARGS__)
#define DEBUG1_PRINTLN(...) Serial.println(__VA_ARGS__)
#define DEBUG1_PRINTF(...) Serial.printf(__VA_ARGS__)
//#error
#endif
#if DEBUGLEVEL > 1
#define DEBUG2_PRINT(...) Serial.print(__VA_ARGS__)
#define DEBUG2_PRINTLN(...) Serial.println(__VA_ARGS__)
#define DEBUG2_PRINTF(...) Serial.printf(__VA_ARGS__)
#endif
#if DEBUGLEVEL > 2
#define DEBUG3_PRINT(...) Serial.print(__VA_ARGS__)
#define DEBUG3_PRINTLN(...) Serial.println(__VA_ARGS__)
#define DEBUG3_PRINTF(...) Serial.printf(__VA_ARGS__)
#endif
#if DEBUGLEVEL > 3
#define DEBUG4_PRINT(...) Serial.print(__VA_ARGS__)
#define DEBUG4_PRINTLN(...) Serial.println(__VA_ARGS__)
#define DEBUG4_PRINTF(...) Serial.printf(__VA_ARGS__)
#endif
#if DEBUGLEVEL > 4
#define DEBUG5_PRINT(...) Serial.print(__VA_ARGS__)
#define DEBUG5_PRINTLN(...) Serial.println(__VA_ARGS__)
#define DEBUG5_PRINTF(...) Serial.printf(__VA_ARGS__)
#endif


const char __SKETCH__[] = __FILE__; //Used to get the current path

#pragma message(VAR_NAME_VALUE(__FILE__))

const byte io_pins [9] =     {D0, D1, D2, D3, D4, D5, D6, D7, D8};
//const byte io_pins[9] =    {16,  5,  4,  0,  2, 14, 12, 13, 15};

int ticks = 0;  //Keeps track of how many times the PID loop has been executed.
bool updateSlidersReq = FALSE;
bool updatePIDDataReq = FALSE;
bool updateButtonsReq = FALSE;
bool updateHTML = FALSE;
volatile bool updateHeaterStateHTML = FALSE;


//The following info is included in the Build Info page
const String strSummary =  "<br><br>Summary<br>Generic Temperature Monitoring Controller <br>controller<br>By Justin Richards<br><br>";

unsigned long last_heartbeat_Millis = 0;  //Time to send a http request to webserver for remote logging if needed
unsigned long lastPIDmillis = 0;    //Keeps track of timming for PID loop execution
unsigned long lastDisplayUpdateMillis = 0;
unsigned long currentMillis = millis();

unsigned long lastPumpMillis = 0;//INTERVAL_PUMP_CHECK;
unsigned long holdDownTimer = 0;

unsigned long lastYieldMillis = 0;    //If probe is disconnected, ensures that utility functions Wifi etc are run periodically in the background
unsigned long lastRotaryMillis = 0;
static unsigned long countdowntimer = 0; //countdown timer for current heating profile

unsigned long smMillis = 0;

//PID variables
float error = 0;
float pTerm = 0;
float iTerm = 0;
float dTerm = 0;

float lastReading = 0;
float sensorValue = 0;
float currentHeat = 0;
float currentHumidity = 0;
float humErr = 0;

float MV;  //Control Variable
float PWM;

enum smHeater //state machine enumerated states
{
  standby, stage1vectoring, stage1sustain, stage2vectoring, stage2sustain, cyclecomplete//, complexProfile
};
const String strState[] {"Starting", "SP1» ≅", "SP1» ≡", "SP2» ≅", "SP2» ≡", "~Comp~", "Invalid"};

int smHeater = standby;

enum heatmodes
{
  run_pid, forced_on, forced_off, manual_mv, invalid
};

enum smHumidty //state machine enumerated states
{
  pStandby, pStart, pRun, pStop
};
int smHumidty = pStandby;

enum smPumpMode //state machine enumerated states
{
  pAuto, pForcedOn, pForcedOff
};
int smPumpMode = pAuto;

//const String outputHeading[] { "ticks", "PV" , "err" , "pterm" , "iterm" , "dterm" , "MV" ,  "hum" , "errHum" , "state" , "clock" , "timer"};
//const char PIDformatString[]{ "KEY_PAIR_PREAMBLE"",ticks=%09d,PV=%06.2f,err=%06.2f,pterm=%06.2f,iterm=%06.2f,dterm=%06.2f,MV=%06.2f,clock=%09d,hum=%06.2f,state=%s,timer=%9d,errHum=%06.2f"};
#if TEMPERATURE_SENSOR_TYPE(DHT11) || TEMPERATURE_SENSOR_TYPE(DHT21) || TEMPERATURE_SENSOR_TYPE(DHT22)
const char PIDformatString[] { KEY_PAIR_PREAMBLE",ticks=%09d,PV=%06.2f,err=%06.2f,pterm=%06.2f,iterm=%06.2f,dterm=%06.2f,MV=%06.2f,hum=%06.2f,errHum=%06.2f,state=%s,clock=%09d,timer=%09d"};//,LT=%09d"};
const char PIDHeadingToolTips[] {"Count of PID loop iterations,Process Variable aka Temperature,SP - PV,calculated pterm,calculated iterm,calculated dterm,"
  "Manipulated Variable\\nThe Output aka Control Variable,Humidity,"
  "Humidty SP - Humidity,Current machine state,Uptime,Count down timer for current state"
#else
const char PIDformatString[] { KEY_PAIR_PREAMBLE",ticks=%09d,PV=%06.2f,err=%06.2f,pterm=%06.2f,iterm=%06.2f,dterm=%06.2f,MV=%06.2f,state=%s,clock=%09d,timer=%09d"};//,LT=%09d"};
const char PIDHeadingToolTips[] {"Count of PID loop iterations,Process Variable aka Temperature,SP - PV,calculated pterm,calculated iterm,calculated dterm,"
  "Manipulated Variable\\nThe Output aka Control Variable,"
  "Current machine state,Uptime,Count down timer for current state"
#endif
};//,Loop execution Time"};


#if ((TEMPERATURE_SENSOR_TYPE(DHT11) || TEMPERATURE_SENSOR_TYPE(DHT21) || TEMPERATURE_SENSOR_TYPE(DHT22)) && ESP8266_PID_WITH_FEATURE(HUMIDIFIER))
enum enumBUTTON     {profile, scale,  heatmode, heaterlogic,  pumpmode, pumplogic,  savestate,  resetrestart, numOfHTMLButts };
#else
enum enumBUTTON     {profile, scale,  heatmode, heaterlogic,  savestate,  resetrestart, numOfHTMLButts };
#endif
const String butt0[]  {"Saved Profile", "Jerky Dryer", "Yogurt Maker", "Egg Incubate", "Egg SousVide", "Black Garlic", "Solder Reflow", "Solder Iron"};
const String butt1[]  {"Toggle Scale &#176;F", "Toggle Scale &#176;C"};
const String butt2[]  {"Run PID Loop", "Forced ON", "Forced OFF", "Manual MV"};
const String butt3[]  {"Active High", "Active Low"};
#if ((TEMPERATURE_SENSOR_TYPE(DHT11) || TEMPERATURE_SENSOR_TYPE(DHT21) || TEMPERATURE_SENSOR_TYPE(DHT22)) && ESP8266_PID_WITH_FEATURE(HUMIDIFIER))
const String butt4[]  {"Auto Humidty", "Inject Water",  "Water Inhibit"};
const String butt5[]  {"Active High", "Active Low"};
#endif
const String butt6[]  {"Save State"};
const String butt7[]  {"Reset Restart"};
//const String butt8[]  {"Recall Profile"};
const String *strButtStates[] {
  butt0,
  butt1,
  butt2,
  butt3,
#if ((TEMPERATURE_SENSOR_TYPE(DHT11) || TEMPERATURE_SENSOR_TYPE(DHT21) || TEMPERATURE_SENSOR_TYPE(DHT22)) && ESP8266_PID_WITH_FEATURE(HUMIDIFIER))
  butt4,
  butt5,
#endif
  butt6,
  butt7,
  //  butt8,
};
#define numOfButtonStatesButt0 sizeof(butt0)/sizeof(butt0[0]) - 1

byte buttonStates[numOfHTMLButts][2] = {
  /*{currentButtState, Permuatations*/
  {0,  sizeof(butt0)/sizeof(butt0[0]) - 1}, //0
  {0,  sizeof(butt1)/sizeof(butt1[0]) - 1}, //1
  {0,  sizeof(butt2)/sizeof(butt2[0]) - 1}, //2
  {0,  sizeof(butt3)/sizeof(butt3[0]) - 1}, //3
#if ((TEMPERATURE_SENSOR_TYPE(DHT11) || TEMPERATURE_SENSOR_TYPE(DHT21) || TEMPERATURE_SENSOR_TYPE(DHT22)) && ESP8266_PID_WITH_FEATURE(HUMIDIFIER))
  {0,  sizeof(butt4)/sizeof(butt4[0]) - 1}, //4
  {0,  sizeof(butt5)/sizeof(butt5[0]) - 1}, //5
#endif
  {0,  sizeof(butt6)/sizeof(butt6[0]) - 1}, //6
  {0,  sizeof(butt7)/sizeof(butt7[0]) - 1}, //7
  //  {0, 1}
};

/*
  {0, 7}, //0
  {0, 2}, //1
  {0, 4}, //2
  {0, 2}, //3
#if ((TEMPERATURE_SENSOR_TYPE(DHT11) || TEMPERATURE_SENSOR_TYPE(DHT21) || TEMPERATURE_SENSOR_TYPE(DHT22)) && ESP8266_PID_WITH_FEATURE(HUMIDIFIER))
  {0, 3}, //4
  {0, 2}, //5
#endif
  {0, 1}, //6
  {0, 1}, //7
  //  {0, 1}
};
*/
/*
#else
enum enumBUTTON     {profile, scale,  heatmode, heaterlogic,  savestate,  resetrestart, numOfHTMLButts };
const String butt0[]  {"Saved Profile", "Jerky Dryer", "Yogurt Maker", "Egg Incubate", "Egg SousVide", "Black Garlic", "Solder Reflow", "Solder Iron"};
const String butt1[]  {"Toggle Scale &#176;F", "Toggle Scale &#176;C"};
const String butt2[]  {"Run PID Loop", "Forced ON", "Forced OFF", "Manual MV"};
const String butt3[]  {"Active High", "Active Low"};
const String butt4[]  {"Save State"};
const String butt5[]  {"Reset Restart"};
//const String butt8[]  {"Recall Profile"};
const String *strButtStates[] {
  butt0,
  butt1,
  butt2,
  butt3,
  butt4,
  butt5,
  //  butt8,
};
byte buttonStates[numOfHTMLButts][2] = {
  ///*{currentButtState, Permuatations
  {0, 7},
  {0, 2},
  {2, 4},
  {0, 2},
  {0, 1},
  {0, 1},
  //  {0, 1}
};
#endif

*/



/*
  this may be better option found at
  https://stackoverflow.com/questions/12509038/can-i-print-defines-given-their-values-in-c
  #define DEFINE_OP(n) { #n, n }

  static const struct {
  const char *name;
  int        value;
  } operators[] = {
  DEFINE_OP(ADD),
  DEFINE_OP(SUB),
  };
  This uses the stringifying preprocessor operator # to avoid repetitions.

  With the above, you can trivially write look-up code:

  const char * op_to_name(int op)
  {
  size_t i;

  for(i = 0; i < sizeof operators / sizeof *operators; ++i)
    if(operators[i].value == op)
      return operators[i].name;
  return NULL;
  }

*/
//enum used as index into Range Slider arrays
#if TEMPERATURE_SENSOR_TYPE(DHT11) || TEMPERATURE_SENSOR_TYPE(DHT21) || TEMPERATURE_SENSOR_TYPE(DHT22)
enum PID                       { sp1  	, sp2   , sp1td   , sp2td   , kp     , ki     , kd     , mvmax	 , mvmin  	, cal     , tc      , mmv   , sphum  , minalert   , maxalert, cpsize};
const char    *cptag  [cpsize] {"sp1" 	, "sp2" , "sp1td" , "sp2td" , "kp"   , "ki"   , "kd"   , "mvmax" , "mvmin"  , "cal"   , "tc"    , "mmv" , "sphum", "minalert" , "maxalert"};
const char    *cpmax  [cpsize] {"440" 	, "440" , "2000"  , "2000"  , "30"   , "30"   , "30"   , "255"   , "255"    , "20"    , "86400" , "255" , "100"  , "440"      , "440"     };
const char    *cpmin  [cpsize] {"0"		, "0"   , "0"     , "0"     , "0"    , "0"    , "0"    , "0"     , "0"      , "-20"   , "0.1"   , "0"   , "0"    , "0"        , "0"       };
const char    *cpstep [cpsize] {"0.1" 	, "0.1" , "1"     , "1"     , "0.01" , "0.01" , "0.01" , "1"     , "1"      , "0.1"   , "0.1"   , "0.1" , "1"    , "0.1"      , "0.1"     };
const char	  *cpttip [cpsize] {"Set Point 1 DegC", "Set Point 2 DegC", "SP1 Time Dur mins\\n0=Indefinte", "SP2 Time Dur mins\\n0=Indefinte", "Proportional Term", "Integral Term", "Differential Term",
  "Max Manipulated Variable", "Min Manipulated Variable", "Calibration Offset", "Time Constant Secs", "Manually Set Manipulated Variable", "Humidity Set Point", "Local Browser Alert Min LeveL" , "Local Browser Alert Max LeveL"
};

const float  profiles [8][cpsize] {
  // sp1 , sp2   , sp1td   , sp2td   , kp      , ki    , kd      , mvmax  , mvmin    , cal   , tc      , mmv  , sphum , minalert  , maxalert,cpsize};
  {0     , 0     , 0       , 0       , 0       , 0     , 0       , 0      ,  0       , 0     , 1       , 0    ,  0    , 0         , 0},   //Saved EEPROM settings place holder
  {72.5  , 57.2  , 5       , 360     , 5       , 0.4   , 6       , 140    ,  0       , 0     , 5       , 0    ,  0    , 80        , 26},  //Jerky Dryer
  {82.5  , 44.5  , 10      , 240     , 5       , 0.4   , 6       , 100    ,  0       , 0     , 5       , 0    ,  0    , 90        , 40},  //Yogurt Maker
  {37.5  , 37.5  , 0       , 0       , 1       , 0.4   , 1       , 80     ,  0       , 0     , 5       , 0    ,  0    , 40        , 35},  //Egg Incubate
  {64    , 63    , 5       , 120     , 20      , 0.4   , 10      , 140    ,  0       , 0     , 5       , 0    ,  0    , 67        , 60},  //Egg SousVide
  {54    , 76    , 600     , 0       , 20      , 0.4   , 10      , 140    ,  0       , 0     , 5       , 0    ,  0    , 80        , 50},  //Black Garlic
  {150   , 248   , 150     , 30      , 2       , 0.4   , 1       , 180    ,  0       , 0     , 5       , 0    ,  0    , 260       , 160}, //Solder Reflow
  {320   , 200   , 120     , 0       , 5       , 0.4   , 6       , 120    ,  0       , 0     , 5       , 0    ,  0    , 350       , 150}, //Soldering Iron
};

#else
enum PID                       { sp1  	, sp2   , sp1td   , sp2td   , kp     , ki     , kd     , mvmax	 , mvmin  	, cal     , tc      , mmv   , minalert   , maxalert, cpsize};
const char    *cptag  [cpsize] {"sp1" 	, "sp2" , "sp1td" , "sp2td" , "kp"   , "ki"   , "kd"   , "mvmax" , "mvmin"  , "cal"   , "tc"    , "mmv" , "minalert" , "maxalert"};
const char    *cpmax  [cpsize] {"440" 	, "440" , "2000"  , "2000"  , "30"   , "30"   , "30"   , "255"   , "255"    , "20"    , "86400" , "255" , "440"      , "440"     };
const char    *cpmin  [cpsize] {"0"		, "0"   , "0"     , "0"     , "0"    , "0"    , "0"    , "0"     , "0"      , "-20"   , "0.1"   , "0"   , "0"        , "0"       };
const char    *cpstep [cpsize] {"0.1" 	, "0.1" , "1"     , "1"     , "0.01" , "0.01" , "0.01" , "1"     , "1"      , "0.1"   , "0.1"   , "0.1" , "0.1"      , "0.1"     };
const char	  *cpttip [cpsize] {"Set Point 1 DegC", "Set Point 2 DegC", "SP1 Time Dur mins\\n0=Indefinte", "SP2 Time Dur mins\\n0=Indefinte", "Proportional Term", "Integral Term", "Differential Term",
  "Max Manipulated Variable", "Min Manipulated Variable", "Calibration Offset", "Time Constant Secs", "Manually Set Manipulated Variable", "Local Browser Alert Min LeveL" , "Local Browser Alert Max LeveL"
};
const float  profiles [8][cpsize] {
  // sp1 , sp2   , sp1td   , sp2td   , kp      , ki    , kd      , mvmax  , mvmin    , cal   , tc      , mmv   , minalert  , maxalert,cpsize};
  {0     , 0     , 0       , 0       , 0       , 0     , 0       , 0      ,  0       , 0     , 1       , 0     , 0         , 0},   //Saved EEPROM settings place holder
  {72.5  , 57.2  , 5       , 360     , 5       , 0.4   , 6       , 140    ,  0       , 0     , 5       , 0     , 80        , 26},  //Jerky Dryer
  {82.5  , 44.5  , 10      , 240     , 5       , 0.4   , 6       , 100    ,  0       , 0     , 5       , 0     , 90        , 40},  //Yogurt Maker
  {37.5  , 37.5  , 0       , 0       , 1       , 0.4   , 1       , 80     ,  0       , 0     , 5       , 0     , 40        , 35},  //Egg Incubate
  {64    , 63    , 5       , 120     , 20      , 0.4   , 10      , 140    ,  0       , 0     , 5       , 0     , 67        , 60},  //Egg SousVide
  {54    , 76    , 600     , 0       , 20      , 0.4   , 10      , 140    ,  0       , 0     , 5       , 0     , 80        , 50},  //Black Garlic
  {150   , 248   , 150     , 30      , 2       , 0.4   , 1       , 180    ,  0       , 0     , 5       , 0     , 260       , 160}, //Solder Reflow
  {320   , 200   , 120     , 0       , 5       , 0.4   , 6       , 120    ,  0       , 0     , 5       , 0     , 350       , 150}, //Soldering Iron
};

#endif

//const uint8_t cpmap   [cpsize] { sp1   , sp2  , sp1td    , sp2td  , kp        , ki   , kd       , tc     , mvmax  , mvmin   , cal    , mmv  , sphum}; //allows re-ordering of sliders without having to edit the table
byte ptrSlider = 0;


ESP8266WebServer server(80);  //instantiate server object of type class ESP8266WebServer
//HTTPClient http;              //instantiate http object of type class HTTPClient



WebSocketsServer webSocket(81);

//OLED display(D4/*SDA*/, D5/*SCL*/);
//OLED display(SDA, SCL);

#if ESP8266_PID_WITH_FEATURE(SSD1306)
SSD1306Wire display(0x3c, SDA, SCL); //// ADDRESS, SDA, SCL, OLEDDISPLAY_GEOMETRY  -  Extra param required for 128x32 displays. ThingPulse, Daniel Eichhorn  2018 by Fabrice Weinberg
#endif

Ticker endOfSpaceTicker;
Ticker endOfMarkTicker;

#if ESP8266_PID_WITH_FEATURE(ROTARY_ENCODER)
//RotaryEncoder encoder(encoder0Pin_A, encoder0Pin_B, BUTTON);
//ESPRotary r = ESPRotary(encoder0Pin_A, encoder0Pin_B, MOVESPERCLICK);
ESPRotary r = ESPRotary(encoder0Pin_A, encoder0Pin_B, 4);    //encoder0Pin_A, encoder0Pin_B, MOVESPERCLICK
void ICACHE_RAM_ATTR encoderISR()                                //interrupt service routines need to be in ram
{
  noInterrupts();
  r.loop();//encoder.readAB();
  interrupts();
}
#endif
void blah() //Seems we need a dummy function in the sketch file for compiling
{
  return;
}
