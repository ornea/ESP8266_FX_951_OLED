struct APP_CONFIG //AppConfig
{
  //This is a signature that is used to determine if EEPROM is to be updated with the hardcoded
  //values or continue using previously saved values. It must be at the start of the struct

  char E_sign[16] = "hackerbbccd0so8";

#define indx_SSID 0
#define indx_KEY 1
#define numAccessPoints 5
#define pragMSG "Credentials File Found"

#ifdef CREDENTIALS
#pragma message(VAR_NAME_VALUE(pragMSG))
#else
#pragma message(VAR_NAME_VALUE(numAccessPoints))
#define SSIDKEY1 {"ssid1", "key1"}
#define SSIDKEY2 {"ssid2", "key2"}
#define SSIDKEY3 {"ssid3", "key3"}
#define SSIDKEY4 {"ssid4", "key4"}
#define SSIDKEY5 {"ssid5", "key5"}
#define URL_Heartbeat "http://yourhtmlserver/cgi-bin/heartbeat.pl?"

#warning Credentials file not included or was included after this file
#endif
#pragma message(VAR_NAME_VALUE(strSummary))
  //The SSID/KEY pairs are derived from the _00_creds.ino file if it exists.  If not the pairs come from the defines above.
  char access_point[numAccessPoints][2][20] = //this is not inline with the rules for size of ssid and key
  {
    SSIDKEY1,
    SSIDKEY2,
    SSIDKEY3,
    SSIDKEY4,
    SSIDKEY5,
  };
#if ((TEMPERATURE_SENSOR_TYPE(DHT11) || TEMPERATURE_SENSOR_TYPE(DHT21) || TEMPERATURE_SENSOR_TYPE(DHT22)) && ESP8266_PID_WITH_FEATURE(HUMIDIFIER))
  //                   sp1  , sp2   , sp1td, sp2td,  kp , ki    , kd  , mvmax , mvmin, cal,  tc (timeconstant),mmv,sphum, minalert, maxalert
  float PID[cpsize] = {22  , 23   , 4    , 360  , 0.5 , 0.05  , 0.05, 140.0 ,   0  ,  0 ,             6     , 0 , 85  , 0       , 0}; //each PID elem has a slider and input box
  //was 15
#else
  //                   sp1  , sp2   , sp1td, sp2td,  kp , ki    , kd  , mvmax , mvmin, cal,  tc (timeconstant),mmv, minalert, maxalert
  float PID[cpsize] = {22  , 23   , 4    , 360  , 0.5 , 0.05  , 0.05, 140.0 ,   0  ,  0 ,             6     , 0 , 0       , 0}; //each PID elem has a slider and input box
  //was 15

#endif
  //Default Configs as EP End Point

  char EpHostname[15] = HOSTNAME;// "solder01"; //End Point Hostname
  char EpControlHostname[15] = HOSTNAME"_cont";//"solder01_cont"; //If we need to send UDP messages to another controller. Not used here

  //the requirement to statically seems to have been mitigated by WiFi.persistent(false); in _init.h
  uint8_t EpIP[4] = {172, 16, 10, 220}; //I have issues when a Cisco Router issues the IP address.
  uint8_t EpMASK[4] = {255, 255, 0, 0}; //So when in that environment I statically assign
  uint8_t EpGW[4] = {172, 16, 0, 1};
  uint8_t EpDNS[4] = {172, 16, 0, 1};

  //Default Configs as AP Access Point so it can be configured for a different network
  char ApSSID[32] = HOSTNAME"_AP";//"solder01AP"; //Max 32 Charshuh
  char ApKey[64];
  uint8_t ApIP[4] = {192, 168, 12, 1};

  uint8_t ApMASK[4] = {255, 255, 255, 0};
  uint8_t ApGW[4] = {192, 168, 12, 1};

  uint8_t Mode = run_pid;
  float ADCEqCoefA = -0.414494862;// /*-0.414494862 good cal with 4 diodes*/;//-0.3024913737;; //-0.37547/*4diodes*/;//-1.2935;//-0.38333;//-0.3838383838383838;//-0.3024913737; //Measured using 4 diodes and 1k resistor to 3.3V
  float ADCEqCoefB = 299.0630241058;// /*299.0630241058 good cal with 4 diodes*/;//223.826872;//277.29797/*4diodes*/;//895.7121;//282.7833;//283.0656565656566;//229.82687237;
#if TEMPERATURE_SENSOR_TYPE(ADC_T12)
  float T12ADCtoCCoeffA = 0.426704;
  float T12ADCtoCCoeffB = -19.88317;
#endif
  struct FLAGS

  {
    unsigned char bIsPumpOutputInverted: 1; //b7
    unsigned char bIsCelsius: 1;
    unsigned char bIsHeaterOutputInverted : 1; //Reverses the logic
    unsigned char xbIsModeAuto : 1; //'1' not used  Normal mode
    unsigned char xbIsOutForcedHigh : 1; //'1' not used forces Output HIGH
    unsigned char bUseStaticIP : 1; //When Cisco Router is doing DHCP
    unsigned char bIsDHCPEnabled : 1; //Not used
    unsigned char bInConfigMode : 1; //Not used
    //  } Flags = { 0, 1, 0, 1, 0, 1, 0, 0};//Leftmost is b7 use static
  } Flags = { 0, 1, 0, 1, 0, 0, 0, 0};//Leftmost is b7 use dhcp
} ;

APP_CONFIG AppConfig;
void saveProfile()
{
  EEPROM.begin(900 + 512);
  delay(10);
  unsigned char k = 0;
  for (int i = 0; i < 900; ++i)
  {

    EEPROM.put(512 + i, k++);
    //k++;
    //if (k > 255)
    //  k = 0;
  }
  EEPROM.end();
  DEBUG3_PRINTLN("Saving EEPROM");
}


void printProfile(void)
{
  unsigned char k;
  EEPROM.begin(900 + 512); delay(10);
  for (int i = 0; i < 900; ++i)
  {

    k = (EEPROM.read(512 + i));
    DEBUG1_PRINT(F("Index: ")); DEBUG1_PRINT(i); DEBUG1_PRINT(F("Char: ")); DEBUG1_PRINT(k); DEBUG1_PRINTLN(char(k));
  }
 // EEPROM.end();
}
void InitAppConfig()
{
  char s[16] = "";
  DEBUG1_PRINTLN(F("Reading EEPROM signature"));
  EEPROM.begin(900 + 512); delay(10);
  for (int i = 0; i < sizeof(AppConfig.E_sign); ++i)
  {
    s[i] += char(EEPROM.read(i));
  }
  EEPROM.end();
  DEBUG1_PRINT(F("Signature read: "));
  DEBUG1_PRINTLN(s);

  if (strcmp(AppConfig.E_sign, s)  == 0)  //if(AppConfig.E_sign == s)
  {
    EEPROM.begin(900 + 512); delay(10);
    DEBUG1_PRINTLN(F("Signature verified. Initialising Config From EEPROM"));
    EEPROM.get(0, AppConfig);
    EEPROM.end();
  }
  else
  {
    DEBUG1_PRINTLN(F("Signature not verified. Initialising EEPROM "));
    SaveAppConfig();
  }
  buttonStates[heaterlogic][0] = AppConfig.Flags.bIsHeaterOutputInverted ;
#if TEMPERATURE_SENSOR_TYPE(DHT11) || TEMPERATURE_SENSOR_TYPE(DHT21) || TEMPERATURE_SENSOR_TYPE(DHT22)
  buttonStates[pumplogic][0] = AppConfig.Flags.bIsPumpOutputInverted ;
#endif
  buttonStates[heatmode][0] = AppConfig.Mode;
  buttonStates[scale][0] = AppConfig.Flags.bIsCelsius;
}

void SaveAppConfig()
{
  EEPROM.begin(900 + 512);
  delay(10);

  EEPROM.put(0, AppConfig);
  EEPROM.end();
  DEBUG3_PRINTLN("Saving EEPROM");
}

void OutAppConfig()
{
  DEBUG1_PRINTLN(AppConfig.E_sign);

  DEBUG1_PRINT(F("End Point Hostname:"));
  DEBUG1_PRINTLN(AppConfig.EpHostname);

  DEBUG1_PRINT(F("End Point Control Hostname:"));
  DEBUG1_PRINTLN(AppConfig.EpControlHostname);

  DEBUG1_PRINT(F("Ep SSID0: "));
  DEBUG1_PRINTLN(AppConfig.access_point[0][indx_SSID]);

  DEBUG1_PRINT(F("Ep Key0: "));
  DEBUG1_PRINTLN(AppConfig.access_point[0][indx_KEY]);


  DEBUG1_PRINT(F("Ep SSID1: "));
  DEBUG1_PRINTLN(AppConfig.access_point[1][indx_SSID]);

  DEBUG1_PRINT(F("Ep Key1: "));
  DEBUG1_PRINTLN(AppConfig.access_point[1][indx_KEY]);


  DEBUG1_PRINT(F("Ep SSID2: "));
  DEBUG1_PRINTLN(AppConfig.access_point[2][indx_SSID]);

  DEBUG1_PRINT(F("Ep Key2: "));
  DEBUG1_PRINTLN(AppConfig.access_point[2][indx_KEY]);


  DEBUG1_PRINT(F("Ep SSID3: "));
  DEBUG1_PRINTLN(AppConfig.access_point[3][indx_SSID]);

  DEBUG1_PRINT(F("Ep Key3: "));
  DEBUG1_PRINTLN(AppConfig.access_point[3][indx_KEY]);


  DEBUG1_PRINT(F("Ep SSID4: "));
  DEBUG1_PRINTLN(AppConfig.access_point[4][indx_SSID]);

  DEBUG1_PRINT(F("Ep Key4: "));
  DEBUG1_PRINTLN(AppConfig.access_point[4][indx_KEY]);

  DEBUG1_PRINT(F("End Point IP:"));
  DEBUG1_PRINTLN(IPAddress(AppConfig.EpIP));
  DEBUG1_PRINT(F("End Point GW:"));
  DEBUG1_PRINTLN(IPAddress(AppConfig.EpGW));
  DEBUG1_PRINT(F("End Point Mask:"));
  DEBUG1_PRINTLN(IPAddress(AppConfig.EpMASK));

  DEBUG1_PRINT(F("Access Point SSID:"));
  DEBUG1_PRINTLN(AppConfig.ApSSID);
  DEBUG1_PRINT(F("Access Point Key:"));
  DEBUG1_PRINTLN(AppConfig.ApKey);
  DEBUG1_PRINT(F("Access Point IP:"));
  DEBUG1_PRINTLN(IPAddress(AppConfig.ApIP));
  DEBUG1_PRINT(F("Access Point GW:"));
  DEBUG1_PRINTLN(IPAddress(AppConfig.ApGW));
  DEBUG1_PRINT(F("Access Point Mask:"));
  DEBUG1_PRINTLN(IPAddress(AppConfig.ApMASK));

  DEBUG1_PRINT(F("DHCP Enabled:(not Used)"));
  DEBUG1_PRINTLN(AppConfig.Flags.bIsDHCPEnabled);
}

void OutChAppConfig()
{
  unsigned char *ptr = (unsigned char *)&AppConfig;
  unsigned char k;
  DEBUG1_PRINTLN(F("About to do lots of bytes out"));
  for (int i = 0; i < sizeof(AppConfig); ++i)
  {
    k = *(ptr + i);
    DEBUG1_PRINT(F("Index: ")); DEBUG1_PRINT(i); DEBUG1_PRINT(F("Char: ")); DEBUG1_PRINT(k); DEBUG1_PRINTLN(char(k));
  }
}
