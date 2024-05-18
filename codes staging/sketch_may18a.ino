#include "UbidotsEsp32Mqtt.h"

const char *UBIDOTS_TOKEN = "BBUS-JXaJGPPs6nxpIOdHtkEAy0aKiiWuD3";
const char *WIFI_SSID = "home 2.4";
const char *WIFI_PASS = "qqqqqqqq";
const char *DEVICE_LABEL = "beewise";
const char *VARIABLE_LABEL = "nsense";

const unsigned long PUBLISH_FREQUENCY = 1000;

unsigned long timer;
uint8_t analogPin = 34;

Ubidots ubidots(UBIDOTS_TOKEN);

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
    float value = analogRead(analogPin);
    ubidots.add(VARIABLE_LABEL, value);
    ubidots.publish(DEVICE_LABEL);
    Serial.println("Publishing noise data to Ubidots Cloud");
    Serial.print("noise: ");
    Serial.println(value);
    timer = millis();
  }
  ubidots.loop();
}
