#ifndef __CLED_H__
#define __CLED_H__

/*
  LED strip
  Library : https://codeload.github.com/adafruit/Adafruit_NeoPixel/zip/master
*/

#include "Adafruit_NeoPixel.h"
#ifdef __AVR__
  #include <avr/power.h>
#endif

#include "cLED_Disable.h"

class cLED{
public:
  cLED():_pixels(LED_NUMPIXELS, PIN_OUTPUT_LED, NEO_GRB + NEO_KHZ800)
    ,_disable()
    ,_num_pixel(LED_NUMPIXELS)
    ,_b_enable(true)
    ,_b_init(false)
    ,_b_change(false)
    ,_rgb_target{0}, _rgb_current{0}
    ,_time_last_update(0)
  {}

  void start(){
    if( !_b_enable || _b_init )
      return;
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
      clock_prescale_set(clock_div_1);
#endif
    _pixels.begin();
    _pixels.setBrightness(255);

    // clear data in buffer and turn off all LEDs
    delay(100);
    _pixels.clear();

    _disable.apply( _pixels );
    for(int j=0;j<10;j++){
      _pixels.show();
      delay(10);
    }

    _disable.start();

    _b_init = true;
  }

  // enable / disable
  inline bool enable(bool b_enable){
    _b_enable = b_enable;
    if( _b_enable && !_b_init )
      start();
    return true;
  }

  // set color of all pixels
  // r,g,b value can be 0 ~ 255
  void set_color(const uint8_t r, const uint8_t g, const uint8_t b){
    for(uint16_t i=0; i<_num_pixel; i++) {
      _pixels.setPixelColor(i, _pixels.Color(r, g, b));
    }
    _rgb_target[0] = r;
    _rgb_target[1] = g;
    _rgb_target[2] = b;
    _pixels.show();
  }
  
  void update(){
    uint32_t t = millis();
    if( uint32_t(t - _time_last_update )>1000 ){
      for(uint16_t i=0; i<_num_pixel; i++) {
        _pixels.setPixelColor(i, _pixels.Color(_rgb_target[0], _rgb_target[1], _rgb_target[2]));
      }
      _disable.apply( _pixels );
      _pixels.show();
      _time_last_update = t;
    }
  }
  
  /*
  // fading LED


  // set color of all pixels
  // r,g,b value can be 0 ~ 255
  void set_color(const uint8_t r, const uint8_t g, const uint8_t b){
    _rgb_target[0] = r;
    _rgb_target[1] = g;
    _rgb_target[2] = b;
    _b_change = true;
  }

  void update(){
    // The speed of fading LED (RGB value per second)
    // RGB values will be changed little by little to prevent power surge
    const int16_t RGB_PER_SEC = 20;

    uint32_t t = millis();

    // check if LED color has been changed
    if( _b_change ){
      // update LED color every 50 milliseconds
      if( uint32_t(t - _time_last_update )>50 ){
        bool b_change = false;  // check if LED need to be updated
        for(int i=0;i<3;i++){
          if( _rgb_target[i]!=_rgb_current[i] ){
            int16_t v;
            // change RGB values little by little to prevent power surge
            if( _rgb_target[i] < _rgb_current[i] ){
              v = int16_t(_rgb_current[i]) - RGB_PER_SEC;
              if( v < _rgb_target[i] )
                v = _rgb_target[i];
            }
            else{
              v = int16_t(_rgb_current[i]) + RGB_PER_SEC;
              if( v > _rgb_target[i] )
                v = _rgb_target[i];
            }
            _rgb_current[i] = v;
            b_change = true;
          }
        }
        if( b_change ){
          // update RGB
          for(uint16_t i=0; i<_num_pixel; i++) {
            _pixels.setPixelColor(i, _pixels.Color(_rgb_current[0], _rgb_current[1], _rgb_current[2]));
          }
          _disable.apply( _pixels );
          _pixels.show();
        }
        else{
          _b_change = false;
        }
        _time_last_update = t;
      }
    }
    else{
      if( uint32_t(t - _time_last_update )>1000 ){
        for(uint16_t i=0; i<_num_pixel; i++) {
          _pixels.setPixelColor(i, _pixels.Color(_rgb_current[0], _rgb_current[1], _rgb_current[2]));
        }
        _disable.apply( _pixels );
        _pixels.show();
        _time_last_update = t;
      }
    }
  }*/


  // set color of the n-th pixel
  // r,g,b value can be 0 ~ 255
  void set_color(const uint16_t n_th, const uint8_t r, const uint8_t g, const uint8_t b){
    if( n_th<_num_pixel ){
      _pixels.setPixelColor( n_th, _pixels.Color(r, g, b));
      _disable.apply( _pixels );
      _pixels.show();
    }
  }

  // get the target RGB set by user
  const uint8_t* get_color(){ return _rgb_target; }

  // change the number of pixels
  void set_pixel_num(uint16_t num){
    _num_pixel = num;
    _pixels.updateLength(num);
  }

  // get the number of pixels
  inline uint16_t get_pixel_num() const {
    return _num_pixel;
  }

  Adafruit_NeoPixel     _pixels;    // LED pixels instance
  cLED_Disable          _disable;   // an instance for disabling some LED pixels

protected:

  uint16_t _num_pixel;              // the number of pixels in LED strip
  bool _b_enable                    // True if LED is enabled
    , _b_init                       // True if initialized
    , _b_change;                    // True if LED pixels' RGB values have been changed
  uint8_t _rgb_target[3]            // target RGB values set by user
    , _rgb_current[3];              // current RGB values
  uint32_t _time_last_update;       // the last time RGB values are updated
};

#endif
