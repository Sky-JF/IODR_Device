#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoHttpClient.h>

// InfluxDB Configuration
#define INFLUXDB_HOST "192.168.50.73"  // InfluxDB server IP
#define INFLUXDB_PORT 8086
#define INFLUXDB_ORG "0ee5fa4aed348c7e"
#define INFLUXDB_BUCKET "IODR test" //try "IODR%20test"
//#define INFLUXDB_TOKEN "ioTMLWf9DXCOcN_dzUofbMDQYFnoghLFi5YImWyhqXh3se0J0IMcGHaeYeJ9gUZ9gC6V1AmCP76pkbLswteU5g"
#define INFLUXDB_TOKEN "VBLemrOmJcF62lYVj2NT2xT6C3i3FfzSeMBXEUL3aV7oThysMrzoxCSFTHDDBcBzXbVXWKR0XVyrwQInxa-HHA=="

// Ethernet Shield Configuration
byte mac[] = { 0xA8, 0x61, 0x0A, 0xAE, 0x3F, 0xBC };
EthernetClient client;
HttpClient httpClient(client, INFLUXDB_HOST, INFLUXDB_PORT);

void setup() {
  Serial.begin(115200);
  while (!Serial);

  delay(1000);

  Serial.println("Initializing Ethernet...");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    return;
  }
  delay(1000);

  Serial.print("Connected! IP Address: ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  uploadDataToInfluxDB(3.2);
  delay(5000);  // Send data every 5 seconds
}

void uploadDataToInfluxDB(float value) {
  // Correct InfluxDB Line Protocol format (without timestamp)
  String data = "temp_value,sensor=arduino val=" + String(value);

  Serial.println("Sending data: " + data);

  // Create HTTP request
  String url = "/api/v2/write?org=" + String(INFLUXDB_ORG) + "&bucket=" + String(INFLUXDB_BUCKET) + "&precision=s";

  httpClient.beginRequest();
  httpClient.post(url);
  httpClient.sendHeader("Authorization", "Token " + String(INFLUXDB_TOKEN));
  httpClient.sendHeader("Content-Type", "text/plain");
  //Serial.println("sent header"); //arduino is resetting after reaching this point
  httpClient.sendHeader("Content-Length", data.length());
  httpClient.beginBody();
  httpClient.print(data);
  httpClient.endRequest();

  // Read response
  int statusCode = httpClient.responseStatusCode();
  String response = httpClient.responseBody();
  Serial.println("after response request"); //arduino is resetting before reaching this point
  Serial.print("Status Code: ");
  Serial.println(statusCode);
  Serial.println("Response: " + response);
}
