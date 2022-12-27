#include <Arduino.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include "Alarm.h"
#include <EEPROM.h>

const char* ssid = "ISKONOVAC-7a7698";
const char* password = "ISKON2914504818";
const long utcOffsetInSeconds = 3600;
const String daysOfTheWeek[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
ESP8266WebServer server(80);


void printLogAtTime(const char* msg){
  Serial.print(msg);
  Serial.print(" at: ");
  Serial.print(timeClient.getHours());
  Serial.print(":");
  Serial.print(timeClient.getMinutes());
  Serial.print(":");
  Serial.println(timeClient.getSeconds());
}

void handle_onConect(){
  //TODO:
  //Send data about current alarm settings

  printLogAtTime("Client connected");
  server.send(200, "text/plain", "Hello world");
  return;
}

void handle_setAlarm(){
  // Parameter example
  // http://website.com/path?parameter1=value1&parameter2=value2
  printLogAtTime("Set alarm");

  uint8_t argCount = server.args();
  const uint8_t expectedArgNum = 4;
  Serial.print("Number of arguments received: ");
  Serial.println(argCount);
  for(uint8_t i = 0; i < server.args(); i++){
    String msg = (String)i + ": " + server.argName(i) + "=" + server.arg(i);
    Serial.println(msg);
  }

  const String argNames[] = {"D", "H", "M", "S"};
  switch (argCount){
  case expectedArgNum:
    for(uint8_t i = 0; i < expectedArgNum; i++){
      if(argNames[i] != server.argName(i)){
        String msg = "Expected parameter: \"" + argNames[i] + "\" but got: \"" + server.argName(i) + "\"";
        Serial.println(msg);
        server.send(400, "text/plain", "Wrong parameters");
      }
      //TODO:
      //Provjerit je li vrijednost svakog parametra broj
      //Ako je, postavit alarm
    }
    server.send(200, "text/plain", "setAlarm");
    return;
  default:
    server.send(400, "text/plain", "Expected 4 parameters: Day,Hour,Minute,Second");
    Serial.print("Expected 4 parameters:\nDay,Hour,Minute,Second\n");
    return;
  }
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
  //TODO:
  //Read EEPROM to find alarms and create them
  return;
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