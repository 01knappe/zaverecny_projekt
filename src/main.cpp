#include <Arduino.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <FastLED.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncTCP.h>
#include <SPI.h>
#include <FS.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

#define NUM_LEDS 30    
#define DATA_PIN D6
#define DHTTYPE DHT11
CRGB leds[NUM_LEDS];
CRGB color = CRGB::Red; 
byte r_val = 0;
byte g_val = 0;
byte b_val = 0;
bool dotOn = true;
byte mode = 0;
byte temperatureSymbol = 10;
byte degreeSymbol = 11;
byte percent = 12;
int teplotavalue = 20;
int vlhkostvalue = 20;

//CRGB color = CRGB(r_val, g_val, b_val);

const char *ssid     = "ssid";
const char *password = "password";

AsyncWebServer server(80);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org");

byte brightness = 100;

DHT dht(D2, DHTTYPE);

String getTime() {
  String time = timeClient.getFormattedTime();
  //Serial.println(time);
  return String(time);
}

String getTemperature(){
  return String(teplotavalue);  
}

String getHumidity(){
  return String(vlhkostvalue);  
}

void displayDots(CRGB color) {
  if (dotOn) {
    leds[14] = color;
    leds[15] = color;
  } 
  else {
    leds[14] = CRGB::Black;
    leds[15] = CRGB::Black;
  }

  dotOn = !dotOn;  
}

void dotsOff(){
    leds[14] = CRGB::Black;
    leds[15] = CRGB::Black;
}

void displayTime(int index, int number) {

  byte numbers[] = {
    0b00111111, //0    
    0b00000110, //1
    0b01011011, //2
    0b01001111, //3
    0b01100110, //4
    0b01101101, //5
    0b01111101, //6
    0b00000111, //7
    0b01111111, //8
    0b01101111, //9   
    0b00111001, //C
    0b01100011, //°
    0b01011100, //%
  };
  
  color = CRGB(r_val,g_val,b_val);
  for (int i = 0; i < 7; i++) {
    leds[i + index] = ((numbers[number] & 1 << i) == 1 << i) ? color : CRGB::Black;
  }

  //tecky  
  /*leds[14] = color;
  leds[15] = color;*/ 
}

void updateClock(){

  int hour1 = timeClient.getHours() / 10;
  int hour2 = timeClient.getHours() % 10;
  int minute1 = timeClient.getMinutes() / 10;
  int minute2 = timeClient.getMinutes() % 10;

  CRGB color = CRGB(r_val, g_val, b_val);

  displayTime(0, minute2);    
  displayTime(7, minute1);
  displayTime(16, hour2);    
  displayTime(23, hour1);
  displayDots(color);

}

void updateTemperature(){

  int teplota1 = dht.readTemperature() / 10;
  int teplota2 = dht.readTemperature();
  displayTime(0,temperatureSymbol);
  displayTime(7,degreeSymbol);
  displayTime(16,teplota2 % 10);
  displayTime(23,teplota1);
  dotsOff();
}

void updateHumidity(){

  int humidity1 = dht.readHumidity() / 10;
  int humidity2 = dht.readHumidity();
  displayTime(0,percent);
  displayTime(7,degreeSymbol);
  displayTime(16,humidity2 % 10);
  displayTime(23,humidity1);
  dotsOff();
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

   server.on("/clock", HTTP_POST, [](AsyncWebServerRequest *request){
    mode = 0;  
    request->send(200, "text/json", "{\"result\":\"ok\"}");
  });

  server.on("/temperature", HTTP_POST, [](AsyncWebServerRequest *request){
    mode = 1;  
    request->send(200, "text/json", "{\"result\":\"ok\"}");
  });

  server.on("/teplota", HTTP_GET, [](AsyncWebServerRequest * request){
    request->send_P(200, "text/plain", getTemperature().c_str());
  });

  server.on("/humidity", HTTP_POST, [](AsyncWebServerRequest *request){
    mode = 2;  
    request->send(200, "text/json", "{\"result\":\"ok\"}");
  });

  server.on("/vlhkost", HTTP_GET, [](AsyncWebServerRequest * request){
    request->send_P(200, "text/plain", getHumidity().c_str());
  });

  timeClient.begin(); 
  dht.begin(); 

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
  teplotavalue = dht.readTemperature();
  vlhkostvalue = dht.readHumidity();
  /*Serial.print("Cas: ");
  Serial.println(formattedTime);
  Serial.println(hour);
  Serial.println(minute);  
  Serial.println(mode);*/
  delay(2000);

  //updateClock();

  if(mode == 0){
      updateClock();
    } 
  else if(mode == 1){
      updateTemperature();
    } 
  else if(mode == 2){
    updateHumidity();
  }
  FastLED.setBrightness(brightness);
  FastLED.show();
}