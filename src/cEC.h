#ifndef __CEC_H__
#define __CEC_H__

/*
  Electricity conductivity (EC) sensor
  model : PONPE 595-L
*/

#include "cAnalogSensor.h"

#define EC_READ_INTERVAL_MS   1    // interval to read EC data [ms]
#define EC_BUFFER_NUM         20   // number of EC data used to calculate average value


class cEC : public cAnalogSensor{
public:
  cEC(int pin_out):cAnalogSensor(pin_out,
    EC_READ_INTERVAL_MS,
    EC_BUFFER_NUM
  ){}

  // read electricity conductivity [millisiemens]
  float read(){
    if( !_b_enable )
      return 0;

    float volt = get_average_volt();

    // convert volt to current (4-20 mA)
    float mA = volt * (16.0 / 5.0) + 4;

    // convert current to EC (0-2 millisiemens)
    float ec = (mA - 4.0) * 2.0 / 16.0;
    return ec;
  }

};

#endif
