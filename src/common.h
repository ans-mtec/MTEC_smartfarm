#ifndef __COMMON_H__
#define __COMMON_H__

#include "cLCD.h"

// structure of data uploaded to server
struct tUploadData{
  float humidity       // %RH
    , temp             // Celcius
    , light            // LUX
    , battery          // Voltage
    , ph               // 0~14
    , ec               // Millisiemens
    , current;         // Ampere

  float input_light[4]; // LED (RGB)

  // these data values can be only 0 / 1 ( means OFF / ON )
  uint8_t input_fan_top
    , input_fan_mid
    , input_fan_bot
    , input_pump_plant
    , input_pump_evap
    , status_pump_plant
    , status_pump_evap;

  // be true if the data exist
  bool b_humidity
    , b_temp
    , b_battery
    , b_light
    , b_ph
    , b_ec
    , b_current
    , b_input_fan_top
    , b_input_fan_mid
    , b_input_fan_bot
    , b_input_pump_plant
    , b_input_pump_evap
    , b_status_pump_plant
    , b_status_pump_evap
    , b_input_light;

  tUploadData(){ memset( this, 0, sizeof(tUploadData) ); }
};

// convert string to integer
// return false if error
bool string_to_int(const char *buf, int *val);

#endif
