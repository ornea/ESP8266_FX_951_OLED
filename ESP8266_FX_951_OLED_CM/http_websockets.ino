// handle index

//https://en.wikipedia.org/wiki/List_of_Unicode_characters
//https://en.wikipedia.org/wiki/Numeric_character_reference
//https://en.wikipedia.org/wiki/List_of_XML_and_HTML_character_entity_references#Character_entity_references_in_HTML
// variable names were heavily abbreviated as there was a limit to the size of the page (variable string) that could be sent
// This has now been resolved breaking the page up and using many "server.sendContent(F("
// tf=textfield
// idx=arrindex
// cptag=arrfieldsinput or control parameters




void handle_pid_websockets() {

  START_HTML;  //Macro
  START_PAGE(AppConfig.EpHostname, " PID Controller"
             "<svg id='svg1' style='width: 65px; height: 26px'>"
             //Holder for the red circle that flashes in time with PWM
             "<circle id = 'circle1' r = '10' style = 'fill: white; stroke: black; stroke-width: 1' cx = '22' cy = '15'> </circle>"
             "<text id = 'circle1_text' x='17' y='20' font-size='12' fill='white'>*</text>"
             "<circle id = 'circle2' r = '10' style = 'fill: white; stroke: black; stroke-width: 1' cx = '50' cy = '15'> </circle>"
             "<text id = 'circle2_text' x='45' y='20' font-size='12' fill='white'>*</text>"
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
                       "<button class='b'type='button' id='idButton06' onclick=connection.send('"BUTTON_PREAMBLE"'+this.id);></button>"
                       "<button class='b'type='button' id='idButton07' onclick=connection.send('"BUTTON_PREAMBLE"'+this.id);></button>"
                       //"<button class='b'type='button' id='idButton08' onclick=connection.send('""BUTTON_PREAMBLE""'+this.id);></button>"
                       "</div>"
                       "<br>"
                       "<div id='PIDHeadings'></div>"
                       "<div id='PIDvalues'></div>"
                     ));

  server.sendContent(F(
                       // https://en.wikipedia.org/wiki/List_of_XML_and_HTML_character_entity_references
                       "<div id='idpvtxt'><br><font color='red'>pv (temperature &#176;C)</font><font color='blue'> hum (Humidity %)</font></div>"

                       //This canvas contains the Temperature and Humidy
                       "<canvas onclick='showCoords(event)'  id='mypvCanvas' width='900' height='150' style='border:5px solid #FFAD21;'>Browser no support HTML5 canvas tag.</canvas>"

                       "<div  id='idmvtxt'><br><font color='red'> MV (aka MV)</font><font color='green'> pterm </font><font color='blue'> iterm </font><font color='white'> dterm </font> </div>"

                       //This canvas contains the mv & PID terms
                       "<canvas  onclick='showCoords(event)' id='mymvCanvas' width='900' height='150' style='border:5px solid #FFAD21;'>Browser no support HTML5 canvas tag.</canvas>"
                       "<br>"

                       //Range Sliders
                       "<div><br>pid tuning levers</div>"
                       "<div class='row' style='border:5px solid #FFAD21;'>"

                       "<div class='column' id='arrrangeinput1' style='background-color:#bbb;float: left;width: 33.33%;padding: 10px;box-sizing: border-box;'>"

                       "<label id='idRangeLab00' style='float:left;width: 65px;' for='idRange00'>sp1[330]</label>"
                       "<input style='width:55%;' type='range' id='idRange00' oninput=connection.send('"RANGE_PREAMBLE"'+this.id+'='+this.value);>"
                       "<label id='idRangeLab01' style='float:left;width: 65px;' for='idRange01'>sp1[330]</label>"
                       "<input style='width:55%;' type='range' id='idRange01' oninput=connection.send('"RANGE_PREAMBLE"'+this.id+'='+this.value);>"
                       "<label id='idRangeLab02' style='float:left;width: 65px;' for='idRange02'>sp1[330]</label>"
                       "<input style='width:55%;' type='range' id='idRange02' oninput=connection.send('"RANGE_PREAMBLE"'+this.id+'='+this.value);>"
                       "<label id='idRangeLab03' style='float:left;width: 65px;' for='idRange03'>sp1[330]</label>"
                       "<input style='width:55%;' type='range' id='idRange03' oninput=connection.send('"RANGE_PREAMBLE"'+this.id+'='+this.value);>"
                       "</div>"

                       "<div class='column' id='arrrangeinput1' style='background-color:#bbb;float: left;width: 33.33%;padding: 10px;box-sizing: border-box;'>"
                       "<label id='idRangeLab04' style='float:left;width: 65px;' for='idRange04'>sp1[330]</label>"
                       "<input style='width:55%;' type='range' id='idRange04' oninput=connection.send('"RANGE_PREAMBLE"'+this.id+'='+this.value);>"
                       "<label id='idRangeLab05' style='float:left;width: 65px;' for='idRange05'>sp1[330]</label>"
                       "<input style='width:55%;' type='range' id='idRange05' oninput=connection.send('"RANGE_PREAMBLE"'+this.id+'='+this.value);>"
                       "<label id='idRangeLab06' style='float:left;width: 65px;' for='idRange06'>sp1[330]</label>"
                       "<input style='width:55%;' type='range' id='idRange06' oninput=connection.send('"RANGE_PREAMBLE"'+this.id+'='+this.value);>"
                       "<label id='idRangeLab07' style='float:left;width: 65px;' for='idRange07'>sp1[330]</label>"
                       "<input style='width:55%;' type='range' id='idRange07' oninput=connection.send('"RANGE_PREAMBLE"'+this.id+'='+this.value);>"
                       "<label id='idRangeLab08' style='float:left;width: 65px;' for='idRange08'>sp1[330]</label>"
                       "<input style='width:55%;' type='range' id='idRange08' oninput=connection.send('"RANGE_PREAMBLE"'+this.id+'='+this.value);>"
                       "</div>"

                       "<div class='column' id='arrrangeinput1' style='background-color:#bbb;float: left;width: 33.33%;padding: 10px;box-sizing: border-box;'>"
                       "<label id='idRangeLab09' style='float:left;width: 65px;' for='idRange09'>sp1[330]</label>"
                       "<input style='width:55%;' type='range' id='idRange09' oninput=connection.send('"RANGE_PREAMBLE"'+this.id+'='+this.value);>"
                       "<label id='idRangeLab10' style='float:left;width: 65px;' for='idRange10'>sp1[330]</label>"
                       "<input style='width:55%;' type='range' id='idRange10' oninput=connection.send('"RANGE_PREAMBLE"'+this.id+'='+this.value);>"
                       "<label id='idRangeLab11' style='float:left;width: 65px;' for='idRange11'>sp1[330]</label>"
                       "<input style='width:55%;' type='range' id='idRange11' oninput=connection.send('"RANGE_PREAMBLE"'+this.id+'='+this.value);>"
                       "<label id='idRangeLab12' style='float:left;width: 65px;' for='idRange12'>sp1[330]</label>"
                       "<input style='width:55%;' type='range' id='idRange12' oninput=connection.send('"RANGE_PREAMBLE"'+this.id+'='+this.value);>"

                       "<label id='idRangeLab13' style='float:left;width: 65px;' for='idRange13'>minalert[330]</label>"
                       "<input style='width:55%;' type='range' id='idRange13' oninput=connection.send('"RANGE_PREAMBLE"'+this.id+'='+this.value);>"

                       "<label id='idRangeLab14' style='float:left;width: 65px;' for='idRange14'>maxalert[330]</label>"
                       "<input style='width:55%;' type='range' id='idRange14' oninput=connection.send('"RANGE_PREAMBLE"'+this.id+'='+this.value);>"


                       "</div>"

                       "<div style='clear: both;'></div>"
                       "</div>"

                       "</div>"));

  END_PAGE_CUSTOM;
  END_PAGE;


  server.sendContent(F(
                       "<script>"

                       "const sCX=900;" //X axis Size of Canvas
                       "const sCY=150;" //Y axis size of Canvas
                       "var mPV=10;" //Maximum Value of the PV or temperature
                       "var minPV=5;" //min Valie of the PV
                       "const maxYpwm=255;"
                       "const minYpwm=0;"
                       "var PVS=sCY/(mPV-minPV);" //PVS=scaler for the pr Process Varaiable Scaler
                       "const MVS=sCY/(maxYpwm-minYpwm);")); //MVS Control Variable Scaler


  server.sendContent(F("var arrpv=[];"
                       "var arrhum=[];"
                       "var arrmv=[];"
                       "var arrpterm=[];"
                       "var arriterm=[];"
                       "var arrdterm=[];"
                       "var units='C';"
                       "Number.prototype.map = function (in_min, in_max, out_min, out_max) {return ((this - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);};"  //Need the last semicolon not sure why

                       "function showCoords(event) {var canvas=document.getElementById(event.currentTarget.id);var ctx = canvas.getContext('2d');var rect = event.currentTarget.getBoundingClientRect();var x = event.clientX-rect.left;var y = event.clientY-rect.top;if(event.currentTarget.id == 'mypvCanvas'){var Val = y.map(sCY,0,minPV,mPV)}else{var Val = y.map(sCY,0,0,255)};if(units == 'F'){Val = Val * 1.8 + 32;} ctx.strokeText(Val.toFixed(2), x, y);}"


                      ));

  //many of the page elements are generated as the page is rendered.  This reduced the size of the page to send.  This is not so important now
  //"var aFO=['ticks','PV','err','pterm','iterm','dterm','MV','millis','hum','state','timer','errHum'];"));
  server.sendContent(F("var PIDformatString='"));
  server.sendContent(PIDformatString);
  server.sendContent(F("';"));
  server.sendContent(F(
                       "var PIDformatElements = PIDformatString.slice(2).split(',');"
                       "var element=document.getElementById('PIDHeadings');"
                       "for (idx=0;idx<PIDformatElements.length;idx++)"
                       "{"
                       "tf=document.createElement('input');"
                       "element.appendChild(tf);"
                       "tf.value=PIDformatElements[idx].split('=')[0];"
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
  server.sendContent(F("var cpdata=["));
  for (int i = 0; i < cpsize; i++)
  {
    //server.sendContent(String(AppConfig.PID[cpmap[i]]));
    server.sendContent(String(AppConfig.PID[i]));
    server.sendContent(F(","));
  }
  server.sendContent(F("];"));
  server.sendContent(F("var cptag=["));
  for (int i = 0; i < cpsize; i++)
  {
    server.sendContent(F("'"));
    //server.sendContent((cptag[cpmap[i]]));
    server.sendContent((cptag[i]));
    server.sendContent(F("',"));
  }
  server.sendContent(F("];"));
  server.sendContent(F("var cpmax=["));
  for (int i = 0; i < cpsize; i++)
  {
    //server.sendContent((cpmax[cpmap[i]]));
    server.sendContent((cpmax[i]));
    server.sendContent(F(","));
  }
  server.sendContent(F("];"));
  server.sendContent(F("var cpmin=["));
  for (int i = 0; i < cpsize; i++)
  {
    //server.sendContent((cpmin[cpmap[i]]));
    server.sendContent((cpmin[i]));
    server.sendContent(F(","));
  }
  server.sendContent(F("];"));
  server.sendContent(F("var cpstep=["));
  for (int i = 0; i < cpsize; i++)
  {
    //server.sendContent((cpstep[cpmap[i]]));
    server.sendContent((cpstep[i]));
    server.sendContent(F(","));
  }
  server.sendContent(F("];"));

  server.sendContent(F("var cpstep=["));
  for (int i = 0; i < cpsize; i++)
  {
    //server.sendContent((cpstep[cpmap[i]]));
    server.sendContent((cpstep[i]));
    server.sendContent(F(","));
  }
  server.sendContent(F("];"));


  server.sendContent(F(
                       //Configure the sliders with thier default values
                       "for(idx=0;idx<cptag.length;idx++)"
                       "{"
                       "var zeropad = '';"
                       "if(idx<10){zeropad = '0';}"
                       "var rangeid = 'idRange' + zeropad + idx;"
                       "var rangelabid = 'idRangeLab' + zeropad + idx;"
                       "document.getElementById(rangeid).max = parseFloat(cpmax[idx]);"
                       "document.getElementById(rangeid).min = parseFloat(cpmin[idx]);"
                       "document.getElementById(rangeid).step = parseFloat(cpstep[idx]);"
                       "document.getElementById(rangeid).value = parseFloat(cpdata[idx]);"
                       "document.getElementById(rangelabid).innerText =  cptag[idx] + '[' + parseFloat(cpdata[idx]) + ']';"
                       "}"

                       "var connection=new WebSocket('ws://'+location.hostname+':81/',['arduino']);"

                       "connection.onopen=function(){connection.send('Connect '+new Date());};"
                       "connection.onerror=function(error){console.log('WebSocket Error ',error);};"
                       "connection.onclose=function(event){var readystate = connection.readyState;"
                       "console.log('readstate:',readystate);"

                       "if(readystate == 3){location.reload(1);}"
                       "};"

                       //incomming data from esp8266
                       "connection.onmessage=function(e){console.log('Server:',e.data);"
                       "var msg=e.data;"
                       "if(msg!='Connected'){"

                       "if(msg=='"HEATER_ON_MSG"'){circle1.style.fill='red';}"
                       "else if(msg=='"HEATER_OFF_MSG"'){circle1.style.fill='DarkRed';}"

                       "else if(msg=='"PUMP_ON_MSG"'){circle2.style.fill='LightBlue';}"
                       "else if(msg=='"PUMP_OFF_MSG"'){circle2.style.fill='DarkBlue';}"

                       "else if(msg=='"HEATER_HIGH_MSG"'){circle1_text.innerHTML='H';}"
                       "else if(msg=='"HEATER_LOW_MSG"'){circle1_text.innerHTML='L';}"

                       "else if(msg=='"PUMP_HIGH_MSG"'){circle2_text.innerHTML='H';}"
                       "else if(msg=='"PUMP_LOW_MSG"'){circle2_text.innerHTML='L';}"


                       //process incomming slider updates
                       "else if(msg.indexOf('"CONTROL_PARAMS_PREAMBLE"') == 0){"
                       "msg = msg.slice(2);"

                       "var arrmsg = msg.split(',');"
                       "for(idx=0;idx<arrmsg.length - 1;idx++)"
                       "{"
                       "var zeropad = '';"
                       "if(idx<10){zeropad = '0';}"
                       "var rangeid = 'idRange' + zeropad + idx;"
                       "var rangelabid = 'idRangeLab' + zeropad + idx;"
                       "document.getElementById(rangeid).value = parseFloat(arrmsg[idx]);"
                       "var activeSliderColor = '#0f0f0f';"
                       "if(idx == arrmsg[arrmsg.length-1]){activeSliderColor = '#c51818';}"
                       "document.getElementById(rangelabid).style.color = activeSliderColor;"
                       "if(units == 'F' && (cptag[idx] == 'sp1' || cptag[idx] == 'sp2')){"
                       "document.getElementById(rangelabid).innerHTML = cptag[idx]  + '[' + Math.round(parseFloat(arrmsg[idx])* 1.8 + 32) +']';"
                       "}else{"
                       "document.getElementById(rangelabid).innerHTML = cptag[idx]  + '[' + (parseFloat(arrmsg[idx])) +']';"
                       "cpdata[idx] = parseFloat(arrmsg[idx]);"
                       "document.getElementById(rangelabid).style.color = activeSliderColor;"
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

                       //process incoming key-value pairs
                       "else if(msg.indexOf('"KEY_PAIR_PREAMBLE"') == 0){"
                       "msg = msg.slice(2);"
                       "var pairs=msg.split(',');"
                       "var keypairMap = new Map();"// – creates the map.
                       "for(var idx =0;idx < pairs.length;idx++)"
                       "{"
                       "var keyvalue = pairs[idx].split('=');"
                       "document.getElementById('id'+keyvalue[0]).value=parseFloat(keyvalue[1]);"
                       "if(keyvalue[0] == 'state'){document.getElementById('id'+keyvalue[0]).value=keyvalue[1]};"
                       "if(keyvalue[0] == 'clock' || keyvalue[0] == 'timer'){document.getElementById('id'+keyvalue[0]).value"
                       " = Math.floor(parseFloat(keyvalue[1]) / (24*60*60*1000)) + ':' + new Date(parseFloat(keyvalue[1])).toISOString().substr(11, 8)};"
                       "if(keyvalue[0] == 'PV'){"
                       "var pv_celsius = keyvalue[1];"
                       "if(units == 'F'){document.getElementById('idPV').value=Math.round(keyvalue[1]*1.8 + 32);}}"
                       "}"

                       "var canvas=document.getElementById('mypvCanvas');"

                       "var ctx=canvas.getContext('2d');"
                       "ctx.clearRect(0,0,sCX,sCY);"
                       "ctx.beginPath();"
                       "ctx.fillStyle='black';" //lightgrey';"
                       "ctx.fillRect(0,0,sCX,sCY);"
                       "ctx.beginPath();"

                       "ctx.font = 'Courier New';"

                       "if(arrpv.length >sCX){arrpv.push(arrpv.shift());arrpv.pop();}"
                       "arrpv.push(pv_celsius);"

                       "mPV = Math.max(...arrpv)+0.2;"
                       "minPV = Math.min(...arrpv)-0.2;"
                       "PVS=sCY/(mPV-minPV);"
                       "var stage1=(mPV-idRange00.value)*PVS;"
                       "var stage2=(mPV-idRange01.value)*PVS;"

                       "ctx.rect(0,stage1,sCX,stage2-stage1);"
                       "ctx.stroke();"
                       "ctx.beginPath();"

                       "ctx.strokeStyle='red';"
                       "for(cnt=0;cnt<arrpv.length;cnt++){"
                       "ctx.beginPath();"
                       "ctx.moveTo(cnt,sCY-((arrpv[cnt]-minPV)*PVS));"
                       "ctx.lineTo((cnt+1),sCY-((arrpv[cnt+1]-minPV)*PVS));"
                       "ctx.stroke();"
                       "}"

                       //hum (index=8) Chart humidty
                       "if(arrhum.length >sCX){arrhum.push(arrhum.shift());arrhum.pop();}"
                       "arrhum.push(document.getElementById('idhum').value);"

                       "ctx.strokeStyle='lightblue';"
                       "for(cnt=0;cnt<arrhum.length;cnt++){"
                       "ctx.beginPath();"
                       "ctx.moveTo(cnt,sCY-((sCY/100) * arrhum[cnt]));"
                       "ctx.lineTo((cnt+1),sCY-((sCY/100) * arrhum[cnt+1]));"
                       "ctx.stroke();"
                       "}"

                       //draw tick marks for PV
                       "ctx.strokeStyle = 'green';"
                       "var j=0;"
                       "for(i =0;i< sCX;i+=60){"
                       "ctx.beginPath();"
                       "ctx.moveTo(i,0);"
                       "ctx.lineTo(i,sCY);"
                       "ctx.stroke();"
                       "ctx.strokeText(j.toFixed(1)+'min',i+1,sCY-2);"
                       "j=j+cpdata[cptag.indexOf('tc')]/1;"
                       "}"

                       //draw min max ticks
                       "if(units == 'F'){"
                       "var dispval = minPV * 1.8 + 32;"
                       "ctx.strokeText(dispval.toFixed(1),0,sCY-10);"
                       "var dispval = mPV * 1.8 + 32;"
                       "ctx.strokeText(dispval.toFixed(1),0,10);"
                       "} else {"
                       "ctx.strokeText(minPV.toFixed(1),0,sCY-10);"
                       "ctx.strokeText(mPV.toFixed(1),0,10);"
                       "}"

                       //Second canvas update
                       //MV (index=6) Chart pwm aka mv Manipulated variable

                       "var canvas=document.getElementById('mymvCanvas');"

                       "var ctx=canvas.getContext('2d');"
                       "ctx.clearRect(0,0,sCX,sCY);"
                       "ctx.beginPath();"
                       "ctx.beginPath();"
                       "ctx.fillStyle='black';"
                       "ctx.fillRect(0,0,sCX,sCY);"
                       "ctx.beginPath();"

                       "if(arrmv.length >sCX){arrmv.push(arrmv.shift());arrmv.pop();}"
                       "arrmv.push(document.getElementById('idMV').value);"

                       "for(cnt=0;cnt<arrmv.length;cnt++){"
                       "ctx.beginPath();"
                       "ctx.moveTo(cnt,sCY-((arrmv[cnt]-minYpwm)*MVS));"
                       "ctx.lineTo((cnt+1),sCY-((arrmv[cnt+1]-minYpwm)*MVS));"
                       "ctx.strokeStyle='red';"
                       "ctx.stroke();"
                       "}"

                       //pterm (index=3) Chart proportional error

                       "if(arrpterm.length >sCX){arrpterm.push(arrpterm.shift());arrpterm.pop();}"
                       "arrpterm.push(document.getElementById('idpterm').value);"

                       "for(cnt=0;cnt<arrpterm.length;cnt++){"
                       "ctx.beginPath();"
                       "ctx.moveTo(cnt,sCY-((arrpterm[cnt]-minYpwm+125)*MVS));"
                       "ctx.lineTo((cnt+1),sCY-((arrpterm[cnt+1]-minYpwm+125)*MVS));"
                       "ctx.strokeStyle='green';"
                       "ctx.stroke();"
                       "}"

                       //iTerm (index=4) Chart integral error

                       "if(arriterm.length >sCX){arriterm.push(arriterm.shift());arriterm.pop();}"
                       "arriterm.push(document.getElementById('iditerm').value);"

                       "for(cnt=0;cnt<arriterm.length;cnt++){"
                       "ctx.beginPath();"
                       "ctx.moveTo(cnt,sCY-((arriterm[cnt]-minYpwm)*MVS));"
                       "ctx.lineTo((cnt+1),sCY-((arriterm[cnt+1]-minYpwm)*MVS));"
                       "ctx.strokeStyle='blue';"
                       "ctx.stroke();"
                       "}"

                       //dterm (index=5) Chart differential error

                       "if(arrdterm.length >sCX){arrdterm.push(arrdterm.shift());arrdterm.pop();}"
                       "arrdterm.push(document.getElementById('iddterm').value);"

                       "for(cnt=0;cnt<arrdterm.length;cnt++){"
                       "ctx.beginPath();"
                       "ctx.moveTo(cnt,sCY-((arrdterm[cnt]-minYpwm+125)*MVS));"
                       "ctx.lineTo((cnt+1),sCY-((arrdterm[cnt+1]-minYpwm+125)*MVS));"
                       "ctx.strokeStyle='white';"
                       "ctx.stroke();"
                       "}"

                       //draw tick marks
                       "ctx.strokeStyle = 'green';"
                       "var j=0;"
                       "for(i =0;i< sCX;i+=60){"
                       "ctx.beginPath();"
                       "ctx.moveTo(i,0);"
                       "ctx.lineTo(i,sCY);"
                       "ctx.stroke();"
                       "ctx.strokeText(j.toFixed(1)+'min',i+1,sCY-2);"
                       "j=j+cpdata[cptag.indexOf('tc')]/1;"
                       "}" //end of for above
                       "}" //end of else
                       "}" //end of if connected
                       "};" //end of onmessage
                       "</script>"));
}
