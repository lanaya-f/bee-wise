#include "DHT.h"
#include <WiFi.h>
#include <Arduino.h>
#include <PubSubClient.h>
#include "UbidotsEsp32Mqtt.h"
#include "HX711.h"
#include "soc/rtc.h"

const char *UBIDOTS_TOKEN = "BBUS-JXaJGPPs6nxpIOdHtkEAy0aKiiWuD3";
const char *WIFI_SSID = "sumire";
const char *WIFI_PASS = "qqqqqqqq";
const char *DEVICE_LABEL = "BEEWISE";
const char *VARIABLE_LABEL_T = "tsense";
const char *VARIABLE_LABEL_H = "hsense";
const char *VARIABLE_LABEL_N = "nsense";
const char *VARIABLE_LABEL_W = "wsense";
const char *VARIABLE_LABEL_I = "isense";

const unsigned long PUBLISH_FREQUENCY = 2000;

unsigned long timer;
#define hpin 22
#define DHTTYPE DHT22
#define MIC_PIN 34
#define LED_PIN 3
#define LOADCELL_DOUT_PIN 16
#define LOADCELL_SCK_PIN 4

Ubidots ubidots(UBIDOTS_TOKEN);
DHT dht(hpin, DHTTYPE);
HX711 scale;

volatile bool pulseDetected = false;
unsigned long pulseStartTime = 0;
unsigned long pulseEndTime = 0;
unsigned long pulseDuration = 0;
unsigned long frequency = 0;

void IRAM_ATTR handleInterrupt() {
  if (!pulseDetected) {
    pulseStartTime = micros();
    pulseDetected = true;
  } else {
    pulseEndTime = micros();
    pulseDetected = false;
    pulseDuration = pulseEndTime - pulseStartTime;
    frequency = 1000000 / pulseDuration;
    if (frequency < 100 || frequency > 1000) {
      frequency = 0;
    }
  }
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();
  dht.begin();
  setupHX711();
  pinMode(MIC_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(MIC_PIN), handleInterrupt, RISING);
  timer = millis();
}

void setupHX711() {
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(111.142);
  scale.tare();
}

void loop() {
  if (!ubidots.connected()) {
    ubidots.reconnect();
  }
  if (millis() - timer >= PUBLISH_FREQUENCY) {
    float humid = dht.readHumidity();
    float temp = dht.readTemperature();
    float hic = dht.computeHeatIndex(temp, humid, false);
    float weight = scale.get_units(10);

    digitalWrite(LED_PIN, HIGH);
    delay(1000);
    digitalWrite(LED_PIN, LOW);
    Serial.println("Publishing data to Ubidots Cloud");
    if (frequency > 0) {
      Serial.print("Frequency: ");
      Serial.print(frequency);
      Serial.println(" Hz");
    } else {
      Serial.println("No pulse detected");
      }
        Serial.print(F("Temperature: "));
        Serial.print(temp);
        Serial.println(F("°C "));
        Serial.print(F("Humidity: "));
        Serial.print(humid);
        Serial.println(F("%"));
        Serial.print(F("Heat index: "));
        Serial.print(hic);
        Serial.println(F("°C "));
        Serial.print(F("Weight: "));
        Serial.print(weight);
        Serial.println(F(" grams"));
        Serial.println("--------------------------------");
    ubidots.add(VARIABLE_LABEL_T, temp);
    ubidots.add(VARIABLE_LABEL_H, humid);
    ubidots.add(VARIABLE_LABEL_I, hic);
    ubidots.add(VARIABLE_LABEL_N, frequency);
    ubidots.add(VARIABLE_LABEL_W, weight);
    ubidots.publish(DEVICE_LABEL);

    timer = millis();
  }
  ubidots.loop();
  scale.power_down();
  delay(5000);
  scale.power_up();
}
