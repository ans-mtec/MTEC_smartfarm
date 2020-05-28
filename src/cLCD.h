#ifndef __CLCD_H__
#define __CLCD_H__

/*
  Liquid crystal display (LCD)
*/

#include <Wire.h>

// https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library.git
#include "LiquidCrystal_I2C.h"

#include "cLog.h"
#include "constant.h"

class cWiFi;

// A LCD for user.
// You can use this LCD to show any message
class cLCD{
public:
  cLCD(uint8_t addr=LCD_ADDR      // LCD address (I2C)
    , uint8_t cols=LCD_COLUMNS
    , uint8_t rows=LCD_ROWS
  );

  void start();

  // print message at column 'n_col' and row 'n_row'
  template <class T> void print(const uint8_t n_col, const uint8_t n_row, const T &msg){
    if( !_b_enable || !_b_init )
      return;
    if( n_col>=_cols ){
      Log.error("E:Invalid LCD cursor position : ");
      Log.errorln(n_col);
      return;
    }
    if( n_row>=_rows ){
      Log.error("E:Invalid LCD line number : ");
      Log.errorln(n_row);
      return;
    }
    _lcd.setCursor( n_col, n_row );
    _lcd.print(msg);
  }

  // enable & initilize if not initilized
  inline bool enable(bool b_enable){
    _b_enable = b_enable;
    if( b_enable && !_b_init )
      start();
    return true;
  }

  // check if enabled
  inline bool is_enabled(){ return _b_enable; }

  // clear message on the LCD
  inline void clear(){ _lcd.clear(); }

  // get LCD instance for direct usage
  inline LiquidCrystal_I2C& get_lcd(){ return _lcd; }

protected:
  LiquidCrystal_I2C _lcd;     // LCD instance
  uint8_t _cols               // the number of columns that the display has
    , _rows;                  // the number of rows that the display has
  bool _b_enable              // the LCD is enabled
    , _b_init;                // the LCD is initilized
};


// A LCD for showing error message from system.
// User should not use this LCD.
class cLCDError : public cLCD{
public:
  cLCDError():cLCD(LCDERROR_ADDR, LCDERROR_COLUMNS, LCDERROR_ROWS),_p_wifi(NULL){}

  // show error message and timestamp
  void print(const char *msg);

  // clear error message and show "NO ERROR" message
  void clear_error();

  // set WiFi instance to receive time from Wi-Fi module
  inline void set_wifi(cWiFi *p_wifi){ _p_wifi = p_wifi; }

private:
  cWiFi *_p_wifi;
};

extern cLCD lcd;
extern cLCDError lcd_err;

#endif
