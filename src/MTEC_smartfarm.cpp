//#define __USE_WIFI_SPI__

#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "./MTEC_smartfarm.h"

#include "./cWiFi.h"
/*
#ifdef __USE_WIFI_SPI__
  #include "cWiFi_SPI.h"
#else
  #include "cWiFi_Serial.h"
#endif
*/

cLog Log(&Serial);
cLCD lcd;
cLCDError lcd_err;
cSmartFarm *p_smartfarm = NULL;

// This function will be called as frequently as possible while WiFi proceeds
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
  _fn_user_update(NULL),
  _t_last_upload(0),
  _station_api_key{0},
  _b_in_update(false),
  _b_in_microupdate(false),
  _b_enable_upload(true)
{
  Log.set_level(LOG_WARN);

/*
#ifdef __USE_WIFI_SPI__
  _p_wifi = new cWiFi_SPI( PIN_WIFI_SS, PIN_WIFI_ACK, PIN_WIFI_RESET );
#else
  _p_wifi = new cWiFi_Serial();
#endif
*/
  p_smartfarm = this;
}

// Call this function first in your setup() function
bool cSmartFarm::init(const char *station_api_key){

  while(Serial.available())
    Serial.read();

  pinMode( PIN_OUTPUT_FAN_TOP, OUTPUT );
  pinMode( PIN_OUTPUT_FAN_MIDDLE, OUTPUT );
  pinMode( PIN_OUTPUT_FAN_BOTTOM, OUTPUT );

  pinMode( PIN_INPUT_FAN_TOP, INPUT );
  pinMode( PIN_INPUT_FAN_MIDDLE, INPUT );
  pinMode( PIN_INPUT_FAN_BOTTOM, INPUT );

  pinMode( PIN_OUTPUT_PUMP_EVAPORATOR, OUTPUT );
  pinMode( PIN_OUTPUT_PUMP_WATER, OUTPUT );

  pinMode( PIN_INPUT_PUMP_WATER, INPUT );
  pinMode( PIN_INPUT_PUMP_EVAPORATOR, INPUT );
  pinMode( PIN_INPUT_LEVEL_WATER, INPUT );
  pinMode( PIN_INPUT_LEVEL_EVAPORATOR, INPUT );

  lcd.start();
  lcd_err.start();
  _temp_humid.start();
  if( !_light.start() )
    return false;
  _led.start();

  if( !check_station_api_key( station_api_key ) )
    return false;
  strcpy( _station_api_key, station_api_key );

  _wifi.set_update_function( fn_update_wifi );
  if( !_wifi.start() )
    return false;

  _t_last_upload = millis();
  lcd_err.clear_error();

  digitalWrite( PIN_OUTPUT_PUMP_WATER, HIGH );
  return true;
}


// Call this function in your loop() function
void cSmartFarm::update(){
  // prevent multiple call
  if( _b_in_update )
    return;
  _b_in_update = true;

  _wifi.update();
  micro_update();

  // upload data to server every [INTERVAL_UPLOAD_DATA] milliseconds
  if( (uint32_t)(millis()-_t_last_upload) >= INTERVAL_UPLOAD_DATA ){
    _t_last_upload = millis();
    upload_data();
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
  _temp_humid.update();

  // call user's update function
  if( _fn_user_update )
    _fn_user_update();

  _b_in_microupdate = false;
}



// Upload data to server
void cSmartFarm::upload_data(){
  // do nothing if station api key does not exist
  if( !_b_enable_upload || !_station_api_key[0] )
    return;

  // read data from sensors and inputs
  tUploadData upload_data;
  upload_data.humidity = _temp_humid.read_humidity();
  upload_data.b_humidity = upload_data.humidity!=0;

  upload_data.temp = _temp_humid.read_temperature();
  upload_data.b_temp = upload_data.temp!=0;

//  upload_data.battery = analogRead( PIN_BATTERY ) * (5.0 / 1023.0);
//  upload_data.b_battery = true;

  upload_data.light = _light.read();
  upload_data.b_light = upload_data.light>=0.0;

  upload_data.ph = _ph.read();
  upload_data.b_ph = _ph.is_enabled();

  upload_data.ec = _ec.read();
  upload_data.b_ec = _ec.is_enabled();

  upload_data.current = _current.read();
  upload_data.b_current = _current.is_enabled();

  upload_data.input_fan_top = digitalRead( PIN_INPUT_FAN_TOP );
  upload_data.b_input_fan_top = true;

  upload_data.input_fan_mid = digitalRead( PIN_INPUT_FAN_MIDDLE );
  upload_data.b_input_fan_mid = true;

  upload_data.input_fan_bot = digitalRead( PIN_INPUT_FAN_BOTTOM );
  upload_data.b_input_fan_bot = true;

  upload_data.input_pump_water = digitalRead( PIN_INPUT_PUMP_WATER );
  upload_data.b_input_pump_water = true;

  upload_data.input_pump_evap = digitalRead( PIN_INPUT_PUMP_EVAPORATOR );
  upload_data.b_input_pump_evap = true;

  const uint8_t *rgb = _led.get_color();
  for(int i=0;i<3;i++)
    upload_data.input_light[i] = rgb[i]/255.0;
  upload_data.b_input_light = true;
/*
  upload_data.input_level_water = digitalRead( PIN_INPUT_LEVEL_WATER );
  upload_data.b_input_level_water = true;

  upload_data.input_level_evap = digitalRead( PIN_INPUT_LEVEL_EVAPORATOR );
  upload_data.b_input_level_evap = true;

*/

  // send data to Wi-Fi module and upload to server
  _wifi.upload_data(HOSTNAME, _station_api_key, upload_data, 20000 );

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
