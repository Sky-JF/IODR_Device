# Arduino Ethernet Sensor Project

This project uses an Arduino with an Ethernet shield to collect and transmit sensor data over the internet. It is designed to be an efficient way to monitor growth of bacterial cultures remotely.

## Features
- Uses a watch dog timer to supervise state of the Arduino board
- Reads sensor data from 8 tubes with connected sensors
- Uses an Ethernet shield to send data over the Internet
- Integrates the ThingSpeak cloud platform for remote monitoring
- Prints data collected to OLED display
- Allows for reset of data collected for individual tubes

## Hardware 
- Arduino board
- Ethernet shield (W5500)
- Light Sensor
- Temperature Sensor (DS18B20)

## Libraries Included
- `<SPI.h>` Serial Peripheral Interface for communication with wifi or ethernet shield
- `<avr/wdt.h>` Watchdog Timer for AVR microcontroller
- `<OneWire.h>` For communication protocol with temperature sensors
- `<DallasTemperature.h>` Handling data from the temperature sensor
- `<Math.h>` 
- `<SoftwareSerial.h>` Communication to LCD display
- `"ThingSpeak.h"` Connection to ThingSpeak cloud 
- `<EEPROM.h>` Store persistent data (blank values) between power offs
- `<gloSerialOLED.h>` Printing in LCD display
- `<Ethernet2.h>` Library for the shield W5500 chip

  ## Additional Resources and Software Used
  - [Database with processed IODR data](https://iodr-605db139538a.herokuapp.com)
  - [GitHub page for the database with the IODR data](https://github.com/danolson1/IODR_project?tab=readme-ov-file#readme)
  - [Thingspeak data](https://thingspeak.mathworks.com/channels/469909)

