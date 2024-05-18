#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <TimeLib.h>

const char *ssid = "sumire";
const char *password = "qqqqqqqq";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "asia.pool.ntp.org", 28800, 60000);

const int targetHour = 7;
const int targetMinute = 0;
const int targetSecond = 0;

const int relayPin = 5;
bool relayTriggered = false;

void setup() {
  Serial.begin(115200);

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);
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
  digitalWrite(relayPin, HIGH);
  int year_ = year(unix_epoch);
  int hour_ = hour(unix_epoch);
  int minute_ = minute(unix_epoch);
  int second_ = second(unix_epoch);

  int remainingSeconds = ((targetHour * 3600 + targetMinute * 60 + targetSecond) - (hour_ * 3600 + minute_ * 60 + second_));

  if (remainingSeconds < 0) remainingSeconds += 24 * 3600;

  int remainingHours = remainingSeconds / 3600;
  int remainingMinutes = (remainingSeconds % 3600) / 60;
  remainingSeconds %= 60;

  Serial.print("Date: ");
  Serial.print(day(unix_epoch));
  Serial.print(" ");
  printMonthName(month(unix_epoch));
  Serial.print(", ");
  Serial.print(year(unix_epoch));
  Serial.print("       Time: ");
  printDigits(hour_);
  Serial.print(":");
  printDigits(minute_);
  Serial.print(":");
  printDigits(second_);

  Serial.print("      Countdown to next feeding: ");
  Serial.print(remainingHours);
  Serial.print(" hours ");
  Serial.print(remainingMinutes);
  Serial.print(" minutes ");
  Serial.print(remainingSeconds);
  Serial.println(" seconds");

  if (year_ != 1970 && hour_ == targetHour && minute_ == targetMinute && second_ == targetSecond && !relayTriggered) {
    digitalWrite(relayPin, LOW);
    relayTriggered = true;
    delay(7000);
    digitalWrite(relayPin, HIGH);
  } else if (relayTriggered) {
    digitalWrite(relayPin, HIGH);
    relayTriggered = false;
  }

  delay(1000);
}

void printMonthName(int month) {
  switch (month) {
    case 1:
      Serial.print("January");
      break;
    case 2:
      Serial.print("February");
      break;
    case 3:
      Serial.print("March");
      break;
    case 4:
      Serial.print("April");
      break;
    case 5:
      Serial.print("May");
      break;
    case 6:
      Serial.print("June");
      break;
    case 7:
      Serial.print("July");
      break;
    case 8:
      Serial.print("August");
      break;
    case 9:
      Serial.print("September");
      break;
    case 10:
      Serial.print("October");
      break;
    case 11:
      Serial.print("November");
      break;
    case 12:
      Serial.print("December");
      break;
  }
}

void printDigits(int digits) {
  if (digits < 10) {
    Serial.print("0");
  }
  Serial.print(digits);
}
