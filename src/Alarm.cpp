#include "Alarm.h"


Alarm::Alarm(uint8_t _day, uint8_t _hour, uint8_t _minute,
            bool _light, bool _sound, uint16_t _addr, uint16_t _index){
    day = _day;
    hour = _hour;
    minute = _minute;
    light = _light;
    sound = _sound;
    address = _addr;
    EEPROMdata = 0x00;
    index = _index;

    EEPROMdata = (day << 13);
    EEPROMdata |= (hour << 8);
    EEPROMdata |= minute;
    if(light){
        EEPROMdata |= 0x80;
    }
    if(sound){
        EEPROMdata |= 0x40;
    }
    if(address + 1 > EEPROM_MAX_ADDR){
        Serial.println("Not enough memory in EEPROM to write alarm");
        return;
    }
    EEPROM.put(address, EEPROMdata);
    if(EEPROM.commit()){
        Serial.print("Alarm saved to EEPROM on address ");
        Serial.println(address);
    }
    else
        Serial.println("Fail happened durin EEPROM commit");
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
bool Alarm::getLight(){
    return light;
}
bool Alarm::getSound(){
    return sound;
}
uint16_t Alarm::getAddress(){
    return address;
}
uint16_t Alarm::getID(){
    return index;
}

void Alarm::updateIndex(uint16_t newIndex){
    index = newIndex;
}

bool Alarm::update(uint8_t newDay, uint8_t newHour, uint8_t newMinute, bool newLight, bool newSound){
    if(newDay > 6)
        return false;
    if(newHour >= 24)
        return false;
    if(newMinute >= 60)
        return false;
    
    day = newDay;
    hour = newHour;
    minute = newMinute;
    light = newLight;
    sound = newSound;

    EEPROMdata = (day << 13) | (hour << 8) | minute;
    EEPROMdata &= 0xC0;
    if(light){
        EEPROMdata |= 0x80;
    }
    if(sound){
        EEPROMdata |= 0x40;
    }
    EEPROM.put(address, EEPROMdata);
    if(EEPROM.commit()){
        Serial.print("Alarm saved to EEPROM on address ");
        Serial.println(address);
    }
    else{
        Serial.println("Fail happened durin EEPROM commit");
    }
    return true;
}

void Alarm::clear(){
    EEPROMdata = 0xFFFF;
    EEPROM.put(address, EEPROMdata);
    EEPROM.commit();
    Serial.println("Deleted alarm from EEPROM");
}