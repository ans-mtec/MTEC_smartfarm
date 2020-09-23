#ifndef __CLED_DISABLE_H__
#define __CLED_DISABLE_H__

#include "Adafruit_NeoPixel.h"
#include <EEPROM.h>

#define LED_DISABLE_ADDR          100     // address of the disable list in EEPROM
#define LED_DISABLE_MAX_NUM       16      // max length of the disable list

/*
  A class to disable some LED's pixels.
  Just for testing.
*/

class cLED_Disable{
public:
  cLED_Disable(){}
  
  void start(){
    read();
  }
  
  // turn off the pixels in the disable list
  void apply( Adafruit_NeoPixel &pixels ){
    for(int i=0;i<LED_DISABLE_MAX_NUM;i++){
      if( _disable[i]>=0 )
        pixels.setPixelColor( _disable[i], pixels.Color(0,0,0));
    }
  }
  
  // add a pixel number into the disable list
  void add(const int16_t n){
    for(int i=0;i<LED_DISABLE_MAX_NUM;i++){
      if( n==_disable[i] ){
        Log.warnln("cLED_Disable.add : already exist");
        return;
      }
    }
    for(int i=0;i<LED_DISABLE_MAX_NUM;i++){
      if( _disable[i]<0 ){
        _disable[i] = n;
        Log.infoln("cLED_Disable.add : added");
        write();
        return;
      }
    }
    Log.warnln("cLED_Disable.add : full");
  }
  
  // remove a pixel number from the disable list
  void remove(const int16_t n){
    for(int i=0;i<LED_DISABLE_MAX_NUM;i++){
      if( n==_disable[i] ){
        _disable[i] = -1;
        write();
        Log.infoln("cLED_Disable.remove : removed");
        return;
      }
    }
    Log.warnln("cLED_Disable.remove : not exist");
  }
  
  // clear data in disable list
  void reset(){
    Log.infoln("cLED_Disable.remove");
    for(int i=0;i<LED_DISABLE_MAX_NUM;i++)
      _disable[i] = -1;
    write();
  }
  
  // get disable list
  const int16_t* get(){
    return _disable;
  }
  
  // print disable list
  void print(){
    Serial.print("DISABLE : ");
    for(int i=0;i<LED_DISABLE_MAX_NUM;i++){
      Serial.print( _disable[i] );
      Serial.print(' ');
    }
    Serial.print(" , CRC = ");
    Serial.print(cal_crc());
    Serial.print(" / ");
    Serial.println(read_crc());
  }


protected:

  // calculate CRC of the the disable list in _disable
  uint16_t cal_crc(){
    uint8_t crc = 0;
    for(int i=0;i<LED_DISABLE_MAX_NUM;i++){
      crc = _crc16_update( crc, _disable[i] );
    }
    return crc;
  }
  
  // calculate CRC of the the disable list in EEPROM
  uint16_t read_crc(){
    uint16_t crc;
    uint8_t *p = (uint8_t*)&crc;
    for(int i=0;i<2;i++)
      p[i] = EEPROM.read( LED_DISABLE_ADDR + sizeof(_disable) + i );
    return crc;
  }
  
  // write disable list to EEPROM
  void write(){
    uint8_t *p = (uint8_t*)_disable;
    for(uint8_t i=0;i<sizeof(_disable);i++)
      EEPROM.write( LED_DISABLE_ADDR + i, p[i] );
    uint16_t crc = cal_crc();
    p = (uint8_t*)&crc;
    for(int i=0;i<2;i++)
      EEPROM.write( LED_DISABLE_ADDR + sizeof(_disable) + i, p[i] );
  }
  
  // read disable list from EEPROM
  void read(){
    uint8_t *p = (uint8_t*)_disable;
    for(uint8_t i=0;i<sizeof(_disable);i++)
      p[i] = EEPROM.read( LED_DISABLE_ADDR + i );
    uint16_t crc = read_crc();
    if( crc!=cal_crc() ){
      reset();
    }
  }
  
  // CRC to check if the disable list is valid
  uint16_t _crc16_update(uint16_t crc, uint8_t a){
    int i;
    crc ^= a;
    for (i = 0; i < 8; ++i)
    {
      if (crc & 1)
        crc = (crc >> 1) ^ 0xA001;
      else
        crc = (crc >> 1);
    }
    return crc;
  }

  int16_t _disable[LED_DISABLE_MAX_NUM];
};

#endif

