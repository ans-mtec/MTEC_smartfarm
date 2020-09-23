#ifndef __MTEC_SMARTFARM_H__
#define __MTEC_SMARTFARM_H__

#include "common.h"
#include "constant.h"
#include "cLCD.h"
#include "cTempHumid.h"
#include "cCurrent.h"
#include "cEC.h"
#include "cPH.h"
#include "cLight.h"
#include "cWiFi.h"
#include "cLED.h"
#include "cLogBuffer.h"

class cSmartFarm{

public:
  cSmartFarm();

  // Call this function in your setup() to initilize.
  // API key can be acquired in https://farm.mtec.or.th/stations.html
  bool init(const char *station_api_key);

  /*
   Read sensors' data and upload them to server.
   Call this function in your loop().
  */
  void update();

  /*
   In update() function there may be some processes that takes a long time.
   If you have something that has to be done frequently
     and don't want them to be blocked by update(),
     add your update function by this add_update_function() function.
   Your update function will be called as frequently as possible
     while  update() function is being processed.
   Keep it small or it will block other functions.
  */
  void add_update_function(void (*f)(void));

  // Delay in milliseconds.
  // micro_update() function will be called while delayed to prevent blocking.
  void delay(uint32_t millisec);

  // send a message via LINE App.
  bool send_line_message( const char *access_token, const char *message );

  // get date & time from Wi-Fi module. return false if failed
  inline bool get_datetime(struct tm *p_timeinfo){ return _wifi.get_datetime(p_timeinfo); }

  /*
  set log level shown in Serial Monitor
  default is LOG_WARN
  * LOG_ERROR
  * LOG_WARN
  * LOG_INFO
  */
  inline void set_log_level(int level){ Log.set_level(level); }

  // enable / disable uploading data to server
  inline void enable_upload(bool b_enable){ _b_enable_upload = b_enable; }

  /*
   This function does something that does not take much time
     but have to be done frequently (ex: reading sensors' data).
   Call this function as frequently as possible
     if you do something that blocks a long period of time.
  */
  void micro_update();


  // Instances
  cTempHumid _temp_humid;           // Temperature & humidity sensor instance
  cCurrent _current;                // Current sensor instance
  cEC _ec;                          // EC sensor instance
  cPH _ph;                          // pH sensor instance
  cLight _light;                    // Light sensor instance
  cWiFi _wifi;                      // Wi-Fi instance
  cLED _led;                        // LED strip instance
  cLCD& _lcd;                       // LCD instance
  cLCDError& _lcd_err;              // LCD for showing error's instance
  cLogBuffer _log_buffer;           // A buffer instance to store log messages before uploading to server

protected:

  // upload sensors' data to server
  void upload_data();

  // upload log messages to server
  void upload_log();


  // check if the station API key is valid
  bool check_station_api_key(const char *api_key);
  void (*_fn_user_update)(void);        // user's update function
  uint32_t _t_last_upload_data          // the last time data uploaded to server
    , _t_last_upload_log;
  char _station_api_key[STATION_API_KEY_LEN+2]; // station API key
  bool _b_in_update, _b_in_microupdate; // Used for preventing multiple calls of update() and micro_update()
  bool _b_enable_upload;            // True if uploading data to server is enabled
};

#endif
