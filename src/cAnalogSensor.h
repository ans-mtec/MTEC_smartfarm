#ifndef __CANALOGSENSOR_H__
#define __CANALOGSENSOR_H__

/*
 A base class for analog sensor
*/

#include "common.h"

// Maximum buffer size of the data used for calculating average value
#define MAX_ANALOG_BUFFER 128

class cAnalogSensor{
public:
  cAnalogSensor(
    // Analog input pin
    uint8_t pin_out,
    // Interval to read analog data and save to buffer [ms]
    uint32_t read_interval_ms,
    // buffer size for calculating average value. not more than MAX_ANALOG_BUFFER
    uint16_t buffer_size,
    // Percentage of data excluded when calculating average value
    float exclude_percent=20.0
  );

  ~cAnalogSensor();

  // Read analog data and save to buffer
  void update();

  // enable / disable
  inline bool enable(bool b_enable){
    _b_enable = b_enable;
    return true;
  }

  // check if enabled
  inline bool is_enabled(){ return _b_enable; }

  // Print data in buffer. just for debug
  void print_buffer();

protected:

  // return average voltage from sensor
  float get_average_volt();

  bool _b_enable;             // True if the sensor is enabled
  uint32_t _t_last_update     // The last time data updated
    , _read_interval_ms;      // interval to read analog data and save to buffer [ms]
  uint16_t *_buffer           // data buffer
    , _buffer_size            // buffer size
    , _top_buffer             // index of the latest data in buffer
    , _half_exclude_num;      // A half of the number of data excluded when calculating average value
  uint8_t _pin_out;           // analog input pin
};

#endif
