#ifndef __CTEMPHUMID_H__
#define __CTEMPHUMID_H__

/*
  Temperature & humidity sensor
  model : DHT22
*/

#include "common.h"

// https://github.com/adafruit/DHT-sensor-library
#include "DHT.h"
#define DHTTYPE DHT22   // DHT 22  (AM2302)

class cTempHumid{
public:
  cTempHumid(int pin):_dht(pin, DHTTYPE)
    ,_humid(0.0)
    ,_temp(0.0)
    ,_t_last_update(0)
    ,_b_enable(true)
    ,_b_init(false)
  {}

  // call this before use
  inline void start(){
    if( _b_enable && !_b_init ){
      _dht.begin();
      _b_init = true;
    }
  }

  // read data and save to variables
  void update(){
    if( !_b_enable || !_b_init )
      return;

    // read data and save to variables
    if( (uint32_t)(millis()-_t_last_update) >= 1000 ){
      _t_last_update = millis();

      float humid, temp;
      humid = _dht.readHumidity();
      // sensor may return error (NaN) at the first read.
      // just skip it.
      if ( isnan(humid) ){
        Log.warnln(F("W:Cannot get humidity"));
        return;
      }
      _humid = humid;

      temp = _dht.readTemperature();
      if ( isnan(temp) ){
        Log.warnln(F("W:Cannot get temperature"));
        return;
      }
      _temp = temp;
    }
  }

  // get humidity [%RH]. Return 0 if error.
  inline float read_humidity(){
    return _humid;
  }

  // get temperature [°C]. Return 0 if error.
  inline float read_temperature(){
    return _temp;
  }

  // enable / disable
  inline bool enable(bool b_enable){
    _b_enable = b_enable;
    if( b_enable && !_b_init ){
      start();
      _humid = _temp = 0.0;
    }
    return true;
  }

  // check if enabled
  inline bool is_enabled(){ return _b_enable; }
  inline bool is_active(){ return _b_enable && _b_init; }


private:
  DHT _dht;                 // sensor instance
  float _humid              // humidity
    , _temp;                // temperature
  uint32_t _t_last_update;  // the last time data are updated
  bool _b_enable            // the sensor is enabled
    , _b_init;              // the sensor is initilized
};

#endif
