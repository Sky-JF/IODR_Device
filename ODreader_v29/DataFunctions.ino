//write blank value to EEPROM
//8 2-byte values get stored
//address 0 and 1 store the first blank value
//address 2 and 3 store the second blank value
void writeBlankToEEPROM (int tubeIndex) {
  Serial << "******************* writeBlankToEEPROM() ******************";
  Serial.print("\nWriting blank value for tube ");
  Serial.print(tubeIndex);
  Serial.println(" from EEPROM");
  int value = blankValue[tubeIndex];
  byte h = highByte(value);
  byte l = lowByte(value);
  //EEPROM.write(tubeIndex*2, h);  //store the high byte in the first address %%%
  //EEPROM.write(tubeIndex*2+1, l); //store the low byte in the second address %%%
  // perhaps add EEPROM.commit(); if flashstorage
}



//read blank value from EEPROM
void readBlankFromEEPROM (int tubeIndex) {
  Serial << "************************** readBlankFromEEPROM() **************************************";
  Serial.print("\nReading blank value for tube ");
  Serial.print(tubeIndex);
  Serial.println(" from EEPROM");
  word value;
  //byte h = EEPROM.read(tubeIndex*2); //read low byte from EEPROM %%%
  //byte l = EEPROM.read(tubeIndex*2+1); //read high byte from EEPROM %%%
  //value = word(h,l); %%%
  blankValue[tubeIndex] = (int)value; //cast value to an integer and store the result in the blankValue array
}



//subroutine for checking the timing of a portion of a loop
void checkTiming() {
  Serial.print("***checkTiming()*** loop time=");
  Serial.print(millis()-lastLoopTime);
  lastLoopTime = millis();
  Serial.println();
}



// check the state of the reset buttons. keep track of which button was most recently pressed.  
// if a button is pressed and held for a certain amount of time (>2 seconds), then reset the blank value for that well
void checkBlankButtons() {
  //Serial << "\n$$$$$$$$$$$ checkBlankButtons() $$$$$$$$$$$$$$$$$";
  for (int i=0; i<numTubes; i++){
    //in v2 of the IODR shield, I accidentally wired the reset buttons to give digital 0 when pressed instead of 
    //digital 1 (as in previous version).  So I added a "!" before the all 3 digitalRead commands to fix this
    blankButtonState[i] += !digitalRead(blankButtonPin[i]); 
    
    // if a button was pressed, keep track of which one
    // if multiple buttons were pressed, remember the highest numbered one
    if (!digitalRead(blankButtonPin[i]) == 1){
      lastButtonPressed = i; 
      Serial << "\nlastButtonPressed=" <<lastButtonPressed;
      displayTubeSummary = false; // switch the OLED display to show data for an individual tube instead of the summary
                                  // note, this will reset back to summary view after the next time data is uploaded to the cloud
    }
    
    // if a button was not held down during this cycle, reset it's counter
    // i.e. reset button 2 was held down for 2 cycles, then it was released
    // now it's held down for 2 cycles again.  it shouldn't reset the blank value
    if (!digitalRead(blankButtonPin[i]) == 0){
      blankButtonState[i] = 0;
    }
    
    // if a button has been held down for more than NUM_CYCLES_TO_RESET cycles, reset that tube
    if (blankButtonState[i] > NUM_CYCLES_TO_RESET) {
      blankValue[i] = (int)lightIn[i]; // reset blankValue for tube i
      Serial << "blankButtonState[" << i << "]=" << blankValue[i];
      //writeBlankToEEPROM(i);// write value to EEPROM, this saves the value even if the arduino is reset; giga R1 does not have EEPROM compatibility %%%
      displayTubeReset(i); // write a message to the serial LCD saying the tube was reset
      blankButtonState[i] = 0; // reset the blank button state
    }
  }
  //blankButtonStatusDisplay();
  
  //if the calibration button was pressed, switch the display back to summary mode
  if (!digitalRead(calibrationPin)){
  displayTubeSummary = true;
  Serial << "\n\t\t calibrationPin=" << digitalRead(calibrationPin);
  }
}



// display the state of each pushbutton used to blank the tube reader
void blankButtonStatusDisplay() {
  //Serial << "*************** blankButtonStatusDisplay() ***********************";
  Serial.print("Blank Button State: ");
  for (int i=0; i<numTubes; i++){
     Serial.print(blankButtonState[i]);
     Serial.print(" ");
  }
  Serial.print(" lastButtonPressed=");
  Serial.print(lastButtonPressed);
  Serial.println();
}


//print the state of the light and temperature sensors to the serial monitor for debugging purposes
void serialPrintState() {  
  Serial << "\n**************** serialPrintState() ******************";
  Serial <<"\nLEDoffReading[]\t LEDonReading[]\t lightIn[]\t ODvalue[]\t blankValue";
  for (int i=0; i<numTubes; i++){
     Serial <<"\nTube[" << i << "]:" ;
     Serial.print(LEDoffReading[i]);
     Serial.print("\t");
     Serial.print(LEDonReading[i]);
     Serial.print("\t");
     Serial.print(lightIn[i]); //raw value of light sensor
     Serial.print("\t");
     Serial.print(ODvalue[i]); //calculated OD value
     Serial.print("\t");
     Serial.print(blankValue[i]);
     Serial.print("\t");
  }
  // display temperature   
  Serial.print("\nTemp: ");
  Serial.print(temperature);
  Serial.println();
}



//display a message on the LCD saying that one tube blank value was reset
void displayTubeReset(int tubeNum) {
  Serial2 << gloClear << "Tube: " << tubeNum + 1; //tubNum index starts at 0, so increment by one for human-readable display
  Serial2 << gloReturn << "Was reset" ;
  
  Serial.print("displayTubeReset:");
  //Serial.println(resetTopLine);
  delay(1500); //show the message for 1.5 seconds
}




// Summary Display
// display current OD for all tubes on OLED display
void displayTubeStatusSummary() {
  //Serial.println("*************************************");
  //Serial.println("displayTubeStatusSummary()");
  char displayLine[4][17] = {
       "1:     5:     T ",
       "2:     6:     e ",
       "3:     7:     m ",
       "4:     8:     p "};  
  char odval[5] = "    ";
                           
  //add in OD values to first column
  for (int i=0; i<numTubes/2; i++){  
    //loop through each character in odval and add it to the displayLine char array
    //add odval in first position (i.e. tubes 1, 2, 3 and 4)
    //dtostrf(ODvalue[i], 4, 2, odval); //%!%% not in GIGA
    snprintf(odval, sizeof(odval), "%.2f", ODvalue[i]);
    //Serial << "   odval[" << i << "]=" << odval <<" length=" << sizeof(odval);
    
    for (int j = 0; j<sizeof(odval)-1; j++){ //sizeof(odval)-1 is to avoid including the "end of string" character
      displayLine[i][j+2] = odval[j];
      //Serial << "        displayLine["<<i<<"]["<<j+2<<"]="<<odval[j];
    }
    //add odval in second position (i.e. tubes 5, 6, 7 and 8)
    //dtostrf(ODvalue[i+4], 4, 2, odval);  //%!%% NOT in GIGA
    snprintf(odval, sizeof(odval), "%.2f", ODvalue[i+4]);
    //Serial << "   odval_2_[" << i << "]=" << odval <<" length=" << sizeof(odval);
    //Serial.println();
    for (int j = 0; j<sizeof(odval)-1; j++){
      displayLine[i][j+9] = odval[j];
    }
  }

  //add temperature value
  char tempStr[5] = "    ";
  snprintf(tempStr, sizeof(tempStr), "%.2f", temperature);
  //dtostrf(temperature, 4,2, tempStr); %!%% NOT in GIGA
  displayLine[0][15] = tempStr[0];
  displayLine[1][15] = tempStr[1];
  displayLine[2][15] = tempStr[2];
  displayLine[3][15] = tempStr[3];

  /*  
  Serial.println("\n*************** displayTubeStatusSummary() *****************");
  for (int i=0; i<4; i++){
    Serial.println(displayLine[i]);
  }
  */  
   //write the result to the OLED display
   Serial2 << gloHome;
   for (int i = 0; i<4; i++){
     Serial2 << displayLine[i]; //<< gloReturn;
     //Serial.println(displayLine[i]);
   }
  
}




// display current OD, raw light sensor value and blank value on serial LCD
void displayTubeStatus(int tubeNum) {
  //Serial << "\n************** displayTubeStatus() ******************* tubeNum=" << tubeNum;

  char bval[15]; //note: if these character arrays are too short, they can overwrite each other
  char odval[15];
  char ltin[15];
  char topLine[] = "Tube:   Blk=    ";
  char botLine[] = "OD=     Raw=    ";
  
  //convert variables to strings of the appropriate length
  snprintf(bval, sizeof(bval), "%.2f", blankValue[tubeNum]);
  //dtostrf(blankValue[tubeNum], 4, 0, bval); //%!%% NOT in GIGA
  //Serial.println();
  //dtostrf(LEDonReading[lastButtonPressed], 4, 0, ltin); //use LEDonReading for the raw value.  
                                                          //We want to be able to see how close we are to saturating the detector when we're adjusting the potentiometer.
                                                          //Note that the raw value will show fluctuations due to ambient light, but the lightIn value (i.e. abient light subtracted)
                                                          //is what is being used to calculate the OD value
  //dtostrf(lightIn[tubeNum], 4, 0, ltin);  %!%% NOT in GIGA      //use lightIn for raw value
  snprintf(ltin, sizeof(ltin), "%.2f", lightIn[tubeNum]);
  //  Serial <<"       ltin=" <<ltin;
  //Serial.println();
                                                          //not sure which I prefer, lightIn or LEDonReading, for raw value
                                                          //using lightIn for now
  //dtostrf(ODvalue[tubeNum], 4, 2, odval); %!%% NOT IN giga
  snprintf(odval, sizeof(odval), "%.2f", ODvalue[tubeNum]);
  // Serial <<"       odval=" <<odval;
  //Serial.println();

  //set tube number
  topLine[5] = tubeNum + 49; //convert integer to ascii by adding 48, add one more to shift index by 1
  
  //set blank, OD and raw numbers
  for (int i=0; i < 4; i++){
     topLine[12+i] = bval[i]; //set blank value
     //Serial << "\n\t\t\t\t bval=" <<bval <<" bval[" <<i<< "]=" << bval[i] << "   ^^^ topLine=" << topLine;
     botLine[12+i] = ltin[i]; //set raw value
     botLine[3+i] = odval[i]; //set OD value
  }

  //send OLED display output to serial port
  //Serial << "\ntopLine=" << topLine;
  //Serial << "\nbotLine=" << botLine;
  
  //send OLED display output to OLED display
  Serial2 << gloHome << topLine << gloSetCursor(0,1) << botLine; //output compiled lines to OLED display
  Serial2 << "                                "; //clear the bottom two lines of the display
}


//send data to Thingspeak
//OD_CHANNEL_ID
//OD_API_KEY
//TEMP_CHANNEL_ID
//TEMP_API_KEY
void uploadDataToThingspeak() {
  // send information to OLED display
  Serial2 << gloClear << "Sending data to Thingspeak...";
  
  //loop through datastreams (except temperature) and update data
  String path = "/update?api_key=" + String(OD_API_KEY);
  for (int i = 0; i < numTubes; i++) {
    path += "&field" + String(i+1) + "=" + String(ODvalue[i]); //set each field one-by-one, thingspeak channels are indexed from 1 
  }
  Serial.println("Server: " + String(server));
  Serial.println("Requesting: " + path);
  
  // send OD data to thingspeak
  Serial.println("sending OD data to thingspeak");
  client.get(path);

  // Server response from OD upload attempt
  int odStatusCode = client.responseStatusCode();
  Serial.print("OD upload status code: ");
  Serial.println(odStatusCode);
  Serial2 << gloReturn << "OD code: " + odStatusCode; // send response to OLED display
  //wdt_reset(); //%%%
  //watchdog.kick(); 

  //send temperature data to thingspeak
  Serial.println("sending temperature data to thingspeak");
  path = "/update?api_key=" + String(TEMP_API_KEY);
  path += "&field" + String(TEMP_EXT_CHANNEL) + "=" + String(temperature); //set each field one-by-one, thingspeak channels are indexed from 1 
  Serial.println("Server: " + String(server));
  Serial.println("Requesting: " + path);

  // send temp data to thingspeak
  Serial.println("sending temperature data to thingspeak");
  client.get(path);

  // Server response from temperature upload attempt
  int tempStatusCode = client.responseStatusCode();
  Serial.print("Temp upload status code: ");
  Serial.println(tempStatusCode);
  Serial2 << gloReturn << "Temp code: " + tempStatusCode; // send response to OLED display
  delay(1500); // enough time to read the display
  //wdt_reset(); //%%%
  //watchdog.kick();

  if (odStatusCode == 200){ // 200 is the server response from Thingspeak indicating success
    numFailedUploads = 0;
    //digitalWrite(wdTimer, HIGH); // reset the watchdog timer
    Serial.println("numFailedUploads reset");
    delay(500);
    //digitalWrite(wdTimer, LOW);
  }
  else{
    numFailedUploads += 1;
    Serial2 << gloClear << "Failed uploads = " << numFailedUploads;
    Serial.print("Failed uploads = ");
    Serial.println(numFailedUploads);
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("ATTENTION: Wifi was not connected on this failed data upload");
    }
    // try to reset the network connection and re-connect to thingspeak
    //Ethernet.begin(mac, ip, dnsServer, gateway, subnet); // try to connect to the network with fixed IP address; commented out for giga
    WiFi.disconnect(); // disconnect from wifi
    connectToWifi(); // try to reconnect
    delay(1500); // allow time to read the display
  }

  // this if block should be unnecessary, since the watchdog timer will reset after 5 minutes with no successful upload (i.e. about 5 failed tries)
  if (numFailedUploads > MAX_FAILED_UPLOADS){
    Serial2 << gloClear << "Upload error: waiting to reset..."; 
    //digitalWrite(wdTimer, LOW); // turn off the upload light to allow the watchdog timer to reset %%% not using wdTimer
    delay(400000); // delay 6 minutes, should trigger watchdog timer to reset
  }
}

void connectToWifi() {
  //check giga wifi module
  if (WiFi.status() == WL_NO_MODULE) { 
    Serial.println("Communication with WiFi module failed!");
    Serial2 << gloClear << "WiFi module failed";
    // don't continue
    while (true);
  }

  //connect to wifi
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    WiFi.begin(ssid, pass);
    // wait 3 seconds for connection:
    // implement reset if unable to connect after 15 attempts ??? %%% 
    delay(3000);
  }
  Serial.println("Connected to wifi");
  serialPrintWifiStatus();
}

void readLightSensors() {
  // clear the lightIn value
  for (int i = 0; i < numTubes; i++){
   lightIn[i] = 0;
   ODvalue[i] = 0;
   //checkBlankButtons(); removed this in version 20 of the program to see if I can fix the software "freezing" bug.
  }
  // start accumulating lightIn values
  for (int j = 0; j<pointsToAverage; j++){ 
    // read light sensor values with the LED off
    // this measures ambient light levels, which will later get subtracted from the reading
    digitalWrite(ledPin, LOW); //turn off LEDs
    delay(10); //it takes ~1ms for the light senor reading to stabilize after the LED has been turned off
    for (int i = 0; i < numTubes; i++){ //
      LEDoffReading[i] = analogRead(lightInPin[i]);      
    }
    // read light sensor values with the LED on
    digitalWrite(ledPin, HIGH); //turn on LEDs
    delay(10); //it takes ~1ms for the light senor reading to stabilize after the LED has been turned on
    for (int i = 0; i < numTubes; i++){ // 
      LEDonReading[i] = analogRead(lightInPin[i]);      
    }   
    // calculate the difference and add it to lightIn
    for (int i = 0; i < numTubes; i++){ // 
      lightIn[i] += (LEDonReading[i] - LEDoffReading[i]);
    } 
  }
  // divide lightIn by pointsToAverage to get the average value
  for (int i = 0; i < numTubes; i++){ //
      lightIn[i] = lightIn[i]/pointsToAverage;
      ODvalue[i] = -(log10(lightIn[i]/blankValue[i]));
  }
  // turn the lights off when you're done
  digitalWrite(ledPin, LOW); //turn off LEDs
}


// ******************* TEMPERATURE SENSOR SUBROUTINES **********************************
//initialize DS18B20 temperature sensor
boolean initTemp() {
  Serial.println("^^^initTemp()^^^");
  boolean deviceFound;
  int numTries = 4;
  
  //find the first device on the bus (i.e. at index = 0)
  for (int i = 0; i < numTries; i++){
    deviceFound = sensors.getAddress(tempSensor, 0);
    if (deviceFound){
      break; // sensor was found
    }
    else{
      Serial.println("device not found, searching again...");
      delay(1000); // wait 1000 ms and try again
    }
  }
  
  // set the resolution to 11 bit - Valid values are 9, 10, or 11 bit.
  sensors.setResolution(tempSensor, 11);
  // confirm that we set that resolution by asking the DS18B20 to repeat it back
  Serial.print("Sensor Resolution: ");
  Serial.println(sensors.getResolution(tempSensor), DEC); 
  Serial.println();
  // then check the temperature
  getTemp();

 
  return deviceFound; //if a device was found and there were no errors
}



//returns the temperature from one DS18S20 in DEG Celsius
//only works with one DS18S20
//note, this was rewritten in version 23
float getTemp() {
  float tempC;
  //Serial.println("^^^getTemp()^^^");
  if (temperatureDeviceFound){
      sensors.requestTemperaturesByAddress(tempSensor); // Send the command to get temperatures
     
      // Get the temperature that you told the sensor to measure
      tempC = sensors.getTempC(tempSensor);
      
      //Serial.print("Temp C: ");
      //Serial.print(tempC,4);  // The four just increases the resolution that is printed
      // The Dallas Temperature Control Libray has a conversion function... we'll use it
      //Serial.println(DallasTemperature::toFahrenheit(tempC),4);

      return tempC;
   }
   return -1000; //error to return if no temperature device was found
}

void serialPrintWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
