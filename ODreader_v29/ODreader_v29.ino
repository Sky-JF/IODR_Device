/*
Pin assignments:
0: used by ethernet shield
1: used by ethernet shield
2: yellow LED
3: external watchdog timer
4: ethernet CS for SD card (not used in this program)
5: previously used for entering calibration mode (note that the calibration mode hasn't been implemented in this version of the software)
7: unused (previous version used this pin for wifi shiled)
8: internal temperature sensor
9: transistor that controls all 8 LEDs (note, this was previously pin 7, but moved to pin 9 since the wifi shield needs to use pin 7)
10: I think this is used for the ethernet shield, output is always high
18: TX for lcd display
44: external temperature sensor

Ethernet shield connected to SPI bus (using SPI header instead of dedicated pins)
53: SS for ethernet controller
51: ethernet MOSI
50: ethernet MISO
52: ethernet SCK

53: necessary to set this pin as output to allow ethernet shield to work
20-34 (even numbered pins): blank reset buttons
A8: pin 89, light sensor for tube 8
A9: pin 88, light sensor for tube 7
A10: pin 87, light sensor for tube 6
A11: pin 86, light sensor for tube 5
A12: pin 85, light sensor for tube 4
A13: pin 84, light sensor for tube 3
A14: pin 83, light sensor for tube 2
A15: pin 82, light sensor for tube 1
*/

//software version
#define VERSION 29

// hardware ID
#define IODR_ID 4

// Libraries
#include <stdlib.h>
#include <SPI.h> //communication with wifi or ethernet shield
//#include <avr/wdt.h> //for watchdog timer; not included in giga boards %%%
#include <OneWire.h> //for temperature sensor
#include <DallasTemperature.h> //for temperature sensor
#include <Math.h>
//#include <SoftwareSerial.h> //for serial LCD communication; Not usable with Giga 
//#include "ThingSpeak.h" %%%
//#include <EEPROM.h> //used to store blank values so they persist between resets; used with Arduino mega boards %%%
// #include <FlashStorage.h> //used to store blank values so they persist between resets; used with Arduino giga boards %%%
// #include <FlashAsEEPROM.h> // might not work %%%
#include <gloSerialOLED.h> //for OLED character display 
//gloStreamingTemplate //allows carat notation for OLED display
#include "mbed.h" //WDT test for giga %%%
mbed::Watchdog &watchdog = mbed::Watchdog::get_instance();



/* ****************** Local Libraries ******************* */
//#include "SensorFunctions.h"

//Parameters for IODR with Arduino Giga R1; change ID as necessary; this block assumes the ID is 1
#if IODR_ID == 4
  #include <WiFi.h> //for Arduino Giga board wifi connection
  template<class T> inline Print &operator <<(Print &obj, T arg) { obj.print(arg); return obj; }

  // Login details for Wifi Network
  char ssid[] = "Dartmouth Public";        // your network SSID (name)
  char pass[] = "";    // your network password (use for WPA, or use as key for WEP)

  int status = WL_IDLE_STATUS;
  WiFiClient client; //used by thingspeak

  //definitions might need to be modified
  #define OD_CHANNEL_ID 405675
  #define OD_API_KEY "J1AW4IF7557WHYAX" //write API key
  #define TEMP_CHANNEL_ID 890567
  #define TEMP_API_KEY "C509JHJCZNRGDNRE" //write API key
  #define TEMP_INT_CHANNEL 1 //channel number for IODR #1 internal temperature data
  #define TEMP_EXT_CHANNEL 2 //channel number for IODR #1 external temperature data
  #define TEMP_SENSE_PIN 44

  //double check these depending on network being connected 
  //byte mac[] = { 0x0E, 0x0E, 0x02, 0x02, 0x02, 0xA8 }; //mac address for IODR #4 %%%
#endif

// Parameters for IODR #1
#if IODR_ID == 1
  #include <Ethernet2.h> //ethernet shield W5500 chip

  // ThingSpeak parameters for IODR #1
  #define OD_CHANNEL_ID 2926522
  #define OD_API_KEY "4ZLMGG2B59CUGMNV" //write API key
  #define TEMP_CHANNEL_ID 2926522
  #define TEMP_API_KEY "4ZLMGG2B59CUGMNV" //write API key
  #define TEMP_INT_CHANNEL 1 //channel number for IODR #1 internal temperature data
  #define TEMP_EXT_CHANNEL 2 //channel number for IODR #1 external temperature data
  #define TEMP_SENSE_PIN 44

  // Network parameters for IODR #1
  byte mac[] = { 0xA8, 0x61, 0x0A, 0xAE, 0x3F, 0xBC }; //mac address for IODR #1 new ethernet shield W5500
  byte ip[] =      { 129, 170, 64, 61 }; //the IP address for IODR #1
#endif


// Parameters for IODR #2
#if IODR_ID == 2
  #include <Ethernet2.h> //ethernet shield W5500 chip
  
  // ThingSpeak parameters for IODR #2
  #define OD_CHANNEL_ID 441742
  #define OD_API_KEY "P212KDUXZPL5BAFA" //write API key
  #define TEMP_CHANNEL_ID 890567
  #define TEMP_API_KEY "C509JHJCZNRGDNRE" //write API key
  #define TEMP_INT_CHANNEL 3 //channel number for IODR #2 internal temperature data
  #define TEMP_EXT_CHANNEL 4 //channel number for IODR #2 external temperature data
  #define TEMP_SENSE_PIN 44

  // Network parameters for IODR #2
  byte mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0xA7, 0x42 }; //mac address for IODR #2
  byte ip[] =      {129, 170, 64, 63}; //the IP address for IODR #2
#endif


// Parameters for IODR #3
#if IODR_ID == 3
  #include <Ethernet2.h> //ethernet shield W5500 chip

  // ThingSpeak parameters for IODR #3
  #define OD_CHANNEL_ID 469909
  #define OD_API_KEY "TE0K3BIY5JQECY51" //write API key
  #define TEMP_CHANNEL_ID 890567
  #define TEMP_API_KEY "C509JHJCZNRGDNRE" //write API key
  #define TEMP_INT_CHANNEL 5 //channel number for IODR #3 internal temperature data
  #define TEMP_EXT_CHANNEL 6 //channel number for IODR #3 external temperature data
  #define TEMP_SENSE_PIN 8 // this device doesn't have an external temperature sensor, so use the internal one

  // Network parameters for IODR #3
  byte mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0x45, 0x10 }; //mac address for IODR #1 old ethernet shield
  byte ip[] =      { 129, 170, 64, 62 }; //the IP address for IODR #3
#endif


unsigned long lastConnectionTime = 0;                // last time we connected to Thingspeak
const unsigned long connectionInterval = 90000;      // delay between connecting to Thingspeak in milliseconds
unsigned long lastLoopTime = 0;                      // to keep track of the loop speed
unsigned long lastODReadTime = 0;                    // last time the light sensor was read
unsigned long connectionIntervalODRead = 800;        // delay between OD reads, also delay between temperature reads
const int MAX_FAILED_UPLOADS = 8;             // reset the arduino if there are too many failed upload attempts
int numFailedUploads = 0;                      // keep track of how many times the data upload has failed (i.e. server did not return 200)

// digital temperature sensor
int DS18B20_Pin = TEMP_SENSE_PIN; //DS18B20 temp sensor signal pin on digital 8 or 44, depending on the device
OneWire ds(DS18B20_Pin); // on digital pin 8
float temperature;
DallasTemperature sensors(&ds);
DeviceAddress tempSensor;
boolean temperatureDeviceFound = false;


// LEDs and light sensors
int ledPin = 9;
int yellowLED = 2;
#define numTubes 8
int wdTimer = 3;

const PureAnalogPin lightInPin[numTubes] = {A13, A13, A13, A12, A11, A10, A9, A8}; //pins for analog inputs for light sensor 
// {A15, A14, A13, A12, A11, A10, A9, A8}
// Pins A13 and A12 require the use of Arduino_AdvancedAnalogue.h library
float lightIn[] = {0,0,0,0,0,0,0,0}; //value of light sensor
float ODvalue[] = {0,0,0,0,0,0,0,0};
int LEDoffReading[] = {0,0,0,0,0,0,0,0};
int LEDonReading[] = {0,0,0,0,0,0,0,0};
int pointsToAverage = 10; //number of light readings to average in each data point sent to Cosm
int blankValue[] = {4000, 4000, 4000, 4000, 4000, 4000, 4000, 4000}; //default blank value of 4000 for each light sensor (12-bit ADC) 

int blankButtonState[] = {0,0,0,0,0,0,0,0}; // status of buttons for blanking individual tubes
int blankButtonPin[] = {20, 22, 24, 26, 28, 30, 32, 34}; 
int lastButtonPressed = 0; // keep track of which reset button was pressed most recently
const int NUM_CYCLES_TO_RESET = 12; //number of cycles the blank button has to be pressed down to before the blank value is reset

int calibrationPin = 5;
int calibrationMode = 1; // 1 is for no calibration, 0 is for calibration mode.  in calibration mode, one reading is taken every time the button is pressed
int calibrationCount = 0; // keeps track of how many times the calibration button was pressed

// for OLED serial display
//gloSoftSerialSetup( myOLED, 18, noninverted ); 
boolean displayTubeSummary = true;


// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
//EthernetClient client; //comment out for giga test %%%

// Network configuration for static IP address on Dartmouth network
byte subnet[] =  { 255, 255, 248, 0 }; //the IP mask for both IODR devices
byte gateway[] = { 129, 170, 64,  1 }; //the gateway address for both IODR devices
byte dnsServer[] =     { 129, 170, 64, 43 }; //the DNS server address for both IODR devices




//------------------------------------------------------------------------
//           SETUP
//------------------------------------------------------------------------
void setup(void)
{
  //enable the watchdog timer at the beginning of the setup loop
  //if the program hangs for more than 8 seconds, the arduino will be reset
  //when the program is functioning correctly, the watchdog timer needs to be reset more frequently than every 8 seconds
  if (watchdog.start(30000)) {
    Serial.println("Initialized watchdog successfully");
  } // 30-second timeout ; max of 32760 ms
  //wdt_enable(WDTO_8S);

  //set up serial LCD display
  // Begin using the serial object created above.
  //gloSoftSerialBegin( myOLED ) ;
  Serial2.begin(9600); // Serial port for printing to the LCD
  Serial2 << gloClear << gloFont_1w1h ;
  delay(500); // wait for lcd display to boot up

  //announce version information
  Serial.begin(115200); //serial port for debugging set to 115200 baud
  Serial.print("Internet OD reader v");
  Serial.println(VERSION);
  Serial.println("==========================");
  Serial2 <<gloClear<<"Internet OD reader v" << VERSION << " ID: " << IODR_ID;
  //wdt_reset(); //not part of giga R1, reimplement later; all instances have been commented out and marked with "%%%"
  watchdog.kick();

  // set up DS18B20 digital thermometer
  Serial.println("calling initTemp()");
  temperatureDeviceFound = initTemp();
  Serial.println(temperatureDeviceFound);
  
  //calibration mode (might add this in later if I need it)
  
  //read blankValues from EEPROM and store in blankValue integer array
  //these values are saved between resets
   for (int i=0; i < numTubes; i++){
     //readBlankFromEEPROM(i); //not compatible with giga R1 board; all instances commented out with "%%%"
   }
  
  //set up I/O pins
  pinMode(ledPin, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(wdTimer, OUTPUT);
  //analogReference(EXTERNAL);
  pinMode(calibrationPin, INPUT);
  pinMode(53, OUTPUT); //necessary for WiFi shield function

  // reset external watchdog timer
  digitalWrite(wdTimer, HIGH);
  delay(100);
  digitalWrite(wdTimer, LOW);
  

  // send message to OLED display
  Serial2 << gloReturn << "connecting to internet..."; 
  
  #if defined(ARDUINO_GIGA) //only check for wifi module if board is a giga
  // Initialize wifi
  Serial.println(F("Initializing wifi chip..."));

  //check giga wifi module
  if (WiFi.status() == WL_NO_MODULE) { 
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  //connect to wifi
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 3 seconds for connection:
    delay(3000);
  }
  serialPrintWifiStatus();                        // you're connected now, so print out the status; remove this function if not needed

  #else 
  // Initialize ethernet
  Serial.println(F("Initializing ethernet chip..."));

  Ethernet.begin(mac); // try to connect to the network by DHCP
  //Ethernet.begin(mac, ip, dnsServer, gateway, subnet); // try to connect to the network with fixed IP address

  #endif


  // try to connect to Thingspeak
  //ThingSpeak.begin(client); 
  /*
  Serial.print("Client connection status:");
  Serial.println(client.connected());
  while (!client.connected())
  {
    Serial.println(F("Couldn't begin()! Check your wiring?"));
    Serial2 << gloClear << "error, no internet connection, please reset";
    delay(500); // wait a bit and try again, eventually the external watchdog timer will do a hard reset
    ThingSpeak.begin(client); // try to connect to the Thingspeak again
    //while(1);
  }
  */
  
  #if defined(ARDUINO_GIGA)
    // WiFi: Display IP address
    IPAddress ip = WiFi.localIP();
    Serial.print("IP = ");
    Serial.println(ip);
    Serial2 << gloClear << "IP=" << ip[0] << "." << ip[1] << "." << ip[2] << "." << ip[3];
    Serial2 << gloReturn << "mac=N/A"; // GIGA WiFi library doesn't give MAC by default
  #else
    // Ethernet: Display IP and MAC address
    Serial.print("IP = ");
    Serial.println(Ethernet.localIP());
    Serial2 << gloClear << "IP=" << Ethernet.localIP();
    Serial2 << gloReturn << "mac=" << mac[0] << ":" << mac[1] << ":" << mac[2] << ":" << mac[3] << ":" << mac[4] << ":" << mac[5];
  #endif
  
  Serial.println("     ***** Network initialized *****");
  //checkTiming();
  //wdt_reset(); //%%%
  watchdog.kick();
  delay(3000); // delay so we can read the info on the OLED display

  Serial.println("Setup complete");
  Serial2 << gloClear;
  checkTiming();
  //wdt_reset();  //%%%
  watchdog.kick();

  
}
//------------------------------------------------------------------------
//           end SETUP
//------------------------------------------------------------------------



//------------------------------------------------------------------------
//           MAIN PROGRAM LOOP
//------------------------------------------------------------------------
void loop()
{
  // Reset watchdog
  watchdog.kick();
  //wdt_reset(); //%%%
  
  //digitalWrite(startMainLoopLED, HIGH);
  
  //check blank buttons.  These are the momentary switches along the top edge of the IODR circuit board
  checkBlankButtons();
  //Serial.println("     ***** checkBlankButtons() *****");
  //checkTiming();
  
  //display light sensor status on LCD
  if (displayTubeSummary){
    displayTubeStatusSummary();
    //Serial.println("     ***** displayTubeStatusSummary() *****");
    //checkTiming();  
  } 
  else {  
     displayTubeStatus(lastButtonPressed);
     Serial.println("     ***** displayTubeStatus() *****");
     //checkTiming();
  } 

  //digitalWrite(startMainLoopLED, LOW);


  //read light sensors and check temperature
  if (millis() - lastODReadTime > connectionIntervalODRead){

    readLightSensors();
    temperature = getTemp(); //read the temperature value from the DS18B20 chip
    lastODReadTime = millis();
    
    //send light sensor, temp and blank button information to serial terminal for debugging purposes
    serialPrintState();
    //blankButtonStatusDisplay();
  }
  
  // send data to Thingspeak
  if (millis() - lastConnectionTime > connectionInterval) { 
    Serial.println("@@@@@ yellow LED on @@@@@");
    digitalWrite(yellowLED, HIGH); // turn on yellow LED when sending data to Thingspeak  
    
    //connect to thingspeak
    Serial.println("@@@@@ uploadDataToThingspeak() @@@@@");
    //uploadDataToThingspeak();
    Serial.println("@@@@@ checkTiming() @@@@@");
    checkTiming();  
    // update connection time so we wait before connecting again
    lastConnectionTime = millis();

    Serial.println("***** yellow LED off *****");
    digitalWrite(yellowLED, LOW); // turn off yellow LED
    
    Serial.println("***** display tube summary *****");
    displayTubeSummary = true; // switch display back to summary mode
    //Serial.println("***** initTemp *****");
    //temperatureDeviceFound = initTemp(); // try to re-initialize the temperature sensor every time data is uploaded, in case there was a problem with the initialization during setup() 
  } 

  
  //Serial.println("################################# END OF MAIN LOOP #####################################");
  //checkTiming();
}
//------------------------------------------------------------------------
//           end MAIN PROGRAM LOOP
//------------------------------------------------------------------------



//write blank value to EEPROM
//8 2-byte values get stored
//address 0 and 1 store the first blank value
//address 2 and 3 store the second blank value
void writeBlankToEEPROM (int tubeIndex){
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
void readBlankFromEEPROM (int tubeIndex){
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
void checkTiming(){
  Serial.print("***checkTiming()*** loop time=");
  Serial.print(millis()-lastLoopTime);
  lastLoopTime = millis();
  Serial.println();
}



// check the state of the reset buttons. keep track of which button was most recently pressed.  
// if a button is pressed and held for a certain amount of time (>2 seconds), then reset the blank value for that well
void checkBlankButtons(){
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
void blankButtonStatusDisplay(){
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
void serialPrintState(){  
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
void displayTubeReset(int tubeNum){
  Serial2 << gloClear << "Tube: " << tubeNum + 1; //tubNum index starts at 0, so increment by one for human-readable display
  Serial2 << gloReturn << "Was reset" ;
  
  Serial.print("displayTubeReset:");
  //Serial.println(resetTopLine);
  delay(1500); //show the message for 1.5 seconds
}




// Summary Display
// display current OD for all tubes on OLED display
void displayTubeStatusSummary(){
  //Serial.println("*************************************");
  //Serial.println("displayTubeStatusSummary()");
  char displayLine[4][17] = {
       "1:     5:     T ",
       "2:     6:     e ",
       "3:     7:     m ",
       "4:     8:     p "};  
  char odval[5] = "    ";
                           
  //add in OD values to first column
  for (int i=0; i<4; i++){  
    //loop through each character in odval and add it to the displayLine char array
    //add odval in first position (i.e. tubes 1, 2, 3 and 4)
    //dtostrf(ODvalue[i], 4, 2, odval); //%%% not in GIGA
    sprintf(odval, "%4.2f", ODvalue[i]);
    //Serial << "   odval[" << i << "]=" << odval <<" length=" << sizeof(odval);
    
    for (int j = 0; j<sizeof(odval)-1; j++){ //sizeof(odval)-1 is to avoid including the "end of string" character
      displayLine[i][j+2] = odval[j];
      //Serial << "        displayLine["<<i<<"]["<<j+2<<"]="<<odval[j];
    }
    //add odval in second position (i.e. tubes 5, 6, 7 and 8)
    //dtostrf(ODvalue[i+4], 4, 2, odval);  //%%% NOT in GIGA
    sprintf(odval, "%4.2f", ODvalue[i+4]);
    //Serial << "   odval_2_[" << i << "]=" << odval <<" length=" << sizeof(odval);
    //Serial.println();
    for (int j = 0; j<sizeof(odval)-1; j++){
      displayLine[i][j+9] = odval[j];
    }
  }

  //add temperature value
  char tempStr[5] = "    ";
  sprintf(tempStr, "%4.2f", temperature);
  //dtostrf(temperature, 4,2, tempStr); %%% NOT in GIGA
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
void displayTubeStatus(int tubeNum){
  //Serial << "\n************** displayTubeStatus() ******************* tubeNum=" << tubeNum;

  char bval[15]; //note: if these character arrays are too short, they can overwrite each other
  char odval[15];
  char ltin[15];
  char topLine[] = "Tube:   Blk=    ";
  char botLine[] = "OD=     Raw=    ";
  
  //convert variables to strings of the appropriate length
  sprintf(bval, "%4.2f", blankValue[tubeNum]);
  //dtostrf(blankValue[tubeNum], 4, 0, bval); //%%% NOT in GIGA
  //Serial.println();
  //dtostrf(LEDonReading[lastButtonPressed], 4, 0, ltin); //use LEDonReading for the raw value.  
                                                          //We want to be able to see how close we are to saturating the detector when we're adjusting the potentiometer.
                                                          //Note that the raw value will show fluctuations due to ambient light, but the lightIn value (i.e. abient light subtracted)
                                                          //is what is being used to calculate the OD value
  //dtostrf(lightIn[tubeNum], 4, 0, ltin);  %%%NOT in GIGA      //use lightIn for raw value
  sprintf(ltin, "%4.2f", lightIn[tubeNum]);
  //  Serial <<"       ltin=" <<ltin;
  //Serial.println();
                                                          //not sure which I prefer, lightIn or LEDonReading, for raw value
                                                          //using lightIn for now
  //dtostrf(ODvalue[tubeNum], 4, 2, odval); %%% NOT IN giga
  sprintf(odval, "%4.2f", ODvalue[tubeNum]);
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
void uploadDataToThingspeak(){
  // send information to OLED display
  Serial2 << gloClear << "Sending data to Thingspeak...";
  
    //loop through datastreams (except temperature) and update data
  for (int i=0; i<numTubes; i++){
      //ThingSpeak.setField(i+1, ODvalue[i]); //set each field one-by-one, thingspeak channels are indexed from 1
    }

  // send OD data to thingspeak
  Serial.println("sending OD data to thingspeak");
  //int odWriteResponse = ThingSpeak.writeFields(OD_CHANNEL_ID, OD_API_KEY);
  //Serial.println(odWriteResponse); %%% thingspeak
  //Serial2 << gloReturn << odWriteResponse; %%%thingspeak // send data to OLED display
  //wdt_reset(); //%%%
  watchdog.kick(); 

  //send temperature data to thingspeak
  Serial.println("sending temperature data to thingspeak");
  //ThingSpeak.setField(TEMP_EXT_CHANNEL, temperature);
  //int tempWriteResponse = ThingSpeak.writeFields(TEMP_CHANNEL_ID, TEMP_API_KEY); %%% thingspeak
  //Serial.println(tempWriteResponse); %%% thingspeak
  //Serial2 << gloReturn << tempWriteResponse;  %%%thingspeak
  //send information to OLED display
  delay(1500); // enough time to read the display
  //wdt_reset(); //%%%
  watchdog.kick();

  // %%%% thingspeak (whole block)
  // if (odWriteResponse == 200){ // 200 is the server response from Thingspeak indicating success
  //   numFailedUploads = 0;
  //   digitalWrite(wdTimer, HIGH); // reset the watchdog timer
  //   Serial.println("numFailedUploads reset");
  //   delay(500);
  //   digitalWrite(wdTimer, LOW);
  // }
  // else{
  //   numFailedUploads += 1;
  //   Serial2 << gloClear << "Failed uploads = " << numFailedUploads;
  //   Serial.print("Failed uploads = ");
  //   Serial.println(numFailedUploads);
  //   // try to reset the network connection and re-connect to thingspeak
  //   //Ethernet.begin(mac, ip, dnsServer, gateway, subnet); // try to connect to the network with fixed IP address; commented out for giga testing %%%
  //   //WiFi.disconnect();
  //   //connectToWiFi(); // try to reconnect
  //   //ThingSpeak.begin(client); 
  //   delay(1000); // allow time to read the display
  // }

  // this if block should be unnecessary, since the watchdog timer will reset after 5 minutes with no successful upload (i.e. about 5 failed tries)
  if (numFailedUploads > MAX_FAILED_UPLOADS){
    Serial2 << gloClear << "Upload error: waiting to reset..."; 
    digitalWrite(wdTimer, LOW); // turn off the upload light to allow the watchdog timer to reset
    delay(400000); // delay 6 minutes, should trigger watchdog timer to reset
  }
  
  
  
}


void readLightSensors(){
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
    for (int i = 4; i < numTubes; i++){ // %%% skips first four tubes
      LEDoffReading[i] = analogRead(lightInPin[i]);      
    }
    // read light sensor values with the LED on
    digitalWrite(ledPin, HIGH); //turn on LEDs
    delay(10); //it takes ~1ms for the light senor reading to stabilize after the LED has been turned on
    for (int i = 4; i < numTubes; i++){ // %%% skips first four tubes
      LEDonReading[i] = analogRead(lightInPin[i]);      
    }   
    // calculate the difference and add it to lightIn
    for (int i = 4; i < numTubes; i++){ // %%% skips first four tubes
      lightIn[i] += (LEDonReading[i] - LEDoffReading[i]);
    } 
  }
  // divide lightIn by pointsToAverage to get the average value
  for (int i = 4; i < numTubes; i++){ // %%% skips first four tubes
      lightIn[i] = lightIn[i]/pointsToAverage;
      ODvalue[i] = -(log10(lightIn[i]/blankValue[i]));
  }
  // turn the lights off when you're done
  digitalWrite(ledPin, LOW); //turn off LEDs
}


// ******************* TEMPERATURE SENSOR SUBROUTINES **********************************
//initialize DS18B20 temperature sensor
boolean initTemp(){
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
float getTemp(){
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
