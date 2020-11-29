// handle index

//https://en.wikipedia.org/wiki/List_of_Unicode_characters
//https://en.wikipedia.org/wiki/Numeric_character_reference
//https://en.wikipedia.org/wiki/List_of_XML_and_HTML_character_entity_references#Character_entity_references_in_HTML
// variable names were heavily abbreviated as there was a limit to the size of the page (variable string) that could be sent
// This has now been resolved breaking the page up and using many "server.sendContent(F("
// tf=textfield
// idx=arrindex
// cptag=arrfieldsinput or control parameters




void handle_pid_websockets_charts() {

  START_HTML;  //Macro
  START_PAGE(AppConfig.EpHostname, " PID Controller   "
             "<svg  version='1.0' xmlns='http://www.w3.org/2000/svg'  width='28.000000pt' height='27.000000pt' viewBox='0 0 28.000000 27.000000'  preserveAspectRatio='xMidYMid meet'>  <g id='heatingIcon' transform='translate(0.000000,27.000000) scale(0.100000,-0.100000)' fill='#ff0000' stroke='none'> <path d='M52 246 c-10 -22 -9 -26 4 -26 13 0 15 -7 10 -37 -18 -111 -19 -133 -5 -133 10 0 15 14 16 43 0 23 4 65 7 93 3 34 1 57 -7 69 -12 15 -14 15 -25 -9z'></path> <path d='M201 252 c-13 -25 -14 -35 -2 -27 19 11 22 -36 5 -89 -16 -52 -15 -86 5 -86 5 0 9 15 9 33 0 17 4 58 9 89 7 41 6 63 -3 79 l-11 22 -12 -21z'></path> <path d='M126 244 c-3 -8 0 -22 8 -29 12 -12 12 -23 -2 -73 -14 -55 -13 -92 5 -92 7 0 11 35 16 137 1 23 5 39 10 36 5 -3 3 4 -5 16 -15 25 -24 26 -32 5z'></path> <path d='M2 43 l3 -38 138 -3 137 -3 0 41 c0 22 -5 40 -11 40 -7 0 -8 -11 -4 -30 l7 -30 -121 0 c-67 0 -124 0 -128 0 -4 0 -6 14 -5 30 2 18 -1 30 -8 30 -7 0 -10 -14 -8 -37z'></path> </g> </svg>"
             "<svg id='svg1' style='width: 95px; height: 26px'>"
             //Holder for the red circle that flashes in time with PWM
             "<circle id = 'circle1' r = '10' style = 'fill: white; stroke: black; stroke-width: 1' cx = '22' cy = '15'> </circle>"
             "<text id = 'circle1_text' x='17' y='20' font-size='12' fill='white'>*</text>"
#if ((TEMPERATURE_SENSOR_TYPE(DHT11) || TEMPERATURE_SENSOR_TYPE(DHT21) || TEMPERATURE_SENSOR_TYPE(DHT22)) && ESP8266_PID_WITH_FEATURE(HUMIDIFIER))

             "<circle id = 'circle2' r = '10' style = 'fill: white; stroke: black; stroke-width: 1' cx = '50' cy = '15'> </circle>"
             "<text id = 'circle2_text' x='45' y='20' font-size='12' fill='white'>*</text>"
#endif
             "<circle id = 'circle3' r = '10' style = 'fill: white; stroke: black; stroke-width: 1' cx = '77' cy = '15'> </circle>"
             "<text id = 'circle3_text' x='73' y='20' font-size='12' fill='white'></text>"

             "</svg>");

  server.sendContent(F(
                       "<div align='center' id='outerdiv' style='line-height: 65%; width: 960px; border:5px solid #000000;margin: auto;'>"
                       "<style>.b{white-space:normal;width: 70px;height: 40px;}</style>"
                       "<div id='butts'>"
                       "<button class='b'type='button' id='idButton00' onclick=connection.send('"BUTTON_PREAMBLE"'+this.id);></button>"
                       "<button class='b'type='button' id='idButton01' onclick=connection.send('"BUTTON_PREAMBLE"'+this.id);></button>"
                       "<button class='b'type='button' id='idButton02' onclick=connection.send('"BUTTON_PREAMBLE"'+this.id);></button>"
                       "<button class='b'type='button' id='idButton03' onclick=connection.send('"BUTTON_PREAMBLE"'+this.id);></button>"
                       "<button class='b'type='button' id='idButton04' onclick=connection.send('"BUTTON_PREAMBLE"'+this.id);></button>"
                       "<button class='b'type='button' id='idButton05' onclick=connection.send('"BUTTON_PREAMBLE"'+this.id);></button>"
#if TEMPERATURE_SENSOR_TYPE(DHT11) || TEMPERATURE_SENSOR_TYPE(DHT21) || TEMPERATURE_SENSOR_TYPE(DHT22)
                       "<button class='b'type='button' id='idButton06' onclick=connection.send('"BUTTON_PREAMBLE"'+this.id);></button>"
                       "<button class='b'type='button' id='idButton07' onclick=connection.send('"BUTTON_PREAMBLE"'+this.id);></button>"
#endif
                       //"<button class='b'type='button' id='idButton08' onclick=connection.send('""BUTTON_PREAMBLE""'+this.id);></button>"
                       "<input id='idLoopTime' title='SP1 Time Dur mins 0=Indefinte' disabled='true' style='width: 54px; text-align: center;'>"
                       "</div>"
                       "<br>"
                       "<div id='PIDHeadings'></div>"
                       "<div id='PIDvalues'></div>"
                       "<br style='line-height:0px;' />"
                       "<div id='TuningHeadings'></div>"
                       "<div id='TuningValues'></div>"
                       "<div id='TuningRanges'></div>"

                     ));

  server.sendContent(F(
                       // https://en.wikipedia.org/wiki/List_of_XML_and_HTML_character_entity_references

                       //This canvas contains the Temperature and Humidy
                       "<canvas onclick='showCoords(event)'  data-chartMin='0' data-chartMax='255'  id='PVCanvas' width='900' height='150' style='border:5px solid #FFAD21;'>Browser no support HTML5 canvas tag.</canvas>"

                       //This canvas contains the mv
                       "<canvas  onclick='showCoords(event)' data-chartMin='0' data-chartMax='255' id='MVCanvas' width='900' height='150' style='border:5px solid #FFAD21;'>Browser no support HTML5 canvas tag.</canvas>"
                       "<br>"
                       "<br>"

                       //This canvas contains the err
                       "<canvas  onclick='showCoords(event)' data-chartMin='0' data-chartMax='255' id='errCanvas' width='900' height='150' style='border:5px solid #FFAD21;'>Browser no support HTML5 canvas tag.</canvas>"
                       "<br>"
                       "<br>"

                       //This canvas contains the P Kp
                       "<canvas  onclick='showCoords(event)' data-chartMin='0' data-chartMax='255' id='ptermCanvas' width='900' height='150' style='border:5px solid #FFAD21;'>Browser no support HTML5 canvas tag.</canvas>"
                       "<br>"
                       "<br>"

                       //This canvas contains the I Ki
                       "<canvas  onclick='showCoords(event)' data-chartMin='0' data-chartMax='255' id='itermCanvas' width='900' height='150' style='border:5px solid #FFAD21;'>Browser no support HTML5 canvas tag.</canvas>"
                       "<br>"
                       "<br>"

                       //This canvas contains the D Kd
                       "<canvas  onclick='showCoords(event)' data-chartMin='0' data-chartMax='255' id='dtermCanvas' width='900' height='150' style='border:5px solid #FFAD21;'>Browser no support HTML5 canvas tag.</canvas>"
                       "<br>"
                       "<br>"

                       //This canvas contains all
                       "<canvas  onclick='showCoords(event)' data-chartMin='0' data-chartMax='255' id='combCanvas' width='900' height='150' style='border:5px solid #FFAD21;'>Browser no support HTML5 canvas tag.</canvas>"
                       "<br>"
                       "<br>"

                       "</div>"));
  END_PAGE_CUSTOM;
  END_PAGE;

  server.sendContent(F(
                       "<script charset='utf-8'>"
                       "const sCX=900;" //X axis Size of Canvas
                       "const sCY=150;" //Y axis size of Canvas
                       "var pauseAsynUpdate = false;"
                       "var in_max=0;" //Maximum Value paramter for map functiom
                       "var in_min=0;" //Minimum value parameter for map function
                     )); //MVS Maniplulated Variable Scaler

  server.sendContent(F("var cpdata=["));
  for (int i = 0; i < cpsize; i++)
  {
    server.sendContent(String(AppConfig.PID[i]));
    server.sendContent(F(","));
  }
  server.sendContent(F("];"));
  server.sendContent(F("var cptag=["));
  for (int i = 0; i < cpsize; i++)
  {
    server.sendContent(F("'"));
    server.sendContent((cptag[i]));
    server.sendContent(F("',"));
  }
  server.sendContent(F("];"));
  server.sendContent(F("var cpmax=["));
  for (int i = 0; i < cpsize; i++)
  {
    server.sendContent((cpmax[i]));
    server.sendContent(F(","));
  }
  server.sendContent(F("];"));
  server.sendContent(F("var cpmin=["));
  for (int i = 0; i < cpsize; i++)
  {
    server.sendContent((cpmin[i]));
    server.sendContent(F(","));
  }
  server.sendContent(F("];"));
  server.sendContent(F("var cpstep=["));
  for (int i = 0; i < cpsize; i++)
  {
    server.sendContent((cpstep[i]));
    server.sendContent(F(","));
  }
  server.sendContent(F("];"));

  server.sendContent(F("var cpttip=["));
  for (int i = 0; i < cpsize; i++)
  {
    server.sendContent(F("'"));
    server.sendContent((cpttip[i]));
    server.sendContent(F("',"));
  }
  server.sendContent(F("];"));

  server.sendContent(F(
                       "var arrpv=[];"
#if TEMPERATURE_SENSOR_TYPE(DHT11) || TEMPERATURE_SENSOR_TYPE(DHT21) || TEMPERATURE_SENSOR_TYPE(DHT22)
                       "var arrhum=[];"
#endif
                       "var arrmv=[];"
                       "var arrerr=[];"
                       "var arrpterm=[];"
                       "var arriterm=[];"
                       "var arrdterm=[];"
                       "var units='C';"
                       "Number.prototype.map = function (in_min, in_max, out_min, out_max) "
                       "{if(in_min == in_max){return(sCY/2);}"
                       "else"
                       "{return ((this - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);}"
                       "};"  //Need the last semicolon not sure why  //snd.play();
                       "function showCoords(event)\
					   {\
						   var canvas=document.getElementById(event.currentTarget.id);\
						   var ctx = canvas.getContext('2d');\
						   var rect = event.currentTarget.getBoundingClientRect();\
						   var x = event.clientX-rect.left;\
						   var y = event.clientY-rect.top;\
						   var chartMin = parseFloat(canvas.dataset.chartmin);\
						   var chartMax = parseFloat(canvas.dataset.chartmax);\
						   var Val = y.map(sCY-10,4,chartMin,chartMax);\
						   if(y < 20 ) {y = 20};\
						  ctx.strokeStyle='lightgrey';\
						  ctx.font = '30px Arial';\
						  if((units == 'F') && (event.currentTarget.id == 'PVCanvas')){\
						  ctx.strokeText((parseFloat(Val)*1.8+32).toFixed(2), x, y);\
						  }else{\
						  ctx.strokeText(parseFloat(Val).toFixed(2), x, y);\
						  }\
						}"

                       "function beep() {"
                       //https://dopiaza.org/tools/datauri/index.php
                       "var snd = new Audio('data:audio/wav;base64,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qJrx4KOQjahZxWbcZgztj2c49nKmkId44S71j0c8eV9yDK6uPRzx5X18eDvjvQ6yKo9ZSS6l//8elePK/Lf//IInrOF/FvDoADYAGBMGb7FtErm5MXMlmPAJQVgWta7Zx2go+8xJ0UiCb8LHHdftWyLJE0QIAIsI+UbXu67dZMjmgDGCGl1H+vpF4NSDckSIkk7Vd+sxEhBQMRU8j/12UIRhzSaUdQ+rQU5kGeFxm+hb1oh6pWWmv3uvmReDl0UnvtapVaIzo1jZbf/pD6ElLqSX+rUmOQNpJFa/r+sa4e/pBlAABoAAAAA3CUgShLdGIxsY7AUABPRrgCABdDuQ5GC7DqPQCgbbJUAoRSUj+NIEig0YfyWUho1VBBBA//uQZB4ABZx5zfMakeAAAAmwAAAAF5F3P0w9GtAAACfAAAAAwLhMDmAYWMgVEG1U0FIGCBgXBXAtfMH10000EEEEEECUBYln03TTTdNBDZopopYvrTTdNa325mImNg3TTPV9q3pmY0xoO6bv3r00y+IDGid/9aaaZTGMuj9mpu9Mpio1dXrr5HERTZSmqU36A3CumzN/9Robv/Xx4v9ijkSRSNLQhAWumap82WRSBUqXStV/YcS+XVLnSS+WLDroqArFkMEsAS+eWmrUzrO0oEmE40RlMZ5+ODIkAyKAGUwZ3mVKmcamcJnMW26MRPgUw6j+LkhyHGVGYjSUUKNpuJUQoOIAyDvEyG8S5yfK6dhZc0Tx1KI/gviKL6qvvFs1+bWtaz58uUNnryq6kt5RzOCkPWlVqVX2a/EEBUdU1KrXLf40GoiiFXK///qpoiDXrOgqDR38JB0bw7SoL+ZB9o1RCkQjQ2CBYZKd/+VJxZRRZlqSkKiws0WFxUyCwsKiMy7hUVFhIaCrNQsKkTIsLivwKKigsj8XYlwt/WKi2N4d//uQRCSAAjURNIHpMZBGYiaQPSYyAAABLAAAAAAAACWAAAAApUF/Mg+0aohSIRobBAsMlO//Kk4soosy1JSFRYWaLC4qZBYWFRGZdwqKiwkNBVmoWFSJkWFxX4FFRQWR+LsS4W/rFRb/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////VEFHAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAU291bmRib3kuZGUAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAMjAwNGh0dHA6Ly93d3cuc291bmRib3kuZGUAAAAAAAAAACU=');"
                       "snd.play();"
                       "}"

                       //Process Tuning Variable Input changes
                       "function processTuningChange(evt){"
                       "var value = parseFloat(this.value);"//evt.target.value;"
                       "var idx = parseInt(this.id.slice(-2));"//selectionEnd
                       //"if(units == 'F' && (cptag[idx] == 'sp1' || cptag[idx] == 'sp2')){"
                       //"value = ((value-32)/1.8);"
                       //"}"

                       "if(value >= cpmin[idx] && value <= cpmax[idx]){"
                       "connection.send('"RANGE_PREAMBLE"'+this.id+'='+value);}else{"
                       "alert('Out of limits')};"
                       "pauseAsynUpdate = false;"
                       "};"
                       //Update Chart Canvas  *************************************************
                       "function updateChart(chart,arrData,plotColour,item) {"
                       "var canvas=document.getElementById(chart+'Canvas');"
                       "var ctx=canvas.getContext('2d');"

                       "ctx.fillStyle='black';"
                       "ctx.fillRect(0,0,sCX,sCY);"

                       "if(arrData.length >sCX){arrData.push(arrData.shift());arrData.pop();}"
                       "arrData.push(item);"

                       "in_max = Math.max(...arrData);"
                       "in_min = Math.min(...arrData);"

                       "canvas.dataset.chartmin=in_min;"
                       "canvas.dataset.chartmax=in_max;"

                       "for(cnt=0;cnt<arrData.length;cnt++){"
                       "ctx.beginPath();"
                       "ctx.moveTo(cnt,parseFloat(arrData[cnt]).map(in_min,in_max,sCY-10,4));"
                       "ctx.lineTo(cnt+1,parseFloat(arrData[cnt+1]).map(in_min,in_max,sCY-10,4));"
                       "ctx.strokeStyle=plotColour;"
                       "ctx.stroke();"
                       "}"

                       //draw tick marks
                       "ctx.strokeStyle = 'green';"
                       "ctx.font = '10px Courier New';"
                       "var j=0;"

                       "for(i =0;i< sCX;i+=60){"
                       "ctx.moveTo(i,0);"
                       "ctx.lineTo(i,sCY);"
                       "ctx.stroke();"
                       "ctx.strokeText(j.toFixed(1)+'min',i+1,sCY-2);"
                       "var zeropad = '';"
                       "var idx = cptag.indexOf('tc');"
                       "if(idx<10){zeropad = '0';}"
                       "var inputid = 'idInput' + zeropad + idx;"
                       "j=j+document.getElementById(inputid).value/1;"

                       //"j=j+cpdata[cptag.indexOf('tc')]/1;"
                       "}" //end of for above

                       //draw horizontal min max ticks
                       "ctx.strokeText(in_min.toFixed(1),0,sCY-10);"
                       "ctx.strokeText(in_max.toFixed(1),0,10);"

                       //text out values
                       "ctx.strokeStyle='lightgrey';"
                       "ctx.font = '30px Arial';"
                       "ctx.strokeText(chart + ':' + document.getElementById('id'+chart).value ,0,50);"
                       "}"

                       //Update Chart PV and HUM Canvas  *************************************************
                       "function updatePVChart(chart,arrData,plotColour,item,setP1,setP2) {"
                       "var canvas=document.getElementById(chart+'Canvas');"
                       "var ctx=canvas.getContext('2d');"
                       "ctx.fillStyle='black';"
                       "ctx.fillRect(0,0,sCX,sCY);"

                       "if(arrData.length >sCX){arrData.push(arrData.shift());arrData.pop();}"
                       "arrData.push(item);"


                       //"var alertLevel = cpdata[cptag.indexOf('alert')];"

                       "var idx = cptag.indexOf('minalert');"
                       "var zeropad = '';"
                       "if(idx<10){zeropad = '0';}"
                       "var inputid = 'idInput' + zeropad + idx;"
                       "alertLevel = document.getElementById(inputid).value ;"

                       "if(item < alertLevel && alertLevel > 0){beep();}"


                       "var idx = cptag.indexOf('maxalert');"
                       "var zeropad = '';"
                       "if(idx<10){zeropad = '0';}"
                       "var inputid = 'idInput' + zeropad + idx;"
                       "alertLevel = document.getElementById(inputid).value ;"

                       "if(item > alertLevel && alertLevel > 0){beep();}"

                       "in_max = Math.max(...arrData,setP1,setP2);"
                       "in_min = Math.min(...arrData,setP1,setP2);"

                       "canvas.dataset.chartmin=in_min;"
                       "canvas.dataset.chartmax=in_max;"
#if TEMPERATURE_SENSOR_TYPE(DHT11) || TEMPERATURE_SENSOR_TYPE(DHT21) || TEMPERATURE_SENSOR_TYPE(DHT22)
                       "if(arrhum.length >sCX){arrhum.push(arrhum.shift());arrhum.pop();}"
                       "arrhum.push(document.getElementById('idhum').value);"
#endif

                       //draw verticle tick marks
                       "ctx.strokeStyle = 'green';"
                       "ctx.font = '10px Courier New';"
                       "var j=0;"

                       "for(i =0;i< sCX;i+=60){"
                       "ctx.beginPath();" //dfgdfgd
                       "ctx.moveTo(i,0);"
                       "ctx.lineTo(i,sCY);"
                       "ctx.stroke();"
                       "ctx.strokeText(j.toFixed(1)+'min',i+1,sCY-2);"
                       // "j=j+cpdata[cptag.indexOf('tc')]/1;"
                       //"j=j+document.getElementById('idInput10').value/1;"
                       "var zeropad = '';"
                       "var idx = cptag.indexOf('tc');"
                       "if(idx<10){zeropad = '0';}"
                       "var inputid = 'idInput' + zeropad + idx;"
                       "j=j+document.getElementById(inputid).value/1;"
                       "}"
                       //                     "ctx.stroke();" //fdfdfd

                       //draw horizontal min max ticks
                       "if(units == 'F'){"
                       "ctx.strokeText((in_min*1.8 + 32).toFixed(1),0,sCY-10);"
                       "ctx.strokeText((in_max*1.8 + 32).toFixed(1),0,10);"
                       "}else{"
                       "ctx.strokeText(in_min.toFixed(1),0,sCY-10);"
                       "ctx.strokeText(in_max.toFixed(1),0,10);"
                       "}"
                       "ctx.stroke();" ////ddsds
                       "ctx.beginPath();" //fgvdfgdf

                       //text out values
                       "ctx.strokeStyle='lightgrey';"
                       "ctx.font = '30px Arial';"
                       "ctx.strokeText(chart + ':' + document.getElementById('id'+chart).value +'\xB0'+ units,0,50);"
#if TEMPERATURE_SENSOR_TYPE(DHT11) || TEMPERATURE_SENSOR_TYPE(DHT21) || TEMPERATURE_SENSOR_TYPE(DHT22)
                       "ctx.strokeText('hum:' + document.getElementById('idhum').value ,0,100);"
#endif
                       //Update Stage1 Stage 2 bounding box
                       "var stage1=parseFloat(setP1).map(in_min,in_max,sCY-10,4);"
                       "var stage2=parseFloat(setP2).map(in_min,in_max,sCY-10,4);"

                       "ctx.rect(0,stage1,sCX,stage2-stage1);"
                       "ctx.stroke();"

                       "ctx.beginPath();" //dfgdfgd

                       "ctx.strokeStyle = 'green';"
                       "ctx.font = '12px Courier New';"
                       "if(units == 'F'){"
                       "ctx.strokeText('sp1: ' + (parseFloat(setP1)*1.8 + 32) ,sCX - 90,stage1+ 12);"
                       "ctx.strokeText('sp2: ' + (parseFloat(setP2)*1.8 + 32) ,sCX - 90,stage2 + 12);"
                       "}else{"
                       "ctx.strokeText('sp1: ' + setP1 ,sCX - 90,stage1+ 12);"
                       "ctx.strokeText('sp2: ' + setP2 ,sCX - 90,stage2 + 12);"
                       "}"

                       //Plot PV Do this last so it writes over the other pixels
                       "for(cnt=0;cnt<arrData.length;cnt++){"
                       "ctx.beginPath();"
                       "ctx.moveTo(cnt,parseFloat(arrData[cnt]).map(in_min,in_max,sCY-10,4));"
                       "ctx.lineTo(cnt+1,parseFloat(arrData[cnt+1]).map(in_min,in_max,sCY-10,4));"
                       "ctx.strokeStyle=plotColour;"
                       "ctx.stroke();"


                       //Plot Humidity
#if TEMPERATURE_SENSOR_TYPE(DHT11) || TEMPERATURE_SENSOR_TYPE(DHT21) || TEMPERATURE_SENSOR_TYPE(DHT22)
                       "ctx.beginPath();"
                       "ctx.moveTo(cnt,parseFloat(arrhum[cnt]).map(0,100,sCY-10,4));"
                       "ctx.lineTo(cnt+1,parseFloat(arrhum[cnt+1]).map(0,100,sCY-10,4));"
                       "ctx.strokeStyle='lightblue';"
                       "ctx.stroke();"
#endif
                       "}"


                       "}"

                       //Update Combined Chart Canvas  *************************************************
                       //"updateChart('myKdCanvas',arrdterm,document.getElementById('iddterm').value);"
                       "function updateCombChart(chart,arrData,plotColour,textPosX,textPosY) {"
                       "var canvas=document.getElementById('combCanvas');"
                       "var ctx=canvas.getContext('2d');"

                       "in_max = Math.max(...arrData);"
                       "in_min = Math.min(...arrData);"

                       "for(cnt=0;cnt<arrData.length;cnt++){"
                       "ctx.beginPath();"
                       "ctx.moveTo(cnt,parseFloat(arrData[cnt]).map(in_min,in_max,sCY-10,4));"
                       "ctx.lineTo(cnt+1,parseFloat(arrData[cnt+1]).map(in_min,in_max,sCY-10,4));"
                       "ctx.strokeStyle=plotColour;"
                       "ctx.stroke();"
                       "}"

                       //text out values
                       "ctx.strokeStyle='lightgrey';"
                       "ctx.font = '30px Arial';"
                       "ctx.strokeText(chart + ':' + document.getElementById('id'+chart).value ,textPosX,textPosY);"
                       "}"
                     ));


  //many of the page elements are generated as the page is rendered.  This reduced the size of the page to send.  This is not so important now
  //"var aFO=['ticks','PV','err','pterm','iterm','dterm','MV','millis','hum','state','timer','errHum'];"));
  server.sendContent(F("var PIDformatString='"));
  server.sendContent(PIDformatString);
  server.sendContent(F("';"));
  server.sendContent(F("var PIDHeadingToolTips='"));
  server.sendContent(PIDHeadingToolTips);
  server.sendContent(F("';"));

  server.sendContent(F(
                       "var PIDformatElements = PIDformatString.slice(2).split(',');"
                       "var PIDToolTipElements = PIDHeadingToolTips.split(',');"

                       "var element=document.getElementById('PIDHeadings');"
                       "for (idx=0;idx<PIDformatElements.length;idx++)"
                       "{"
                       "tf=document.createElement('input');"
                       "element.appendChild(tf);"
                       "tf.value=PIDformatElements[idx].split('=')[0];"
                       "tf.title=PIDToolTipElements[idx];"
                       "tf.disabled = true;"
                       "tf.style.width='70px';"
                       "tf.style.textAlign='center';"
                       "}"

                       "var element=document.getElementById('PIDvalues');"
                       "for (idx=0;idx<PIDformatElements.length;idx++)"
                       "{"
                       "tf=document.createElement('input');"
                       "element.appendChild(tf);"
                       "tf.id='id'+PIDformatElements[idx].split('=')[0];"
                       "tf.disabled = true;"
                       "tf.style.width='70px';"
                       "tf.style.textAlign='center';"
                       "}"
                     ));

  server.sendContent(F(
                       //Render the Tuning Headings
                       "var element=document.getElementById('TuningHeadings');"
                       "for(idx=0;idx<cptag.length;idx++)"
                       "{"
                       "tf=document.createElement('input');"
                       "element.appendChild(tf);"
                       "tf.value=cptag[idx];"
                       "tf.title=cpttip[idx];"
                       "tf.disabled = true;"
                       "tf.style.width='54px';"
                       "tf.style.textAlign='center';"
                       "}"

                       //Render the Tuning Default Input Values and Range input sliders
                       "var element=document.getElementById('TuningValues');"
                       "var trElements=document.getElementById('TuningRanges');"
                       "for(idx=0;idx<cptag.length;idx++)"
                       "{"

                       //Input Elements
                       "tf=document.createElement('input');"
                       "element.appendChild(tf);"
                       "tf.id=cptag[idx];"
                       "tf.type = 'number';"
                       "tf.dir='rtl';"
                       "tf.value=cpdata[idx];"
                       "tf.max = cpmax[idx];"
                       "tf.min = cpmin[idx];"
                       "tf.step = cpstep[idx];"
                       "tf.disabled = false;"
                       "tf.style.width='54px';"
                       "tf.style.textAlign='right';"
                       "var zeropad = '';"
                       "if(idx<10){zeropad = '0';}"
                       "var inputid = 'idInput' + zeropad + idx;"
                       "tf.id=inputid;"
                       "tf.onchange=processTuningChange;"
                       "tf.onmousedown=function(){pauseAsynUpdate = true};"
                       "tf.onmouseup=function(){pauseAsynUpdate = false};"
                       "tf.oninput=function(){pauseAsynUpdate = true};"

                       //Range Slider Elements
                       "tr=document.createElement('input');"
                       "trElements.appendChild(tr);"
                       "tr.id=cptag[idx];"
                       "tr.type = 'range';"
                       "tr.value=cpdata[idx];"
                       "tr.max = cpmax[idx];"
                       "tr.min = cpmin[idx];"
                       "tr.step = cpstep[idx];"
                       "tr.disabled = false;"
                       "tr.style.width='58px';"
                       "tr.style.textAlign='center';"
                       "var zeropad = '';"
                       "if(idx<10){zeropad = '0';}"
                       "var inputid = 'idInputR' + zeropad + idx;"
                       "tr.id=inputid;"
                       "tr.onchange=processTuningChange;"
                       "tr.onmousedown=function(){pauseAsynUpdate = true};"
                       "tr.onmouseup=function(){pauseAsynUpdate = false};"
                       "tr.oninput=function(){pauseAsynUpdate = true};"
                       "}"
                     ));

  //Process web connections
  server.sendContent(F(
                       "var connection=new WebSocket('ws://'+location.hostname+':81/',['arduino']);"
                       "connection.onopen=function(){connection.send('Connect '+new Date());};"
                       "connection.onerror=function(error){console.log('WebSocket Error ',error);};"
                       
                    //   "connection.onclose=function(event){var readystate = connection.readyState;"
                    //   "console.log('readstate:',readystate);"

                    //   "if(readystate == 3){location.reload(1);}"
                    //   "};"

                       //incomming data from esp8266
                       "connection.onmessage=function(e){console.log('Server:',e.data);"
                       "var msg=e.data;"
                       "if(msg!='Connected'){"
                       "if (msg=='"HEARTBEAT_MSG"'){"
                       "if (circle3.style.fill=='darkgreen'){circle3.style.fill='lightgreen';}else{circle3.style.fill='darkgreen';} }"

                       "else if(msg=='"HEATER_ON_MSG"'){circle1.style.fill='red';heatingIcon.style.fill='red';}"
                       "else if(msg=='"HEATER_OFF_MSG"'){circle1.style.fill='DarkRed';heatingIcon.style.fill='DarkRed';}"


                       "else if(msg=='"HEATER_HIGH_MSG"'){circle1_text.innerHTML='H';}"
                       "else if(msg=='"HEATER_LOW_MSG"'){circle1_text.innerHTML='L';}"

#if TEMPERATURE_SENSOR_TYPE(DHT11) || TEMPERATURE_SENSOR_TYPE(DHT21) || TEMPERATURE_SENSOR_TYPE(DHT22)
                       "else if(msg=='"PUMP_HIGH_MSG"'){circle2_text.innerHTML='H';}"
                       "else if(msg=='"PUMP_LOW_MSG"'){circle2_text.innerHTML='L';}"

                       "else if(msg=='"PUMP_ON_MSG"'){circle2.style.fill='LightBlue';}"
                       "else if(msg=='"PUMP_OFF_MSG"'){circle2.style.fill='DarkBlue';}"
#endif
                       //process incomming slider updates
                       "else if(msg.indexOf('"CONTROL_PARAMS_PREAMBLE"') == 0 && pauseAsynUpdate == false){"
                       "msg = msg.slice(2);"

                       "var arrmsg = msg.split(',');"
                       "for(idx=0;idx<arrmsg.length - 1;idx++)"
                       "{"
                       "var zeropad = '';"
                       "if(idx<10){zeropad = '0';}"
                       "var inputid = 'idInput' + zeropad + idx;"
                       //                      "var inputrid = 'idInputR' + zeropad + idx;"

                       "document.getElementById(inputid).valueAsNumber = parseFloat(arrmsg[idx]);"
                       //                    "document.getElementById(inputrid).value = parseFloat(arrmsg[idx]);"
                       "var activeSliderColor = '#ffffff';"//'#0f0f0f';"
                       "if(idx == arrmsg[arrmsg.length-1]){activeSliderColor = '#00ffff';}" //last arrary element points to active element//'#c51818';}"
                       "document.getElementById(inputid).style.backgroundColor = activeSliderColor;"


                       "if(units == 'F' && (cptag[idx] == 'sp1' || cptag[idx] == 'sp2')){"
                       //"document.getElementById(inputid).value = parseFloat(arrmsg[idx])*1.8 + 32;"
                       // "cpdata[idx] = parseFloat(arrmsg[idx]);"
                       "}else{"
                       // "cpdata[idx] = parseFloat(arrmsg[idx]);"
                       "}"
                       "}"
                       "}"

                       //process incomming button updates

                       "else if(msg.indexOf('"BUTTON_PREAMBLE"') == 0){"
                       "msg = msg.slice(2);"
                       "var arrmsg=msg.split(',');"
                       "var zeropad = '';"
                       "for (idx=0;idx<arrmsg.length;idx++)"
                       "{"
                       "if(idx<10){zeropad = '0';}"
                       "var buttonid = 'idButton' + zeropad + idx;"
                       "document.getElementById(buttonid).innerHTML = arrmsg[idx];"
                       "}"
                       "if(arrmsg[1].indexOf('C') == -1){units = 'F';}else{units='C';}"
                       "}"

                       //process random variable as required
                       "else if(msg.indexOf('"DEBUG_PREAMBLE"') == 0){"
                       "msg = msg.slice(1);"
                       "document.getElementById('idLoopTime').value=msg;"
                       "}"


                       //process incoming key-value pairs
                       "else if(msg.indexOf('"KEY_PAIR_PREAMBLE"') == 0){"
                       "msg = msg.slice(2);"
                       "var pairs=msg.split(',');"
                       "var keypairMap = new Map();"// – creates the map.
                       "for(var idx =0;idx < pairs.length;idx++)"
                       "{"
                       "var keyvalue = pairs[idx].split('=');"
                       "keypairMap.set(keyvalue[0],keyvalue[1]);"
                       /* "document.getElementById('id'+keyvalue[0]).value=parseFloat(keyvalue[1]);"
                         "if(keyvalue[0] == 'state'){document.getElementById('id'+keyvalue[0]).value=keyvalue[1]};"
                         "if(keyvalue[0] == 'clock' || keyvalue[0] == 'timer'){document.getElementById('id'+keyvalue[0]).value"
                         " = Math.floor(parseFloat(keyvalue[1]) / (24*60*60*1000)) + ':' + new Date(parseFloat(keyvalue[1])).toISOString().substr(11, 8)};"
                         "if(keyvalue[0] == 'PV'){"
                         "var pv_celsius = keyvalue[1];"
                         "if(units == 'F'){document.getElementById('idPV').value=Math.round(keyvalue[1]*1.8 + 32);}}"
                       */
                       "}"

                       "document.getElementById('idticks').value=parseFloat(keypairMap.get('ticks'));"
                       "document.getElementById('idPV').value=parseFloat(keypairMap.get('PV'));"
                       "if(units == 'F'){document.getElementById('idPV').value=Math.round(keypairMap.get('PV')*1.8 + 32);}"
                       "document.getElementById('iderr').value=parseFloat(keypairMap.get('err'));"
                       "document.getElementById('idpterm').value=parseFloat(keypairMap.get('pterm'));"
                       "document.getElementById('iditerm').value=parseFloat(keypairMap.get('iterm'));"
                       "document.getElementById('iddterm').value=parseFloat(keypairMap.get('dterm'));"
                       "document.getElementById('idMV').value=parseFloat(keypairMap.get('MV'));"
#if TEMPERATURE_SENSOR_TYPE(DHT11) || TEMPERATURE_SENSOR_TYPE(DHT21) || TEMPERATURE_SENSOR_TYPE(DHT22)
                       "document.getElementById('idhum').value=parseFloat(keypairMap.get('hum'));"
                       "document.getElementById('iderrHum').value=parseFloat(keypairMap.get('errHum'));"
#endif
                       "document.getElementById('idstate').value=keypairMap.get('state');"
                       "document.getElementById('idclock').value=Math.floor(parseFloat(keypairMap.get('clock')) / (24*60*60*1000)) + ':' + new Date(parseFloat(keypairMap.get('clock'))).toISOString().substr(11, 8);"
                       "document.getElementById('idtimer').value=Math.floor(parseFloat(keypairMap.get('timer')) / (24*60*60*1000)) + ':' + new Date(parseFloat(keypairMap.get('timer'))).toISOString().substr(11, 8);"

                       "updatePVChart('PV',arrpv,'red',parseFloat(keypairMap.get('PV')),idInput00.value,idInput01.value);"
                       "updateChart('MV',arrmv,'yellow',parseFloat(keypairMap.get('MV')));"
                       "updateChart('err',arrerr,'brown',parseFloat(keypairMap.get('err')));"
                       "updateChart('pterm',arrpterm,'purple',parseFloat(keypairMap.get('pterm')));"
                       "updateChart('iterm',arriterm,'blue',parseFloat(keypairMap.get('iterm')));"
                       "updateChart('dterm',arrdterm,'orange',parseFloat(keypairMap.get('dterm')));"

                       "var canvas=document.getElementById('combCanvas');"
                       "var ctx=canvas.getContext('2d');"
                       "ctx.fillStyle='black';"
                       "ctx.fillRect(0,0,sCX,sCY);"

                       "updateCombChart('PV',arrpv,'red',0,50);"
                       "updateCombChart('MV',arrmv,'yellow',0,100);"
                       "updateCombChart('pterm',arrpterm,'purple',250,50);"
                       "updateCombChart('iterm',arriterm,'blue',250,100);"
                       "updateCombChart('dterm',arrdterm,'orange',500,50);"

                       //draw tick marks
                       "ctx.strokeStyle = 'green';"
                       "ctx.font = '10px Courier New';"
                       "var j=0;"
                       "for(i =0;i< sCX;i+=60){"
                       "ctx.moveTo(i,0);"
                       "ctx.lineTo(i,sCY);"
                       "ctx.stroke();"
                       "ctx.strokeText(j.toFixed(1)+'min',i+1,sCY-2);"
                       "var zeropad = '';"
                       "var idx = cptag.indexOf('tc');"
                       "if(idx<10){zeropad = '0';}"
                       "var inputid = 'idInput' + zeropad + idx;"
                       "j=j+document.getElementById(inputid).value/1;"

                       //                       "j=j+cpdata[cptag.indexOf('tc')]/1;"
                       "}" //end of for above

                       "}" //end of else
                       "}" //end of if connected
                       "};" //end of onmessage
                       "</script>"));
}
