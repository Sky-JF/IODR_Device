#define DARTMOUTH_PUBLIC_SSID  //<wifi SSID>
#define DARTMOUTH_PUBLIC_PASS //<wifi password>

#define INFLUXDB_API_KEY //<influxdb api key>
#define ORG_NAME //<influxdb organization name>
#define BUCKET_NAME //<influxdb bucket name>

// ThingSpeak API keys

// all the API keys below belong to the data channels under Professor Olson's account
// Note: CHANNEL_ID for the channels are not required for upload, so they are not included for the devices below

#if IODR_ID == //<id_number>
  #define OD_API_KEY //<api_key>
  #define TEMP_API_KEY //<api_key>
#endif

