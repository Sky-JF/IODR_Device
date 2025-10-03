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
A8: pin 89, light sensor for tube 8 (new pin is A0)
A9: pin 88, light sensor for tube 7 (new pin is A1)
A10: pin 87, light sensor for tube 6 (new pin is A2)
A11: pin 86, light sensor for tube 5 (new pin is A3)
A12: pin 85, light sensor for tube 4 (new pin is A4)
A13: pin 84, light sensor for tube 3 (new pin is A5)
A14: pin 83, light sensor for tube 2 (new pin is A6)
A15: pin 82, light sensor for tube 1 (new pin is A7)
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
//#include "GigaCompatibility.h" // modified ThingSpeak library
//#include <EEPROM.h> //used to store blank values so they persist between resets; used with Arduino mega boards %%%
// #include <FlashStorage.h> //used to store blank values so they persist between resets; used with Arduino giga boards %%%
// #include <FlashAsEEPROM.h> // might not work %%%
#include <gloSerialOLED.h> //for OLED character display 
//gloStreamingTemplate //allows carat notation for OLED display
//#include "mbed.h" //WDT test for giga %%% mbed::Watchdog &watchdog = mbed::Watchdog::get_instance();
#include "secrets.h" // Include secrects (wifi passwords and API keys)


//Parameters for IODR with Arduino Giga R1; change ID as necessary; this block assumes the ID is 1
#if IODR_ID == 4
  #include <WiFi.h> //for Arduino Giga board wifi connection
  #include <WiFiClient.h>
  #include <ArduinoHttpClient.h>

  template<class T> inline Print &operator <<(Print &obj, T arg) { obj.print(arg); return obj; } // allow operator << to print strings to OLED

  // Login details for Wifi Network from secrets.h
  char ssid[] = DARTMOUTH_PUBLIC_SSID;        // your network SSID (name)
  char pass[] = DARTMOUTH_PUBLIC_PASS;    // your network password (use for WPA, or use as key for WEP)

  WiFiSSLClient wifiClient;  //used to upload to thingspeak
  const char* server = "api.thingspeak.com";
  #define PORT 443 //post used by the WiFiSSLClient to send through https
  HttpClient client = HttpClient(wifiClient, server, PORT);

  #define OD_CHANNEL_ID 2926522
  #define TEMP_CHANNEL_ID 3098483 
  // API keys are in secret.h file
  // OD_API_KEY
  // TEMP_API_KEY
  #define TEMP_INT_CHANNEL 1 //channel number for IODR #1 internal temperature data
  #define TEMP_EXT_CHANNEL 2 //channel number for IODR #1 external temperature data
  #define TEMP_SENSE_PIN 44 // Is this correct pin ??? check

  #define ADC_12_BITS 12 // 12-bit ADC to more accurately measure light sensor
#endif

// Parameters for IODR #1
#if IODR_ID == 1
  #include <Ethernet2.h> //ethernet shield W5500 chip

  // ThingSpeak parameters for IODR #1
  #define OD_CHANNEL_ID 405675
  #define OD_API_KEY "J1AW4IF7557WHYAX" //write API key
  #define TEMP_CHANNEL_ID 890567
  #define TEMP_API_KEY "C509JHJCZNRGDNRE" //write API key
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

const uint8_t lightInPin[numTubes] = {A7, A6, A5, A4, A3, A2, A1, A0}; //pins for analog inputs for light sensor 
// originally {A15, A14, A13, A12, A11, A10, A9, A8}
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
void setup(void) {
  analogReadResolution(ADC_12_BITS); // Use 12-bit resolution for the analogRead() function

  //enable the watchdog timer at the beginning of the setup loop
  //if the program hangs for more than 8 seconds, the arduino will be reset
  //when the program is functioning correctly, the watchdog timer needs to be reset more frequently than every 8 seconds
  //if (watchdog.start(30000)) {
  //  Serial.println("Initialized watchdog successfully");
  //} // 30-second timeout ; max of 32760 ms
  //wdt_enable(WDTO_8S);

  //enable the watchdog timer at the beginning of the setup loop
  //if the program hangs for more than 8 seconds, the arduino will be reset
  //when the program is functioning correctly, the watchdog timer needs to be reset more frequently than every 8 seconds
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
  //watchdog.kick();

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
  // pinMode(wdTimer, OUTPUT); // %%% Review this 
  //analogReference(EXTERNAL);
  pinMode(calibrationPin, INPUT);
  pinMode(53, OUTPUT); //necessary for WiFi shield function

  // reset external watchdog timer
  // digitalWrite(wdTimer, HIGH); // %%% Review this
  delay(100);
  // digitalWrite(wdTimer, LOW); //%%% Review this



  // Initialize wifi
  Serial.println(F("Initializing wifi chip..."));
  connectToWifi();

  Serial.println("Setup complete");
  Serial2 << gloClear;

}
//------------------------------------------------------------------------
//           end SETUP
//------------------------------------------------------------------------



//------------------------------------------------------------------------
//           MAIN PROGRAM LOOP
//------------------------------------------------------------------------
void loop()
{
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
    uploadDataToThingspeak(); 
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
