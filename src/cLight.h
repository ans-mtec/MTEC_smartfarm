#ifndef __CLIGHT_H__
#define __CLIGHT_H__

/*
  Light sensor
  model : Adafruit TSL2591
*/

#include <Wire.h>

// https://github.com/adafruit/Adafruit_Sensor
#include <Adafruit_Sensor.h>

// https://learn.adafruit.com/adafruit-tsl2591/overview
#include "Adafruit_TSL2591.h"

#include "common.h"

class cLight{
public:
  cLight():_tsl(2591),_b_init(false),_b_enable(true){}

  bool start(){
    if( !_b_enable || _b_init )
      return true;

    if (!_tsl.begin()) {
      lcd_err.print("NO LIGHT SENSOR");
      return false;
    }
    //tsl.setGain(TSL2591_GAIN_LOW);     // 1x gain (bright light)
    _tsl.setGain(TSL2591_GAIN_MED);      // 25x gain
    //tsl.setGain(TSL2591_GAIN_HIGH);    // 428x gain

    // Changing the integration time gives you a longer time over which to sense light
    // longer timelines are slower, but are good in very low light situtations!
    _tsl.setTiming(TSL2591_INTEGRATIONTIME_600MS);
    _b_init = true;
    return true;
  }

  // read light level [LUX]
  float read(){
    if(!_b_init || !_b_enable){
      return -1.0;
    }
    // calculate intensity of light in LUX
    uint32_t lum = _tsl.getFullLuminosity();
    uint16_t ir, full;
    ir = lum >> 16;
    full = lum & 0xFFFF;
    return _tsl.calculateLux(full, ir);
  }

  // enable / disable
  inline bool enable(bool b_enable){
    _b_enable = b_enable;
    if( b_enable && !_b_init )
      return start();
    return true;
  }

  // check if enabled
  inline bool is_enabled(){ return _b_enable; }


private:
  Adafruit_TSL2591 _tsl;      // Light sensor instance
  bool _b_init                // True if the sensor is initilized
    , _b_enable;              // True if the sensor is enabled
};

#endif
