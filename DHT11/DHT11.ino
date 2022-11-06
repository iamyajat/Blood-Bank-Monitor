#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include "DHT.h"

#define DHTTYPE DHT11  // DHT 11

const char *ssid = "***";
const char *password = "***";
const char *apiKeyIn = "***";

WiFiClient wifiClient;

String host = "http://api.asksensors.com";

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
  Temperature = dht.readTemperature();    // Gets the values of the temperature
  Humidity = dht.readHumidity();          // Gets the values of the humidity
  Temperature2 = dht2.readTemperature();  // Gets the values of the temperature
  Humidity2 = dht2.readHumidity();        // Gets the values of the humidity

  if (WiFi.status() == WL_CONNECTED) {

    HTTPClient http;

    Serial.print("[HTTP] begin...\n");

    // Create a URL for the request
    String url = "";
    url += host;
    url += "/write/";
    url += apiKeyIn;
    url += "?module1=";
    url += Temperature;
    url += "&module2=";
    url += Temperature2;

    Serial.print("********** requesting URL: ");
    Serial.println(url);
    http.begin(wifiClient, url);  //HTTP

    Serial.println("> Request sent to ASKSENSORS");

    Serial.println("[HTTP] GET...");
    int httpCode = http.GET();
    if (httpCode > 0) {
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println(payload);
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  }
}

void handle_OnConnect() {
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