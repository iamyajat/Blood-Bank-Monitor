#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "DHT.h"

// Uncomment one of the lines below for whatever DHT sensor type you're using!
#define DHTTYPE DHT11  // DHT 11
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
// #define DHTTYPE DHT22  // DHT 22  (AM2302), AM2321

/*Put your SSID & Password*/
const char *ssid = "VIT8G";           // Enter SSID here
const char *password = "iamyajat69";  //Enter Password here

ESP8266WebServer server(80);

// DHT Sensor
uint8_t DHTPin = D1;
uint8_t DHTPin2 = D2;


// Initialize DHT sensor.
DHT dht(DHTPin, DHTTYPE);
DHT dht2(DHTPin2, DHTTYPE);

float Temperature;
float Humidity;
float Temperature2;
float Humidity2;

void setup() {
  pinMode(DHTPin, INPUT);
  pinMode(DHTPin2, INPUT);
  delay(3000);

  Serial.begin(115200);


  dht.begin();
  dht2.begin();


  Serial.println("Connecting to ");
  Serial.println(ssid);

  //connect to your local wi-fi network
  WiFi.begin(ssid, password);

  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

void handle_OnConnect() {
  Temperature = dht.readTemperature();  // Gets the values of the temperature
  Humidity = dht.readHumidity();        // Gets the values of the humidity

  Temperature2 = dht2.readTemperature();  // Gets the values of the temperature
  Humidity2 = dht2.readHumidity();        // Gets the values of the humidity

  server.send(200, "text/html", SendHTML(Temperature, Humidity, Temperature2, Humidity2));
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}

String SendHTML(float Temperaturestat, float Humiditystat, float Temperaturestat2, float Humiditystat2) {
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr += "<title>Blood Bank Monitor</title>\n";
  ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr += "p {font-size: 20px;color: #444444;margin-bottom: 10px;}\n";
  ptr += "</style>\n";
  ptr += "</head>\n";
  ptr += "<body>\n";
  ptr += "<div id=\"webpage\">\n";
  ptr += "<h1>Blood Bank Monitor</h1>\n";

  ptr += "<h2>Refrigerator Conditions</h2>\n";
  ptr += "<p>Temperature: ";
  ptr += (float)Temperaturestat;
  ptr += "&#8451;</p>";
  ptr += "<p>Humidity: ";
  ptr += (float)Humiditystat;
  ptr += "%</p>";

  ptr += "<h2>Blood Conditions</h2>\n";
  ptr += "<p>Temperature: ";
  ptr += (float)Temperaturestat2;
  ptr += "&#8451;</p>";
  ptr += "<p>Humidity: ";
  ptr += (float)Humiditystat2;
  ptr += "%</p>";

  ptr += "</div>\n";
  ptr += "</body>\n";
  ptr += "</html>\n";
  return ptr;
}