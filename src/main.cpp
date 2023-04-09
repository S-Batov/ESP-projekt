#include <Arduino.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include "Alarm.h"
#include <EEPROM.h>
#include <vector>

const char* ssid = "ISKONOVAC-7a7698";
const char* password = "ISKON2914504818";
const long utcOffsetInSeconds = 3600;
const String daysOfTheWeek[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
ESP8266WebServer server(80);
std::vector<Alarm> Alarms;
IPAddress local_IP(192, 168, 5, 43);
IPAddress gateway(192, 168, 1, 1);

IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);   //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional

int createAlarm(uint8_t, uint8_t, uint8_t, bool, bool);

void handle_onConect(){
  Serial.print("Client connected at: ");
  Serial.print(timeClient.getHours());
  Serial.print(":");
  Serial.print(timeClient.getMinutes());
  Serial.print(":");
  Serial.println(timeClient.getSeconds());

  String data = "";

  for(size_t i = 0; i < Alarms.size(); i++){
    data += Alarms[i].getDay();
    data += ":";
    data += Alarms[i].getHour();
    data += ":";
    data += Alarms[i].getMinute();
    data += ":";
    if(Alarms[i].getLight())
      data += 1;
    else
      data += 0;
    data += ":";
    if(Alarms[i].getSound())
      data += 1;
    else
      data += 0;
    data += "\n";
  }
  data += "\r";
  server.send(200, "text/plain", data);
}

int checkParamValues(String data, String day, String hour, String minute, String light, String sound){
  if(day.toInt() < 0 || day.toInt() >= 7){
    Serial.println("Day value out of range");
    data += "3: Day out of range";
    return 3;
  }
  if(hour.toInt() < 0 || hour.toInt() >= 24){
    Serial.println("Hour value out of range");
    data += "4: Hour out of range";
    return 4;
  }
  if(minute.toInt() < 0 || minute.toInt() >= 60){
    Serial.println("Minute value out of range");
    data += "5: Minute out of range";
    return 5;
  }
  if(light.toInt() < 0 || light.toInt() > 1){
    Serial.println("Light value out of range");
    data += "6: Light out of range";
    return 6;
  }
  if(sound.toInt() < 0 || sound.toInt() > 1){
    Serial.println("Sound value out of range");
    data += "7: Sound out of range";
    return 7;
  }
  return 0; 
}

void handle_setAlarm(){
  // Parameter example
  // http://website.com/path?parameter1=value1&parameter2=value2
  // Expected arguments:
  // Day:     D - uint8_t <0, 6>
  // Hour:    H - uint8_t <0, 23>
  // Minute:  M - uint8_t <0, 59>
  // Light:   L - uint8_t <0, 1>
  // Sound:   S - uint8_t <0, 1>

  Serial.print("Set alarm at: ");
  Serial.print(timeClient.getHours());
  Serial.print(":");
  Serial.print(timeClient.getMinutes());
  Serial.print(":");
  Serial.println(timeClient.getSeconds());

  uint8_t argCount = server.args();
  const uint8_t expectedArgNum = 5;
  const String argNames[] = {"D", "H", "M", "L", "S"};

  String data = "";

  Serial.print("Number of arguments received: ");
  Serial.println(argCount);
  for(uint8_t i = 0; i < server.args(); i++){
    String msg = (String)i + ": " + server.argName(i) + "=" + server.arg(i);
    Serial.println(msg);
  }

  if (argCount != expectedArgNum){
    data += "1: Wrong number of arguments received";
    server.send(400, "text/plain", data);
    return;
  }

  for(uint8_t i = 0; i < expectedArgNum; i++){
    if(argNames[i] != server.argName(i)){
      String msg = "Expected parameter: \"" + argNames[i] + "\" but got: \"" + server.argName(i) + "\"";
      Serial.println(msg);
      data += "2: Argument name mismathc";
      server.send(400, "text/plain", data);
      return;
    }
  }

  if ( checkParamValues(data, server.arg(0), server.arg(1),
            server.arg(2), server.arg(3), server.arg(4)) ){
    server.send(400, "text/plain", data);
    return;
  }

  Serial.println("Arguments are valid");

  uint8_t d = server.arg(0).toInt();
  uint8_t h = server.arg(1).toInt();
  uint8_t m = server.arg(2).toInt();
  bool l = server.arg(3).toInt();
  bool s = server.arg(4).toInt();

  if(createAlarm(d, h, m, l, s)){
    data += "8: Alarm not created";
    server.send(400, "text/plain", data);
    return;
  }
  data += "0: Alarm successfully created";
  server.send(400, "text/plain", data);
  return;
}

void handle_snooze(){
  return;
}
void handle_stop(){
  return;
}
void handle_badRequest(){
  return;
}

void findAlarms(){
  for(uint16_t i = 0; i < EEPROM_MAX_ADDR; i+=2){
    if(EEPROM.read(i) != 255){
      uint16_t data = EEPROM.get(i, data);

      uint8_t day = (data & 0xE000) >> 13;
      uint8_t hour = (data & 0x1F00) >> 8;
      uint8_t minute = data & 0x003F;
      bool light = data & 0x80;
      bool sound = data & 0x40;

      if(day <= 6 && hour < 24 && minute < 60){
        Serial.print("Alarm found on address: ");
        Serial.println(i);
        Alarm al(day, hour, minute, light, sound, i, Alarms.size());
        Alarms.push_back(al);
      }
      else
        Serial.println("Invalid alarm settings found");
    }
  }
}

int createAlarm(uint8_t _day, uint8_t _hour, uint8_t _minute, bool _light, bool _sound){
  if(_day > 6)
    return 3;
  if(_hour >= 24)
    return 4;
  if(_minute >= 60)
    return 5;
  if(Alarms.size()*2 >= EEPROM_MAX_ADDR-1)
    return 8;

  Alarm temp(_day, _hour, _minute, _light, _sound, Alarms.size()*2, Alarms.size());
  Alarms.push_back(temp);
  return 0;
}

bool deleteAlarm(uint16_t _index){
  if(Alarms.size() < _index){
    Serial.println("Index too big - no such alarm");
    return false;
  }
  Alarms.erase(Alarms.begin() + _index);
  for(uint16_t i = 0; i < Alarms.size(); i++){
    Alarms[i].updateIndex(i);
  }
  return true;
}

void handleAlarms(){
  //TODO:
  //Write body
  return;
}

void setup(){
  //Start serial
  Serial.begin(115200);
  while(!Serial){
    continue;
  }
  
  //Get time
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }
  WiFi.begin(ssid, password);
  Serial.print("\n\n");
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }
  Serial.print("\nConnected to WiFi: ");
  Serial.println(ssid);
  Serial.print("With IP: ");
  Serial.println(WiFi.localIP());

  timeClient.begin();

  //Setup server
  server.on("/", handle_onConect);
  server.on("/setAlarm", handle_setAlarm);
  server.on("/snooze", handle_snooze);
  server.on("/stop", handle_stop);
  server.onNotFound(handle_badRequest);
  
  server.begin();
  Serial.println("HTTP server started");

  //Setup EEPROM and alarms
  EEPROM.begin(4096);
  findAlarms();
}

void loop() {
  timeClient.update();
  server.handleClient();
  handleAlarms();

  delay(1000);
}