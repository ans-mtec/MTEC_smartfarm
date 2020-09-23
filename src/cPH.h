#ifndef __CPH_H__
#define __CPH_H__

/*
  pH sensor
  model : Gravity: Analog pH Sensor / Meter Kit For Arduino
*/

#include "cAnalogSensor.h"

#define PH_READ_INTERVAL_MS   1    // an interval to read pH data [ms]
#define PH_BUFFER_NUM         20   // the number of pH data used to calculate average value

class cPH : public cAnalogSensor{
public:
  cPH(int pin_out):cAnalogSensor(pin_out,
    PH_READ_INTERVAL_MS,
    PH_BUFFER_NUM
  )
  ,_param_volt2pH{1.94247, 3.39571}
  {}

  // read pH
  float read(){
    if( !_b_enable )
      return -1.0;

    float volt = get_average_volt();
    if( volt<=0.0 )
      return -1.0;

    float pH = _param_volt2pH[0] * volt + _param_volt2pH[1];
    return pH;
  }

  /*
    Set parameters for converting voltage to pH
    These parameters should be calibrated every month
    pH = a * volt + b
  */
  void set_param_volt2pH(const float a, const float b){
    _param_volt2pH[0] = a;
    _param_volt2pH[1] = b;
  }

private:
  /*
    parameters for converting voltage to pH
    pH = _param_volt2pH[0] * volt + _param_volt2pH[1]
  */
  float _param_volt2pH[2];
};

#endif
