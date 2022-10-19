
#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "cWiFi.h"

cWiFi::cWiFi():_n_serial_buf(0),_fn_update(NULL),_b_enable(true),_b_init(false){
  memset( &_timeinfo, 0, sizeof(_timeinfo) );
  _t_timeinfo = 0;
  _t_last_timeinfo = -INTERVAL_UPDATE_TIME;

  _unixtime = 0;
  _t_unixtime = 0;
  _t_last_unixtime = -INTERVAL_UPDATE_TIME;

  pinMode(PIN_WIFI_RESET, OUTPUT);
  digitalWrite(PIN_WIFI_RESET, HIGH);
}

bool cWiFi::start(){

  if( !_b_enable || _b_init )
    return true;
  Log.info("I:cWiFi::start()\n");
  Serial2.begin(9600);    // use Serial2 to communicate with Wi-Fi module
  while(Serial2.available())
    Serial2.read();

  // send PING command to a Wi-Fi module to check if the Wi-Fi module is active
  for(int k=0;k<2;k++){
    // reset Wi-Fi module
    reset();
    for(int i=0;i<3;i++){
      send_command("PING\r\n");
      if( wait_response( "OK:PING", NULL, 1000 ) ){
        _b_init = true;
        delay(100);
        read_response();
        set_log_level( LOG_WARN );
        delay(200);
        read_response();
        send_command("RECONNECT\r\n");
        return true;
      }
    }
  }
  // No response from Wi-Fi module.
  lcd_err.print("WiFi NO RESPONSE");
  return false;
}



/*
 Wait until got specified responses from the Wi-Fi module.
 Return true if got "ok_msg".
 Return false if got "fail_msg" or timeout.
 If "all_response" variable is not NULL, all response messages will be stored in "all_response"
 "max_response_len" is the maximum length of "all_response"
*/
bool cWiFi::wait_response(const char *ok_msg, const char *fail_msg, const uint32_t timeout, char *all_response, const uint16_t max_response_len){
  if( !_b_enable )
    return false;

  Log.info("I:wait_response\n");
  const uint16_t ok_len = strlen(ok_msg), fail_len = fail_msg ? strlen(fail_msg) : 0;
  const uint32_t t = millis();
  uint16_t response_len = 0;

  _n_serial_buf = 0;
  while( (uint32_t)(millis()-t) < timeout ){
    while(Serial2.available()){
      char c = Serial2.read();
      if( (uint32_t)(millis()-t) >= timeout ){
        break;
      }
      // save response messages in all_response
      if( all_response && response_len < max_response_len-1 ){
        all_response[response_len++] = c;
      }
      if( c=='\r' || c=='\n' ){
        if( _n_serial_buf>0 ){
          _serial_buf[_n_serial_buf] = 0;
          _n_serial_buf = 0;
          Log.info("I:WiFi:");
          Log.infoln(_serial_buf);

          // Return true if got "ok_msg".
          if( strncmp(_serial_buf, ok_msg, ok_len)==0 ){
            if( all_response )
              all_response[response_len] = 0;
            return true;
          }
          // Return false if got "fail_msg".
          else if( fail_msg && strncmp(_serial_buf, fail_msg, fail_len)==0 ){
            Log.warn("W:Got error from WiFi module:");
            Log.warnln(_serial_buf);
            if( all_response )
              all_response[response_len] = 0;
            return false;
          }
        }
      }
      else{
        if( _n_serial_buf>=WIFI_BUFFER_SIZE-1 ){
          _serial_buf[WIFI_BUFFER_SIZE-1] = 0;
          Log.warn("W:Too long msg from WiFi:");
          Log.warnln(_serial_buf);
          _n_serial_buf = 0;
        }
        else{
          _serial_buf[_n_serial_buf++] = c;
        }
      }
    }
    if( _fn_update ){
      _fn_update();
    }
  }
  Log.error("W:Waiting response from WiFi timeout:");
  Log.errorln(ok_msg);
  if( all_response )
    all_response[response_len] = 0;
  return false;
}




// upload data to server
bool cWiFi::upload_data(const char *host, const char *api_key, const tUploadData &upload_data, unsigned long timeout){
  if( !_b_enable || !_b_init )
    return false;

  // GET:{hostname}:1:/api/add_weather?data={upload data};
  update();
  send_command("\r\n");
  send_command("GET:");
  send_command(host, true);
  send_command(":1:/api/add_weather?data={\"api_key\":\"", true);
  send_command(api_key, true);
  send_command('\"', true);
  Serial.flush();
  if( upload_data.b_humidity ){
    send_command(",\"humidity\":", true);
    send_command(upload_data.humidity,true);
  }
  if( upload_data.b_temp ){
    send_command(",\"temp\":", true);
    send_command(upload_data.temp, true);
  }
  if( upload_data.b_light ){
    send_command(",\"light\":", true);
    send_command(upload_data.light, true);
  }
  if( upload_data.b_battery ){
    send_command(",\"battery\":", true);
    send_command(upload_data.battery, true);
  }
  if( upload_data.b_ph ){
    send_command(",\"ph\":", true);
    send_command(upload_data.ph, true);
  }
  if( upload_data.b_ec ){
    send_command(",\"ec\":", true);
    send_command(upload_data.ec, true);
  }
  if( upload_data.b_current ){
    send_command(",\"current\":", true);
    send_command(upload_data.current, true);
  }
  if( upload_data.b_input_fan_top ){
    send_command(",\"input_fan_top\":", true);
    send_command(upload_data.input_fan_top, true);
  }
  if( upload_data.b_input_fan_mid ){
    send_command(",\"input_fan_mid\":", true);
    send_command(upload_data.input_fan_mid, true);
  }
  if( upload_data.b_input_fan_bot ){
    send_command(",\"input_fan_bot\":", true);
    send_command(upload_data.input_fan_bot, true);
  }
  if( upload_data.b_input_pump_plant ){
    send_command(",\"input_pump_plant\":", true);
    send_command(upload_data.input_pump_plant, true);
  }
  if( upload_data.b_input_pump_evap ){
    send_command(",\"input_pump_evap\":", true);
    send_command(upload_data.input_pump_evap, true);
  }
  if( upload_data.b_status_pump_plant ){
    send_command(",\"status_pump_plant\":", true);
    send_command(upload_data.status_pump_plant, true);
  }
  if( upload_data.b_status_pump_evap ){
    send_command(",\"status_pump_evap\":", true);
    send_command(upload_data.status_pump_evap, true);
  }
  if( upload_data.b_input_light ){
    send_command(",\"input_light_r\":", true);
    send_command(upload_data.input_light[0], true);
    send_command(",\"input_light_g\":", true);
    send_command(upload_data.input_light[1], true);
    send_command(",\"input_light_b\":", true);
    send_command(upload_data.input_light[2], true);
    send_command(",\"input_light_w\":", true);
    send_command(upload_data.input_light[3], true);
  }
  send_command("};\r\n", true);
  Log.infoln("I:Waiting response from server ...");
  if( wait_response("R:{\"result\":\"success\"", "R:{\"result\":\"fail\"", timeout) ){
    Log.infoln("I:Data uploaded");
    return true;
  }
  else{
    lcd_err.print("UPLOAD FAILED");
    return false;
  }

  lcd_err.print("UPLOAD TIMEOUT");
  return false;
}

// send GET/POST request to WiFi module.
// See example usage in cSmartFarm::send_line_message() function in MTEC_smartfarm.cpp.
void cWiFi::send_request( const char *host, const char *msg ){
  Log.info("send_request : host=");
  Log.infoln(host);
  if( !_b_enable || !_b_init )
    return;
  send_command("\r\nREQ:");
  send_command(host, true);
  send_command(":1:", true);
  send_command(strlen(msg), true);
  send_command(";\r\n", true);
  send_command(msg, true);
}

// get WiFi status
bool cWiFi::get_status(uint8_t *status){
  if( !_b_enable || !_b_init )
    return false;
  update();

  send_command("\r\nSTATUS\r\n");
  const uint32_t t = millis();
  do{
    if( _fn_update )
      _fn_update();
    if( Serial2.available() ){
      char c = Serial2.read();
      if( c=='\r' || c=='\n' ){
        if( _n_serial_buf>0 ){
          _serial_buf[_n_serial_buf] = 0;
          Log.info("WiFi:");
          Log.infoln(_serial_buf);
          _n_serial_buf = 0;
          if( strncmp( _serial_buf, "OK:STATUS:", 10 )==0 ){
            int val;
            if( string_to_int( _serial_buf + 10, &val ) ){
              *status = val;
              return true;
            }
            else{
              return false;
            }
          }
        }
      }
      else{
        if( _n_serial_buf>=WIFI_BUFFER_SIZE-1 ){
          _serial_buf[WIFI_BUFFER_SIZE-1] = 0;
          Log.warn("W:Too long serial msg:");
          Log.warnln(_serial_buf);
          _n_serial_buf = 0;
        }
        else{
          _serial_buf[_n_serial_buf++] = c;
        }
      }
    }
  }
  while( (uint32_t)(millis()-t) < 3000 );

  Log.errorln("E:get_status timeout");
  return false;
}

// return true if WiFi is connected.
bool cWiFi::is_connected(){
  uint8_t status;
  if( !get_status( &status ) )
    return false;
  return status==WL_CONNECTED;
}

// get time from cache
bool cWiFi::get_datetime(struct tm *p_timeinfo){
  if( !_b_enable || !_b_init ){
    return false;
  }
  if( _unixtime==0 ){
    if( !get_unixtime_from_wifi(&_unixtime) )
      return false;
  }
  uint32_t t = _unixtime + (millis()-_t_unixtime) / 1000 + 7*3600 - UNIX_OFFSET;
  gmtime_r( &t, p_timeinfo);
  return true;
}


// get time from Wi-Fi module. return false if failed
bool cWiFi::get_datetime_from_wifi(struct tm *p_timeinfo){
  if( !_b_enable || !_b_init )
    return false;

  struct tm timeinfo;
  const char WRONG_FORMAT_MSG[] = "E:Wrong time format from Wi-Fi module : ";
  update();
  send_command("\r\nTIME\r\n");
  const uint32_t t = millis();

  // if failed wait for 1 min
  _t_last_timeinfo = t - INTERVAL_UPDATE_TIME + 60000;
  do{
    if( _fn_update )
      _fn_update();
    if( Serial2.available() ){
      char c = Serial2.read();
      if( c=='\r' || c=='\n' ){
        if( _n_serial_buf>0 ){
          _serial_buf[_n_serial_buf] = 0;
          int len = _n_serial_buf;
          _n_serial_buf = 0;
          Log.info("WiFi:");
          Log.infoln(_serial_buf);
          if( strncmp( _serial_buf, "OK:TIME:", 8 )==0 ){
            int val, i1 = 8, cnt = 0;
            _serial_buf[len++] = ':';
            _serial_buf[len] = 0;
            for(int i=i1;i<len;i++){
              if( _serial_buf[i]==':' ){
                _serial_buf[i] = 0;
                if( string_to_int( _serial_buf + i1, &val ) ){
                  switch(cnt++){
                    case 0:
                      timeinfo.tm_year = val - 1900;
                      break;
                    case 1:
                      timeinfo.tm_mon = val - 1;
                      break;
                    case 2:
                      timeinfo.tm_mday = val;
                      break;
                    case 3:
                      timeinfo.tm_hour = val;
                      break;
                    case 4:
                      timeinfo.tm_min = val;
                      break;
                    case 5:
                      timeinfo.tm_sec = val;
                      break;
                  }
                }
                else{
                  Log.error(WRONG_FORMAT_MSG);
                  Log.errorln(_serial_buf);
                  return false;
                }
                i1 = i+1;
              }
            }
            if( cnt==6 ){
              *p_timeinfo = timeinfo;
              _timeinfo = timeinfo;
              _t_timeinfo = _t_last_timeinfo = millis();
              return true;
            }
            else{
              Log.error(WRONG_FORMAT_MSG);
              Log.errorln(_serial_buf);
              return false;
            }
          }
        }
      }
      else{
        if( _n_serial_buf>=WIFI_BUFFER_SIZE-2 ){
          _serial_buf[WIFI_BUFFER_SIZE-2] = 0;
          Log.warn("W:Too long serial msg:");
          Log.warnln(_serial_buf);
          _n_serial_buf = 0;
        }
        else{
          _serial_buf[_n_serial_buf++] = c;
        }
      }
    }
  }
  while( uint32_t(millis()-t) < 5000 );

  Log.infoln("I:get_datetime() timeout");
  return false;
}


// get time from Wi-Fi module. return false if failed
bool cWiFi::get_unixtime_from_wifi(time_t *p_unixtime){
  if( !_b_enable || !_b_init )
    return false;

  const char WRONG_FORMAT_MSG[] = "E:Wrong time format from Wi-Fi module : ";
  send_command("\r\nUTIME\r\n");
  const uint32_t t = millis();

  // if failed wait for 10 sec
  _t_last_unixtime = t - INTERVAL_UPDATE_TIME + 10000;
  do{
    if( _fn_update )
      _fn_update();
    if( Serial2.available() ){
      char c = Serial2.read();
      if( c=='\r' || c=='\n' ){
        if( _n_serial_buf>0 ){
          _serial_buf[_n_serial_buf] = 0;
          int len = _n_serial_buf;
          _n_serial_buf = 0;
          Log.info("WiFi:");
          Log.infoln(_serial_buf);
          if( strncmp( _serial_buf, "OK:UTIME:", 9 )==0 ){
            const int i1 = 9;
            _serial_buf[len] = 0;
            for(int i=i1;i<len;i++){
              if( _serial_buf[i]<'0' || _serial_buf[i]>'9' ){
                Log.error(WRONG_FORMAT_MSG);
                Log.errorln(_serial_buf);
                return false;
              }
            }
            _unixtime = *p_unixtime = strtoul ( _serial_buf + 9, NULL, 0 );
            _t_unixtime = _t_last_unixtime = millis();
            return true;
          }
        }
      }
      else{
        if( _n_serial_buf>=WIFI_BUFFER_SIZE-2 ){
          _serial_buf[WIFI_BUFFER_SIZE-2] = 0;
          Log.warn("W:Too long serial msg:");
          Log.warnln(_serial_buf);
          _n_serial_buf = 0;
        }
        else{
          _serial_buf[_n_serial_buf++] = c;
        }
      }
    }
  }
  while( uint32_t(millis()-t) < 5000 );

  Log.infoln("I:get_datetime() timeout");
  return false;
}

/* set log level for WiFi module.
  default is LOG_WARN
  * LOG_ERROR
  * LOG_WARN
  * LOG_INFO
*/
void cWiFi::set_log_level(const uint8_t level){
  switch(level){
  case LOG_INFO:
    send_command( "LOG:INFO\r\n" );
    break;
  case LOG_WARN:
    send_command( "LOG:WARN\r\n" );
    break;
  case LOG_ERROR:
    send_command( "LOG:ERROR\r\n" );
    break;
  default:
    Log.warn("W:Invalid WiFi log level:");
    Log.warnln(level);
  }
}


bool cWiFi::change_network_profile(const char *ssid, const char *username, const char *password){
  if( !_b_enable || !_b_init )
    return false;

  if( strlen(ssid)>=32 ){
    Log.error("Too long SSID\r\n");
    return false;
  }
  if( username ){
    if( strlen(username)>=32 ){
      Log.error("Too long username\r\n");
      return false;
    }
  }
  if( strlen(password)>=32 ){
    Log.error("Too long password\r\n");
    return false;
  }

  char str[64];

  read_response();

  // SSID
  sprintf( str, "SSID:%s\r\n", ssid );
  send_command(str);
  if( !wait_response( "OK:SSID", NULL, 1000 ) ){
    Log.error("Cannot change SSID\r\n");
    return false;
  }
  delay(100);
  read_response();

  // username
  if( username )
    sprintf( str, "USER:%s\r\n", username );
  else
    strcpy( str, "USER:\r\n" );
  send_command(str);
  if( !wait_response( "OK:USER", NULL, 1000 ) ){
    Log.error("Cannot change username\r\n");
    return false;
  }
  delay(100);
  read_response();

  // password
  sprintf( str, "PASS:%s\r\n", password );
  send_command(str);
  if( !wait_response( "OK:PASS", NULL, 1000 ) ){
    Log.error("Cannot change password\r\n");
    return false;
  }

  delay(100);
  read_response();

  // reconnect
  return reconnect();

}



bool cWiFi::string_to_mac(const char *str, uint8_t mac[6]){
  if( strlen(str)!=6*2+5 ){
    Log.errorln("FAIL:MAC:Invalid length");
    return false;
  }
  const char *p = str;
  for(int i=0;i<6;i++){
    mac[i] = 0;
    for(int j=0;j<2;j++){
      if( p[j]>='0' && p[j]<='9' ){
        mac[i]+= p[j]-'0';
      }
      else if( p[j]>='a' && p[j]<='f' ){
        mac[i]+= p[j]-'a' + 10;
      }
      else if( p[j]>='A' && p[j]<='F' ){
        mac[i]+= p[j]-'A' + 10;
      }
      else{
        Log.error("FAIL:MAC:Invalid char:");
        Log.errorln(p[j]);
        return false;
      }
      if( j==0 )
        mac[i]*= 16;
    }
    if( i<5 && p[2]!=':' ){
      Log.errorln("FAIL:MAC:No colon");
      return false;
    }
    p+=3;
  }
  return true;
}



void cWiFi::mac_to_string( const uint8_t mac[6], char str[18]){
  char *p = str;
  for(int i=0;i<6;i++){
    const uint8_t n[2] = { uint8_t(mac[i] >> 4), uint8_t(mac[i] & 15) };
    for(int j=0;j<2;j++){
      if( n[j] < 10 )
        p[j] = '0'+n[j];
      else
        p[j] = 'A' - 10 + n[j];
    }
    p[2] = ':';
    p+=3;
  }
  p[-1] = 0;
}


// get MAC address of the Wi-Fi module
bool cWiFi::get_mac_address(uint8_t mac[6]){
  if( !_b_enable || !_b_init )
    return false;
  update();

  send_command("\r\nMAC\r\n");
  const uint32_t t = millis();
  do{
    if( _fn_update )
      _fn_update();
    if( Serial2.available() ){
      char c = Serial2.read();
      if( c=='\r' || c=='\n' ){
        if( _n_serial_buf>0 ){
          _serial_buf[_n_serial_buf] = 0;
          Log.info("WiFi:");
          Log.infoln(_serial_buf);
          _n_serial_buf = 0;
          if( strncmp( _serial_buf, "OK:MAC:", 7 )==0 ){
            return string_to_mac( _serial_buf + 7, mac );
          }
        }
      }
      else{
        if( _n_serial_buf>=WIFI_BUFFER_SIZE-1 ){
          _serial_buf[WIFI_BUFFER_SIZE-1] = 0;
          Log.warn("W:Too long serial msg:");
          Log.warnln(_serial_buf);
          _n_serial_buf = 0;
        }
        else{
          _serial_buf[_n_serial_buf++] = c;
        }
      }
    }
  }
  while( (uint32_t)(millis()-t) < 3000 );

  Log.errorln("E:get_mac_address timeout");
  return false;
}



// get MAC address of the Wi-Fi module
void cWiFi::set_mac_address(const uint8_t mac[6]){
  if( !_b_enable || !_b_init )
    return;
  update();

  char str[18];
  mac_to_string( mac, str );
  send_command("\r\nMAC:");
  send_command(str, true);
  send_command("\r\n", true);
}


bool cWiFi::reconnect(){
  if( !_b_enable || !_b_init )
    return false;
  send_command("RECONNECT\r\n");
  if( !wait_response( "OK:RECONNECT", NULL, 1000 ) ){
    Log.error("Cannot reconnect Wi-Fi\r\n");
    return false;
  }
  delay(100);
  read_response();
  return true;
}

bool cWiFi::restart(){
  if( !_b_enable || !_b_init )
    return false;
  send_command("RESTART\r\n");
  if( !wait_response( "OK:RESTART", NULL, 1000 ) ){
    Log.error("Cannot restart Wi-Fi module\r\n");
    return false;
  }
  delay(100);
  read_response();
  return true;
}

// reset Wi-Fi module
void cWiFi::reset(){
  Log.info("I:Resetting WiFi module ...\r\n");
  digitalWrite(PIN_WIFI_RESET, LOW);
  delay(100);
  digitalWrite(PIN_WIFI_RESET, HIGH);
  delay(1000);
}

// read response from WiFi module
void cWiFi::read_response(){
  if( !_b_enable || !_b_init )
    return;
  while( Serial2.available() ){
    char c = Serial2.read();
    if( c=='\r' || c=='\n' ){
      if( _n_serial_buf>0 ){
        _serial_buf[_n_serial_buf] = 0;
        if( _serial_buf[0]=='R' && _serial_buf[1]==':' ){
          Log.info("I:WiFi:");
          Log.infoln(_serial_buf);
        }
        else{
          Log.essential("WiFi:");
          Log.essentialln(_serial_buf);
        }
        _n_serial_buf = 0;
      }
    }
    else{
      if( _n_serial_buf>=WIFI_BUFFER_SIZE-1 ){
        _serial_buf[WIFI_BUFFER_SIZE-1] = 0;
        Log.error("E:Too long serial msg:");
        Log.errorln(_serial_buf);
        _n_serial_buf = 0;
      }
      else{
        _serial_buf[_n_serial_buf++] = c;
      }
    }
  }
}

// read response from WiFi module
void cWiFi::update(){
  if( !_b_enable )
    return;
  if( !_b_init ){
    static uint32_t t_last_init = millis();
    if( millis()-t_last_init>= 1000 ){
      if( !start() ){
        t_last_init = millis();
        return;
      }
      t_last_init = millis();
    }
    else
      return;
  }


  read_response();

  // update time from Wi-Fi module every [INTERVAL_UPDATE_TIME] milliseconds
  /*
  if( uint32_t(millis()-_t_last_timeinfo) >= INTERVAL_UPDATE_TIME ){
    this->get_datetime_from_wifi(&_timeinfo);
  }*/
  if( uint32_t(millis()-_t_last_unixtime) >= INTERVAL_UPDATE_TIME ){
    this->get_unixtime_from_wifi(&_unixtime);
  }
}
