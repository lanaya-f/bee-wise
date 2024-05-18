#include "DHT.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include "UbidotsEsp32Mqtt.h"

const char *UBIDOTS_TOKEN = "BBUS-JXaJGPPs6nxpIOdHtkEAy0aKiiWuD3";
const char *WIFI_SSID = "sumire";
const char *WIFI_PASS = "qqqqqqqq";
const char *DEVICE_LABEL = "BEEWISE";
const char *VARIABLE_LABEL_T = "tsense";
const char *VARIABLE_LABEL_H = "hsense";
const char *VARIABLE_LABEL_N = "nsense";
const char *VARIABLE_LABEL_W = "wsense";
const unsigned long PUBLISH_FREQUENCY = 5000;

unsigned long timer;
uint8_t tpin = 27;
#define hpin 4
#define DHTTYPE DHT22
uint8_t npin = 34;
uint8_t wpin = 2;

Ubidots ubidots(UBIDOTS_TOKEN);
DHT dht(hpin, DHTTYPE);

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void setup()
{
  Serial.begin(115200);
  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();
  dht.begin();

  timer = millis();
}

void loop()
{
  if (!ubidots.connected())
  {
    ubidots.reconnect();
  }
  if (millis() - timer >= PUBLISH_FREQUENCY)
  {
    float humid = dht.readHumidity();
    float temp = dht.readTemperature();
    float hic = dht.computeHeatIndex(temp, humid, false);
    Serial.println("Publishing data to Ubidots Cloud");
      Serial.print(F("Temperature: "));
      Serial.print(temp);
      Serial.println(F("°C "));
      Serial.print(F("Humidity: "));
      Serial.print(humid);
      Serial.println(F("%"));
      Serial.print(F("Heat index: "));
      Serial.print(hic);
      Serial.println(F("°C "));
    ubidots.add(VARIABLE_LABEL_T, temp);
    ubidots.publish(DEVICE_LABEL);
    ubidots.add(VARIABLE_LABEL_H, humid);
    ubidots.publish(DEVICE_LABEL);
    float nfreq = analogRead(npin);
    ubidots.add(VARIABLE_LABEL_N, nfreq);
    ubidots.publish(DEVICE_LABEL);
    
    Serial.print("noise: ");
    Serial.println(nfreq);
    timer = millis();
  }
  ubidots.loop();
}
