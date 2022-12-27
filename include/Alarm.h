#pragma once
#include <Arduino.h>
#include <EEPROM.h>

#define EEPROM_MAX_ADDR 4095
#define EEPROM_ADDR_UNDEFINED 4096

class Alarm{
public:
  Alarm(uint8_t _day, uint8_t _hour, uint8_t _minute, bool _light, bool _sound, uint16_t _addr);
  ~Alarm();

  uint8_t   getDay();
  uint8_t   getHour();
  uint8_t   getMinute();
  bool      getActive();
  uint16_t  getAddress();

  void update();
private:
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  bool light;
  bool sound;
  uint16_t address;

  uint16_t EEPROMdata = 0xFF;
};