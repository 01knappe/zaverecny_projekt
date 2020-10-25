#include <Arduino.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char *ssid     = "ssid";
const char *password = "password";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org");

void setup() {
  Serial.begin(115200);
  Serial.print("Pripojovani k ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  timeClient.begin();  

  //GMT +1 (1 * 60 * 60 = 3600)
  timeClient.setTimeOffset(3600);
}

void loop() {
  timeClient.update();
  String formattedTime = timeClient.getFormattedTime();
  int hour = timeClient.getHours();
  int minute = timeClient.getMinutes();
  Serial.print("Cas: ");
  Serial.println(formattedTime);
  Serial.println(hour);
  Serial.println(minute);  
  delay(2000);
}