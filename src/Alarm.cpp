#include "Alarm.h"


Alarm::Alarm(uint8_t _day, uint8_t _hour, uint8_t _minute, bool _light, bool _sound, uint16_t _addr){
    day = _day;
    hour = _hour;
    minute = _minute;
    light = _light;
    sound = _sound;
    address = _addr;

    EEPROMdata = (day << 13) | (hour << 8) | minute;
    if(light){
        EEPROMdata |= 0x80;
    }
    if(sound){
        EEPROMdata |= 0x40;
    }
    if(EEPROM.length() == 0){
        Serial.println("EEPROM length 0, failed to write alarm");
        return;
    }
    if(address + 1 > EEPROM_MAX_ADDR){
        Serial.println("Not enough memory in EEPROM to write alarm");
        return;
    }
    EEPROM.write(address, EEPROMdata);
    Serial.print("Alarm saved to EEPROM on address ");
    Serial.println(address);
}

Alarm::~Alarm(){
    EEPROM.write(address, 255);
    return;
}

uint8_t Alarm::getDay(){
    return day;
}
uint8_t Alarm::getHour(){
    return hour;
}
uint8_t Alarm::getMinute(){
    return minute;
}
bool Alarm::getActive(){
    return active;
}
uint16_t Alarm::getAddress(){
    return address;
}

void Alarm::update(){
    //TODO:
    //Reqrite EEPROM, update EEPROMdata variable
    return;
}