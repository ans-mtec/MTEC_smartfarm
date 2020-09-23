
#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include <cLCD.h>
#include <cWiFi.h>

cLCD* p_lcd = NULL;
cLCDError* p_lcd_err = NULL;

cLCD::cLCD(uint8_t addr      // LCD address (I2C)
  , uint8_t cols
  , uint8_t rows
):
  _lcd(addr,cols,rows),
  _cols(LCD_COLUMNS),
  _rows(LCD_ROWS),
  _b_enable(true),
  _b_init(false)
{}

void cLCD::start(){
  if( !_b_enable || _b_init )
    return;

  _lcd.init();
  _lcd.backlight();
  _lcd.home ();
	_lcd.noCursor();
  _b_init = true;
}

void cLCDError::print(const char *msg){
  Log.error("E:");
  Log.errorln(msg);

  if( !_b_enable || !_b_init )
    return;

  clear();
  int len = strlen(msg);
  if( len <= LCDERROR_COLUMNS ){
    _lcd.home();
    _lcd.print(msg);
  }
  else{
    int n_line = len / LCDERROR_COLUMNS;
    if( len % LCDERROR_COLUMNS > 0 )
      n_line++;
    if( n_line > LCDERROR_COLUMNS )
      n_line = LCDERROR_COLUMNS;
    for(int i=0;i<n_line;i++){
      _lcd.setCursor( 0, i );
      _lcd.print( msg + i*LCDERROR_COLUMNS );
    }
  }

  if( _p_wifi ){
    struct tm timeinfo;
    // get date & time from Wi-Fi module
    if( _p_wifi->get_datetime(&timeinfo) ){
      // print timestamp
      char str[16];
      sprintf( str, " %02d/%02d/%02d %02d:%02d"
        , timeinfo.tm_year-100
        , timeinfo.tm_mon+1
        , timeinfo.tm_mday
        , timeinfo.tm_hour
        , timeinfo.tm_min
      );
      _lcd.setCursor( LCDERROR_COLUMNS - strlen(str), LCDERROR_ROWS - 1 );
      _lcd.print( str );
    }
  }
}

void cLCDError::clear_error(){
  if( !_b_enable || !_b_init )
    return;
  clear();
  _lcd.home();
  _lcd.print("NO ERROR");
}
