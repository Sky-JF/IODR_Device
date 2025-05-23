# Internet OD reader (IODR) program
Detect growth of bacterial cultures by measuring the absorbtion of a light
## Version 29
Dan Olson
3-14-2024

### Note: 4-27-2025
* Changed light read pins since A15 and A14 do not exist in the giga

### Note: 4-19-2025
* Commented out functions that do not work with giga to test out the board
* Added if statements to differentiate between mega and giga boards for the migration
* Commented out parts that are not compatible with gigas with "%%%"

### Note: 5-20-2024. 
* I replaced the Arduino Mega and Ethernet shield on IODR #2
* I replaced the Ethernet shield on IODR #3

### New in v29 3-14-2024
* increased Thingspeak update time interval from 60 to 90 seconds

### New in v28 4-8-2022
* Support for 

### New in v27 10-25-2019
* Added more watchdog timer resets to avoid unnecessary resetting

### New in v26 10-16-2019
* Updated MAC address for IODR #1 because I replaced all of the circuit boards
* Switched temperature sensor to pin 44
* Set up new thingspeak channel to hold temperature data from all OD readers

### New in v25
* IODR\_ID is a constant that sets up the device-specific parameters

### New in v24
* moved up gloSoftSerialBegin to top of setup loop. Previously there was an instruction sending data to the display before the Begin command was called
* disabled internal watchdog timer because I'm using an external one
* set up to use manual IP configuration instead of DHCP server
* set up to make repeated attempts to connect to network
* send thingspeak response codes to OLED display
* reset device if too many unsuccessful attempts to send data to thingspeak

### New in v23
* replaced ethernet shield with new one from Adafruit. It uses the W5500 chip, so I needed to change to the Ethernet2 library.
* I commented out a bunch of the println statments so I can see what's happening to the thingspeak uploads
* In some cases, the MemoryUsage check seems to interfere with uploading data to ThingSpeak
* I think the temperature sensor is a DS18B20, not DS18S20. Maybe this was causing read problems?
* I disabled the getTemp() call in the loop to see if this solves the freezing problem

### New in v22
* changing over from xively to thingspeak for storing cloud data, since xively just shut down
* I'm using the ThingSpeak account associated with my daniel.g.olson@dartmouth.edu account

### New in v21
* still having intermittent freezing problems, trying to troubleshoot.

### For use with v1 of the IODR, 
* using an Arduino Mega 2560 
* Arduino ethernet shield (replaced Adafruit CC3000 WiFi shield, which had stability problems)
* IODR v2 shield circuit board (designed by me)
* note:  the Spol jumper on the OLED display board needs to be cut in order for the display to function properly with the Arduino

