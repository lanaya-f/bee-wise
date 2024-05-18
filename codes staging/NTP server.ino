#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>               
#include <TimeLib.h>                 

const char *ssid     = "sumire";
const char *password = "qqqqqqqq";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "asia.pool.ntp.org", 28800, 60000);

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected!");

  timeClient.begin();
}

void loop() {
  timeClient.update();
  unsigned long unix_epoch = timeClient.getEpochTime();

  int second_ = second(unix_epoch);
  int minute_ = minute(unix_epoch);
  int hour_   = hour(unix_epoch);
  int day_    = day(unix_epoch);
  int month_  = month(unix_epoch);
  int year_   = year(unix_epoch);

  Serial.print("TIME: ");
  Serial.print(hour_);
  Serial.print(":");
  if (minute_ < 10) Serial.print("0");
  Serial.print(minute_);
  Serial.print(":");
  if (second_ < 10) Serial.print("0");
  Serial.println(second_);

  Serial.print("DATE: ");
  Serial.print(day_);
  Serial.print("/");
  Serial.print(month_);
  Serial.print("/");
  Serial.println(year_);

  delay(1000);
}
