# ESP8266_FX_951_OLED
my PID controller modified to support T12 soldering tip with OLED, rotary encoder support

This project is based on my other PID temperature controller.
This project measures the temperature of a T12 style soldering iron tip.  
During the ADC read it sets the heater control pin to input (basically tristating it). Then reads the ADC value and converts to a temperature value.
The temperature setting and PID parameters can be set via a web interface. the web interface also plots all the relavant graphs.
They values can also be modified with the rotarty encoder.  The rotary encoder button selects which paramter is to be modified.
