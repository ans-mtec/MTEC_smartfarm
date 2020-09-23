#ifndef __CCURRENT_H__
#define __CCURRENT_H__

/*
  Current sensor for measuring power consumption
  model : ACS712-20A
*/

#include "cAnalogSensor.h"

#define CURRENT_READ_INTERVAL_MS    5  // an interval to read current [ms]
#define CURRENT_BUFFER_SIZE         50 // a number of current data used for calculating average value

class cCurrent : public cAnalogSensor{
public:
  cCurrent(uint8_t pin_out):cAnalogSensor(
    pin_out,
    CURRENT_READ_INTERVAL_MS,
    CURRENT_BUFFER_SIZE
  ){}

  // read current [Ampere]
  float read(){
    if( !_b_enable )
      return 0;

    float volt = this->get_average_volt();

    // convert from voltage to ampere
    float amp = (volt - 2.5)/0.136; // Pololu ACS711EX (-15~+15A )
//    float amp = (volt - 2.5)/0.100; // ACS712-20A (-20A~+20A)
//    float amp = (volt - 2.5)/0.090; // ACS711EX (-15~+15A )
//    float amp = (volt - 2.5)/0.066; // ACS714 (-30~+30A )
    return amp >= 0.0 ? amp : 0.0;
  }
};

#endif
