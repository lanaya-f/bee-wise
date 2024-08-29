//NOT FFT!!!
#include <Arduino.h>

const int MIC_PIN = 34;
const int LED_PIN = 2;

unsigned long startTime = 0;
volatile unsigned long period = 0;
volatile boolean pulseDetected = false;

void IRAM_ATTR handleInterrupt() {
  unsigned long currentTime = micros();
  period = currentTime - startTime;
  startTime = currentTime;
  pulseDetected = true;
}

void setup() {
  Serial.begin(115200);
  pinMode(MIC_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(MIC_PIN), handleInterrupt, RISING);
}

void loop() {
  if (pulseDetected) {
    float frequency = 1000000.0 / period;
    Serial.print("Frequency: ");
    Serial.print(frequency);
    Serial.println(" Hz");
    
    pulseDetected = false;
    digitalWrite(LED_PIN, HIGH);
    delay(10);
    digitalWrite(LED_PIN, LOW); 
  }
}
