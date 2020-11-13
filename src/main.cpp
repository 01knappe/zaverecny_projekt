#include <Arduino.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <FastLED.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncTCP.h>
#include <SPI.h>
#include <FS.h>

#define NUM_LEDS 30    
#define DATA_PIN D6
CRGB leds[NUM_LEDS];
//CRGB color = CRGB::Red; 
byte r_val = 255;
byte g_val = 0;
byte b_val = 0;

CRGB color = CRGB(r_val, g_val, b_val);

const char *ssid     = "Knappe_Home";
const char *password = "Odysea1347";

AsyncWebServer server(80);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org");

byte brightness = 100;

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

  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/css/style.css", String());
  });

  server.on("/time", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", getTime().c_str());
  });

  server.on("/brightness", HTTP_POST, [](AsyncWebServerRequest *request) {    
    brightness = request->arg("brightness").toInt();    
    request->send_P(200, "text/json", "{\"result\":\"ok\"}");
  });

  server.on("/jquery.minicolors.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/css/jquery.minicolors.css", String());
  });

  server.on("/index.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/js/index.js", String());
  });

  server.on("/jquery.minicolors.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/js/jquery.minicolors.min.js", String());
  });

  server.on("/jquery.minicolors.png", HTTP_GET, [](AsyncWebServerRequest *request){
  request->send(SPIFFS, "/css/jquery.minicolors.png", "image/png");
  });

   server.on("/color", HTTP_POST, [](AsyncWebServerRequest *request) {    
    r_val = request->arg("r").toInt();
    g_val = request->arg("g").toInt();
    b_val = request->arg("b").toInt();
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
  for (int i = 0; i <= 29; i++) {
    leds[i] = CRGB ( 0, 0, 255);
    FastLED.show();
    delay(40);
  }
  for (int i = 29; i >= 0; i--) {
    leds[i] = CRGB(r_val, g_val, b_val);
    FastLED.show();
    delay(40);
  }
}