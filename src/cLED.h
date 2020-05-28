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

class cLED{
public:
  cLED():_pixels(LED_NUMPIXELS, PIN_OUTPUT_LED, NEO_GRB + NEO_KHZ800)
    ,_num_pixel(LED_NUMPIXELS)
    ,_b_enable(true)
    ,_b_init(false)
    ,_rgb{0}
  {}

  void start(){
    if( !_b_enable || _b_init )
      return;
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
      clock_prescale_set(clock_div_1);
#endif
    _pixels.begin();
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
    _rgb[0] = r;
    _rgb[1] = g;
    _rgb[2] = b;
    _pixels.show();
  }
  
  // get last RGB set by user
  const uint8_t* get_color(){ return _rgb; }

  // change number of pixel
  void set_pixel_num(uint16_t num){
    _num_pixel = num;
    _pixels.updateLength(num);
  }


  Adafruit_NeoPixel     _pixels;    // LED pixels instance

private:
  uint16_t _num_pixel;              // number of pixels in LED strip
  bool _b_enable                    // True if enabled
    , _b_init;                      // True if initialized
  uint8_t _rgb[3];                  // last RGB set by user
};

#endif
