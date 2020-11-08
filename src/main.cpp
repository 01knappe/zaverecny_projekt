#include <Arduino.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <FastLED.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncTCP.h>

#define NUM_LEDS 7    
#define DATA_PIN D6
CRGB leds[NUM_LEDS];
CRGB color = CRGB::Red; 

const char *ssid     = "ssid";
const char *password = "password";

AsyncWebServer server(80);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org");

byte brightness = 255;

String getTime() {
  String time = timeClient.getFormattedTime();
  Serial.println(time);
  return String(time);
}

void setup() {
  Serial.begin(115200);
  Serial.print("Pripojovani k ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  } 

  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String());
  });

  server.on("/time", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", getTime().c_str());
  });

  server.on("/brightness", HTTP_POST, [](AsyncWebServerRequest *request) {    
    brightness = request->arg("brightness").toInt();    
    request->send_P(200, "text/json", "{\"result\":\"ok\"}");
  });

  timeClient.begin();  
  

  //GMT +1 (1 * 60 * 60 = 3600)
  timeClient.setTimeOffset(3600);

  FastLED.delay(3000);
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  server.begin();
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
  
  FastLED.setBrightness(brightness);
//test ledek
  for (int i = 0; i <= 6; i++) {
    leds[i] = CRGB ( 0, 0, 255);
    FastLED.show();
    delay(40);
  }
  for (int i = 6; i >= 0; i--) {
    leds[i] = CRGB ( 255, 0, 0);
    FastLED.show();
    delay(40);
  }
}