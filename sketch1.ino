#define BLYNK_TEMPLATE_ID "TMPL6PnCYvuxy"
#define BLYNK_TEMPLATE_NAME "watering tree"
#define BLYNK_AUTH_TOKEN "c3zjfPksWyoz5M7o2d6Xcuo9dwA19_H8"

#include <DHT.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TimeLib.h>

#define DHTPIN 2 // Replace this with the pin you connected the OUT pin of DHT11 to
#define DHTTYPE DHT11

char ssid[] = "sieuquyda";
char pass[] = "drstone001";
int relayDuration = 10000; 
int level_pump = 1;
const int relayPin = D6;
boolean active = 0;

BlynkTimer timer;
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(relayPin, OUTPUT);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);
  WiFi.begin(ssid, pass);
  configTime(7 * 3600, 0, "103.130.217.41");
}

void loop() {
  delay(500);
    Blynk.run();
    timer.run();
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

    Blynk.virtualWrite(V0, temperature);
    Blynk.virtualWrite(V1, humidity);
    status(temperature, humidity);
    Blynk.virtualWrite(V3, level_pump);
    Blynk.virtualWrite(V4, active);
  if(active == 1){
    scheduleRelayActivation(5,0);
  }
  else{
    digitalWrite(relayPin, LOW);   
  }
}
void scheduleRelayActivation(int hour1, int minute1) {
  time_t now = time(nullptr);
  struct tm *timeinfo;
  timeinfo = localtime(&now);
  int currentHour = timeinfo->tm_hour;
  int currentMinute = timeinfo->tm_min;
  int currentSec = timeinfo->tm_sec;
  int secondsDifference = (hour1 - currentHour) * 3600 + (minute1 - currentMinute) * 60 + (0 - currentSec);
  Serial.println(secondsDifference);
  if (secondsDifference == 0) {
    activateRelay();
  }
}
void activateRelay() {
  digitalWrite(relayPin, HIGH); 
  timer.setTimeout(level_pump * relayDuration, turnOffRelay); 
}
void turnOffRelay() {
  digitalWrite(relayPin, LOW);
}

void status(float temperature, float humidity){
  String result = "";
  String advice = "";
  int level = 0;
  if (temperature <= 16){
    result += "The temperature is cold";
    level += 1;
  }
  else if (16 < temperature < 35){
    result += "The temperature is good";
    level += 2;
  }
  else {
    result += "The temperature is hot";
    level += 3;
  }
  if(humidity <= 45){
    result += " and the humidity is low.";
    level += 3;
  }
  else if (45 < humidity < 70){
    result += " and the humidity is good.";
    level += 2;
  }
  else{
    result += " and the humidity is high.";
    level += 1;
  }
  if (level < 4){
    advice += "You should put pump in low (lv1).";
  }
  else if (level > 4){
    advice += "You should put pump in high (lv3).";
  }
  else {
    advice += "You should put pump in avg (lv2).";
  }
    Blynk.virtualWrite(V2, result);
    Blynk.virtualWrite(V5, advice);
}

BLYNK_CONNECTED(){
  Blynk.syncVirtual(V3,V4);
}
BLYNK_WRITE(V4){
  active = param.asInt();
}
BLYNK_WRITE(V3){
  level_pump = param.asInt();
}