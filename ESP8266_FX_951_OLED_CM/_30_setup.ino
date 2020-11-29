void setup()
{
#include "_init.h"
#include "http.h"
#include "OTA.h"
  //#include "logo.h"
    webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  server.on("/"                 , handle_root);
  server.on("/var"              , handle_variable_adc);
  server.on("/mmv"              , handle_variable_mmv);

  server.on("/pid_config"       , handle_pid_config);
  server.on("/pid_config_update", handle_pid_config_update);
  server.on("/pid_websockets"   , handle_pid_websockets);
  server.on("/pid_websockets_charts"   , handle_pid_websockets_charts);

  server.on("/pid_dataonly"     , handle_pid_data_only);
  server.on("/pid_status"       , handle_pid_status);
  server.on("/pid_help"         , handle_pid_help);
  server.on("/config"           , handle_config);
  server.on("/config_update"    , handle_config_update);
  server.on("/build"            , handle_build);
  server.on("/IO_read"          , handle_IO_read);
  server.on("/IO_write"         , handle_IO_write);

  server.on("/draw"             , handle_draw);
  server.on("/restart"          , handle_restart);
  server.onNotFound(handle_not_found);
  server.begin();
  DEBUG1_PRINTLN(F("HTTP server started"));

  DEBUG1_PRINTLN();
  DEBUG1_PRINTLN();
  DEBUG1_PRINTLN(F("Starting"));

#if ((TEMPERATURE_SENSOR_TYPE(DHT11) || TEMPERATURE_SENSOR_TYPE(DHT21) || TEMPERATURE_SENSOR_TYPE(DHT22)) && ESP8266_PID_WITH_FEATURE(HUMIDIFIER))
  PUMP_OFF;
  pinMode(PUMP_PIN, OUTPUT);
#endif
  pinMode(D4, OUTPUT); //test led, can be removed
  digitalWrite(D4, HIGH);

  HEATER_OFF;
  pinMode(HEATER_PIN, OUTPUT);

//  webSocket.begin();
//  webSocket.onEvent(webSocketEvent);

  MV = computePID(AppConfig.PID[sp1]);//Do a reading so web page canvas centers on current temperature

#if ESP8266_PID_WITH_FEATURE(ROTARY_ENCODER)

  //  pinMode(BUTTON_PIN, INPUT_PULLUP);
  r.setLeftRotationHandler(procLeftRight);
  r.setRightRotationHandler(procLeftRight);
  //attaching to both encoder0Pin_A and encoder0Pin_B and modifying the library to CASE on only one pattern produces more  consistent reliable and accurate results
  attachInterrupt(digitalPinToInterrupt(encoder0Pin_A),  encoderISR,       CHANGE);  //call encoderISR()       every high->low or low->high changes
  attachInterrupt(digitalPinToInterrupt(encoder0Pin_B),  encoderISR,       CHANGE);  //call encoderISR()       every high->low or low->high changes

  //associate butt ints with functions
  for (int i = 0; i < NBR_BUTTS; i++)
  {
    pinMode(buttonPins[i], INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(buttonPins[i]), (*switchFunctions[i]) , FALLING); //LOW does not seem to trigger interrupt so FALLING used
  }
#endif
  endOfSpaceTicker.attach_ms((uint32_t)(INTERVAL_250HZ_PERIOD * 255), endOfSpace); //initiate the tickers for PWM
  //saveProfile();
  //printProfile();
  Serial.print("Sizeof butt0[0]:");
  Serial.println(sizeof(butt0[0]));

  Serial.print("Sizeof butt0:");
  Serial.println(sizeof(butt0));
  
    Serial.print("Sizeof butt0/butt0[0]:");
  Serial.println(numOfButtonStatesButt0);//sizeof(butt0)/sizeof(butt0[0]));


}
