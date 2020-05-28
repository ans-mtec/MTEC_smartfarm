
#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "cAnalogSensor.h"

cAnalogSensor::cAnalogSensor(
  uint8_t pin_out,              // analog input pin
  uint32_t read_interval_ms,    // interval to read analog data and save to buffer [ms]
  uint16_t buffer_size,         // buffer size
  float exclude_percent         // percentage of data excluded when calculating average value
):
  _b_enable(true),
  _read_interval_ms(read_interval_ms),
  _buffer(NULL),
  _buffer_size(buffer_size),
  _top_buffer(0),
  _pin_out(pin_out)
{
  if( buffer_size > MAX_ANALOG_BUFFER ){
    Log.error("E:Analog buffer size cannot be larger than ");
    Log.errorln(MAX_ANALOG_BUFFER);
    while(1);
  }

  _t_last_update = millis();
  _buffer = new uint16_t[_buffer_size];
  _half_exclude_num = _buffer_size * exclude_percent * 0.01 * 0.5;
  memset( _buffer, 0, sizeof(uint16_t)*_buffer_size );
}

cAnalogSensor::~cAnalogSensor(){
  if( _buffer )
    delete [] _buffer;
}

// read analog data and save to buffer
void cAnalogSensor::update(){
  if( !_b_enable )
    return;
  if( (uint32_t)(millis()-_t_last_update) >= _read_interval_ms ){
    _t_last_update = millis();
    _buffer[_top_buffer] = analogRead(_pin_out);
    _top_buffer = (_top_buffer+1) % _buffer_size;
  }
}

// print data in buffer. for debug
void cAnalogSensor::print_buffer(){
  for(uint16_t i=0;i<_buffer_size;i++){
    Serial.print(_buffer[i]);
    Serial.print(' ');
  }
  Serial.println();
}

// return average voltage from sensor
float cAnalogSensor::get_average_volt(){
  if( !_b_enable )
    return 0;

  uint16_t buf[MAX_ANALOG_BUFFER];
  memcpy( buf, _buffer, sizeof(uint16_t)*_buffer_size );

  // sort data from small to large
  for(uint16_t i=0;i<_buffer_size-1;i++){
    for(uint16_t j=i+1;j<_buffer_size;j++){
      if(buf[i]>buf[j]){
        uint16_t temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }

  // exclude too large and too small data and calculate average value
  uint32_t sum_value = 0;
  for(uint16_t i=_buffer_size-1-_half_exclude_num;i>=_half_exclude_num;i--)
    sum_value+= buf[i];

  // convert analog value into volt (0-5 V)
  float volt = float(sum_value)*5.0/(1023.0*(float)(_buffer_size-2*_half_exclude_num));
  return volt;
}
