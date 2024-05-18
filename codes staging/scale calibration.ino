#include <Arduino.h>
#include "soc/rtc.h"
#include "HX711.h"

const int LOADCELL1_DOUT_PIN = 16;
const int LOADCELL1_SCK_PIN = 4;
const int LOADCELL2_DOUT_PIN = 17;
const int LOADCELL2_SCK_PIN = 5;
HX711 scale1;
HX711 scale2;

void setup() {
  Serial.begin(115200);
  rtc_cpu_freq_config_t config;
  rtc_clk_cpu_freq_get_config(&config);
  rtc_clk_cpu_freq_to_config(RTC_CPU_FREQ_80M, &config);
  rtc_clk_cpu_freq_set_config_fast(&config);
  scale1.begin(LOADCELL1_DOUT_PIN, LOADCELL1_SCK_PIN);
  scale2.begin(LOADCELL2_DOUT_PIN, LOADCELL2_SCK_PIN);
}

void loop() {
  if (scale1.is_ready()) {
    scale1.set_scale();    
    Serial.println("Tare... remove any weights from load cell 1.");
    delay(5000);
    scale1.tare();
    Serial.println("Tare done for load cell 1.");
    Serial.println("Place a known weight on load cell 1...");
    delay(5000);
    float reading1 = scale1.get_units(10);
    Serial.print("Result for load cell 1: ");
    Serial.println(reading1);
  } 
  else {
    Serial.println("HX711 not found for load cell 1.");
  }

  if (scale2.is_ready()) {
    scale2.set_scale();    
    Serial.println("Tare... remove any weights from load cell 2.");
    delay(5000);
    scale2.tare();
    Serial.println("Tare done for load cell 2.");
    Serial.println("Place a known weight on load cell 2...");
    delay(5000);
    float reading2 = scale2.get_units(10);
    Serial.print("Result for load cell 2: ");
    Serial.println(reading2);
  } 
  else {
    Serial.println("HX711 not found for load cell 2.");
  }

  delay(1000);
}
