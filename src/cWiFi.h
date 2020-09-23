#ifndef __CWIFI_H__
#define __CWIFI_H__


#include <time.h>
/*
  A class for communicating with Wi-Fi module
*/

#include "common.h"

#define WIFI_BUFFER_SIZE 512    // Buffer size for storing data from Wi-Fi module

// WiFi status
const uint8_t WL_NO_SHIELD        = 255,
  WL_IDLE_STATUS      = 0,
  WL_NO_SSID_AVAIL    = 1,
  WL_SCAN_COMPLETED   = 2,
  WL_CONNECTED        = 3,
  WL_CONNECT_FAILED   = 4,
  WL_CONNECTION_LOST  = 5,
  WL_DISCONNECTED     = 6;

// inteval to update time from Wi-Fi module
const uint32_t INTERVAL_UPDATE_TIME = 3600000;

class cWiFi{
public:
   cWiFi();

  // call this before use
  bool start();

  // Upload data to server. return false if failed
  bool upload_data( const char *host  // hostname
    , const char *api_key             // station API key
    , const tUploadData &upload_data  // data uploaded
    , unsigned long timeout           // Time limit [ms] for uploading.
  );

  // send GET/POST request to Wi-Fi module.
  // See example usage in cSmartFarm::send_line_message() function in MTEC_smartfarm.cpp.
  void send_request( const char *host, const char *msg );

  /* set log level for WiFi module.
    * LOG_ERROR
    * LOG_WARN
    * LOG_INFO
    Default is LOG_WARN
  */
  void set_log_level(const uint8_t level);

  /*
   Wait untile got specified responses from Wi-Fi module.
   Return true if got "ok_msg".
   Return false if got "fail_msg" or timeout.
   If "all_response" is not NULL, all response messages will be stored in "all_response"
   "max_response_len" is the maximum length of "all_response"
  */
  bool wait_response(const char *ok_msg, const char *fail_msg, const uint32_t timeout
    , char *all_response=NULL, const uint16_t max_response_len=0);

  // get Wi-Fi status
  bool get_status(uint8_t *status);

  // return true if Wi-Fi is connected.
  bool is_connected();

  // get date & time from the internet
  bool get_datetime(struct tm *p_timeinfo);

  // Change SSID, user name and password
  // Set username to NULL if NOT Enterprise mode
  bool change_network_profile(const char *ssid, const char *username, const char *password);

  // Reconnect Wi-Fi
  bool reconnect();

  // Restart Wi-Fi module
  bool restart();


  // enable / disable
  inline bool enable(bool b_enable){
    _b_enable = b_enable;
    if( b_enable )
      return start();
    return true;
  }

  // check if enabled
  inline bool is_enabled(){ return _b_enable; }

  /********  for internal use  ********/

  // Read response from Wi-Fi module. This function will be called in update function.
  void read_response();

  // read response from Wi-Fi module and update real time from Wi-Fi module
  void update();

  // set an update function you want it to be called while Wi-Fi functions proceeded
  inline void set_update_function(void (*fn)(void)){ _fn_update = fn; }

  // send command to Wi-Fi module.
  template <class T> void send_command(const T &msg, bool b_noprefix=false){
    if( !b_noprefix )
      Log.info("I:send_command:");
    Log.info(msg);
    if( _b_enable )
      Serial2.print(msg);
  }

  // reset Wi-Fi module
  void reset();

private:
  // get date & time from Wi-Fi module. return false if failed
  bool get_datetime_from_wifi(struct tm *p_timeinfo);

  // get unix time from Wi-Fi module. return false if failed
  bool get_unixtime_from_wifi(time_t *p_unixtime);

  char _serial_buf[WIFI_BUFFER_SIZE]; // a buffer to store response from Wi-Fi module
  uint16_t _n_serial_buf;             // The latest index of the buffer
  void (*_fn_update)(void);           // update function

  struct tm _timeinfo;                // for storing date & time got from Wi-Fi module
  uint32_t _t_timeinfo                // timestamp when _timeinfo is updated
    , _t_last_timeinfo;               // the last time _timeinfo was updated

  uint32_t _unixtime;                 // for storing unix time got from Wi-Fi module (sec)
  uint32_t _t_unixtime                // timestamp when _unixtime is updated
    , _t_last_unixtime;               // the last time _unixtime was updated

  bool _b_enable                      // True if Wi-Fi module is enabled
    , _b_init;                         // True if initilized
};

#endif
