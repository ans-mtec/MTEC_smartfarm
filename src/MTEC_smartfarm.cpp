//#define __USE_WIFI_SPI__

#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "./MTEC_smartfarm.h"

#include "./cWiFi.h"

cLog Log(&Serial);
cLCD lcd;
cLCDError lcd_err;
cSmartFarm *p_smartfarm = NULL;

// This function will be called as frequently as possible while Wi-Fi funtions proceed
void fn_update_wifi(){
  if( p_smartfarm )
    p_smartfarm->micro_update();
}

cSmartFarm::cSmartFarm():
  _temp_humid(PIN_TEMP_HUMID),
  _current(PIN_CURRENT),
  _ec(PIN_EC),
  _ph(PIN_PH),
  _light(),
  _wifi(),
  _led(),
  _lcd(lcd),
  _lcd_err(lcd_err),
  _log_buffer(LOG_WARN),
  _fn_user_update(NULL),
  _t_last_upload_data(0),
  _t_last_upload_log(0),
  _station_api_key{0},
  _b_in_update(false),
  _b_in_microupdate(false),
  _b_enable_upload(true)
{
  Log.set_level(LOG_WARN);
  p_smartfarm = this;
}

// Call this function first in your setup() function
bool cSmartFarm::init(const char *station_api_key){
  Log.set_buffer( &_log_buffer );

  while(Serial.available())
    Serial.read();

  for(int i=0;i<6;i++)
    pinMode( PIN_OUTPUT_RELAYS[i], OUTPUT );

  pinMode( PIN_INPUT_FAN_TOP, INPUT );
  pinMode( PIN_INPUT_FAN_MIDDLE, INPUT );
  pinMode( PIN_INPUT_FAN_BOTTOM, INPUT );

  pinMode( PIN_INPUT_PUMP_PLANT, INPUT );
  pinMode( PIN_INPUT_PUMP_EVAPORATOR, INPUT );

  pinMode( PIN_STATUS_PUMP_PLANT, INPUT );
  pinMode( PIN_STATUS_PUMP_EVAPORATOR, INPUT );

  lcd.start();
  lcd_err.start();
  lcd_err.set_wifi( &_wifi );
  lcd_err.clear_error();

  _temp_humid.start();
  if( !_light.start() )
    return false;
  _led.start();

  if( !check_station_api_key( station_api_key ) )
    return false;
  strcpy( _station_api_key, station_api_key );

  _wifi.set_update_function( fn_update_wifi );
  _wifi.start();

  _t_last_upload_log = _t_last_upload_data = millis();
  _t_last_upload_data-= INTERVAL_UPLOAD_DATA - 30000;
  _t_last_upload_log-= INTERVAL_UPLOAD_LOG;

  digitalWrite( PIN_OUTPUT_PUMP_PLANT, HIGH );
  digitalWrite( PIN_OUTPUT_CAMERA, HIGH );
  return true;
}


// Call this function in your loop() function
void cSmartFarm::update(){
  // prevent multiple call
  if( _b_in_update )
    return;
  _b_in_update = true;

  _wifi.update();
  _temp_humid.update();
  micro_update();

  // upload data to server every [INTERVAL_UPLOAD_DATA] milliseconds
  if( (uint32_t)(millis()-_t_last_upload_data) >= INTERVAL_UPLOAD_DATA ){
    _t_last_upload_data = millis();
    upload_data();
  }

  // upload log messages to server
  if( (uint32_t)(millis()-_t_last_upload_log) >= INTERVAL_UPLOAD_LOG ){
    upload_log();
    _t_last_upload_log = millis();
  }

  // control camera
  {
    static uint32_t t_control_camera = 0;
    if( millis() - t_control_camera >= 300000 ){
      struct tm timeinfo;
      if( get_datetime(&timeinfo) ){
        // turn on camera between 8:00 ~ 16:00
        if( timeinfo.tm_hour < 16 && timeinfo.tm_hour>=8 )
          digitalWrite( PIN_OUTPUT_CAMERA, HIGH );
        else
          digitalWrite( PIN_OUTPUT_CAMERA, LOW );
      }
      t_control_camera = millis();
    }
  }
  _b_in_update = false;
}

/*
  Add a function that does something you don't want them to be blocked by other smartfarm functions.
  This function will be called as frequently as possible while in cSmartFarm::update().
  Keep it small or it will block other functions.
*/
void cSmartFarm::add_update_function(void (*f)(void)){
  _fn_user_update = f;
}


// delay in milliseconds.
// micro_update() function will be called while delayed to prevent blocking.
void cSmartFarm::delay(uint32_t millisec){
  uint32_t t = millis();
  while( (uint32_t)(millis()-t) < millisec ){
    micro_update();
  }
}


// do something that takes not much time but has to be done frequently
void cSmartFarm::micro_update(){
  // prevent multiple call
  if( _b_in_microupdate )
    return;

  _b_in_microupdate = true;

  // Update sensors' data
  _ec.update();
  _ph.update();
  _current.update();

  // call user's update function
  if( _fn_user_update ){
    _fn_user_update();
  }

  _b_in_microupdate = false;
}



// Upload data to server
void cSmartFarm::upload_data(){
  // do nothing if station API key does not exist
  if( !_b_enable_upload || !_station_api_key[0] )
    return;

  // read data from sensors and inputs
  tUploadData upload_data;
  upload_data.humidity = _temp_humid.read_humidity();
  upload_data.b_humidity = upload_data.humidity!=0;

  upload_data.temp = _temp_humid.read_temperature();
  upload_data.b_temp = upload_data.temp!=0;

  upload_data.battery = analogRead( PIN_VIN ) * (5.0 / 1023.0) * (12.0 / 4.8);
  upload_data.b_battery = true;

  upload_data.light = _light.read();
  upload_data.b_light = upload_data.light>=0.0;

  upload_data.ph = _ph.read();
  upload_data.b_ph = upload_data.ph >= 0.0;

  upload_data.ec = _ec.read();
  upload_data.b_ec = upload_data.ec >= 0.0;

  upload_data.current = _current.read();
  upload_data.b_current = _current.is_enabled();

  upload_data.input_fan_top = digitalRead( PIN_INPUT_FAN_TOP );
  upload_data.b_input_fan_top = true;

  upload_data.input_fan_mid = digitalRead( PIN_INPUT_FAN_MIDDLE );
  upload_data.b_input_fan_mid = true;

  upload_data.input_fan_bot = digitalRead( PIN_INPUT_FAN_BOTTOM );
  upload_data.b_input_fan_bot = true;

  upload_data.input_pump_plant = digitalRead( PIN_INPUT_PUMP_PLANT );
  upload_data.b_input_pump_plant = true;

  upload_data.input_pump_evap = digitalRead( PIN_INPUT_PUMP_EVAPORATOR );
  upload_data.b_input_pump_evap = true;

  const uint8_t *rgbw = _led.get_color();
  for(int i=0;i<4;i++)
    upload_data.input_light[i] = rgbw[i]/255.0;
  upload_data.b_input_light = true;

  // have to inverse because of the opto isolator
  upload_data.status_pump_plant = !digitalRead( PIN_STATUS_PUMP_PLANT );
  upload_data.b_status_pump_plant = true;

  upload_data.status_pump_evap = !digitalRead( PIN_STATUS_PUMP_EVAPORATOR );
  upload_data.b_status_pump_evap = true;

  // send data to Wi-Fi module and upload to server
  _wifi.upload_data(HOSTNAME, _station_api_key, upload_data, 20000 );

}

// Upload log messages to server
void cSmartFarm::upload_log(){
  if( !_wifi.is_enabled() ||
      !_station_api_key[0] ||
      strcmp("xxxxxxxxxxxx", _station_api_key)==0 )
    return;

  {
    char buf1[200+LOGBUFFER_MAX_NUM*(LOGBUFFER_MAX_LEN+1)]
      , buf2[200+LOGBUFFER_MAX_NUM*(LOGBUFFER_MAX_LEN+1)];
    uint16_t len = _log_buffer.get_all_logs(buf1, sizeof(buf1));
    if( len==0 ){
      Log.info("I:No log in buffer\r\n");
      return;
    }

    Log.infoln("I:LOGBUFFER");
    Log.infoln(buf1);

    sprintf( buf2, "{\"api_key\":\"%s\",\"log\":\"%s\"}", _station_api_key, buf1 );
    uint16_t body_len = strlen(buf2);

    sprintf( buf1, "POST /api/add_log HTTP/1.1\r\n"
      "Host: %s\r\n"
      "content-type: application/json\r\n"
      "content-length: %d\r\n"
      "Connection: close\r\n\r\n%s", HOSTNAME, body_len, buf2);

    _wifi.send_request(HOSTNAME, buf1);
  }
  Log.info("\r\n");
  Log.infoln("I:Waiting response from server ...");
  if( _wifi.wait_response("R:{\"result\":\"success\"", "R:{\"result\":\"fail\"", 20000) ){
    _log_buffer.clear();
    Log.infoln("I:Log uploaded");
    return;
  }
  else{
    lcd_err.print("LOGUPLOAD FAILED");
    return;
  }
}

// send LINE message
bool cSmartFarm::send_line_message( const char *access_token, const char *message ){
  if( _b_in_update || _b_in_microupdate ){
    Log.errorln("E:You cannot use this function while in update() or micro_update()");
    return false;
  }

  char json[150];
  if( strlen(message)>100 ){
    Log.error("E:Line message cannot be longer than 100 letters\r\n");
    return false;
  }
  if( strlen(access_token)>200 ){
    Log.error("E:Line access_token cannot be longer than 200 letters\r\n");
    return false;
  }
  sprintf( json, "{\"messages\":[{\"type\":\"text\",\"text\":\"%s\"}]}", message );

  char msg[550];
  sprintf( msg, "POST /v2/bot/message/broadcast HTTP/1.1\r\n"
    "Host: api.line.me\r\n"
    "content-type: application/json\r\n"
    "authorization: Bearer %s\r\n"
    "content-length: %d\r\n"
    "Connection: close\r\n\r\n%s", access_token, strlen(json), json );

  _wifi.send_request("api.line.me", msg);
  return _wifi.wait_response("R:{}", "FAIL:", 10000);
}

// Check if the station API key is valid
bool cSmartFarm::check_station_api_key(const char *api_key){
  if( strlen(api_key)!=STATION_API_KEY_LEN ){
    Log.error(F("E:Station api key length must be "));
    Log.errorln( STATION_API_KEY_LEN );
    return false;
  }
  for (int i = STATION_API_KEY_LEN - 1; i >= 0; i--) {
    if( api_key[i] < '0' || api_key[i] > 'z' ){
      Log.error(F("E:Invalid characters found in station api key : "));
      Log.errorln( api_key[i] );
      return false;
    }
  }
  return true;
}
