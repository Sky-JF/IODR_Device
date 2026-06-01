# Arduino Internet Sensor Project

Arduino Giga R1 

This project uses an Arduino Giga R1 to collect and transmit sensor data over the internet. It is designed to be an efficient way to monitor growth of bacterial cultures remotely.

## Navigation
- `ODreader_v30`

Directory with the .ino files used by the Arduino IDE to upload the code. 

- `secrets_template.h`

Template file with constant used in `ODreader_v30`, which include API keys, device IDs, and other information relating to connected databases.

## How to use
Open the `ODreader_v30` folder with the Arduino IDE and upload the code. 

## Features
- Uses a watch dog timer to supervise state of the Arduino board
- Reads sensor data from 8 tubes with connected sensors
- Integrates the ThingSpeak cloud platform for remote monitoring
- Integrates InfluxDB on a locally run server for locally storing data
- Prints data collected to OLED display
- Allows for reset of data collected for individual tubes

## Hardware 
- Arduino Giga R1 board
- Light Sensor
- Temperature Sensor (DS18B20)

## Additional Resources and Software Used
- [Frontend data viewer](https://iodr-605db139538a.herokuapp.com)
- [GitHub repository for frontend data viewer](https://github.com/danolson1/IODR_project?tab=readme-ov-file#readme)
- [Thingspeak data webpage](https://thingspeak.mathworks.com/channels/469909)

