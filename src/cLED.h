#ifndef __CLED_H__
#define __CLED_H__

/*
  LED panel controlled by Adafruit PCA9685 PWM Servo Driver
  Library : https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library
*/

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

class cLED{
public:
  cLED():_pwm()
    ,_b_enable(true)
    ,_b_init(false)
    ,_rgb_target{0}
  {

  }

  void start(){
    if( !_b_enable || _b_init )
      return;
    _pwm.setOscillatorFrequency(27000000);
    _pwm.setPWMFreq(1600);
    Wire.setClock(10000);
    _b_init = true;
  }

  // enable / disable
  inline bool enable(bool b_enable){
    _b_enable = b_enable;
    if( _b_enable && !_b_init )
      start();
    return true;
  }

  // set color
  // r(red), g(green), b(blue), w(white) value can be 0 ~ 255
  void set_color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t w){
    if( !_b_enable || _b_init )
      return;
    _rgb_target[0] = r;
    _rgb_target[1] = g;
    _rgb_target[2] = b;
    _rgb_target[3] = w;
    for(int i=0;i<4;i++)
      _pwm.setPWM( i, 0, uint32_t(4095)*_rgb_target[i] / 255 );
  }

  inline const uint8_t *get_color(){ return _rgb_target; }

  /******   deprecated  ******/

  inline void set_pixel_num(const uint16_t){}

  // set RGB color for backward compatiblity
  // set white value to 255 if any non-zero color value exists or set white to 0
  inline void set_color(const uint8_t r, const uint8_t g, const uint8_t b){
    set_color(r, g, b, r || g || b ? 255 : 0 );
  }

protected:
  Adafruit_PWMServoDriver _pwm;
  bool _b_enable                    // True if LED is enabled
    , _b_init;                      // True if initialized
  uint8_t _rgb_target[4];           // target RGB values set by user
};

#endif
