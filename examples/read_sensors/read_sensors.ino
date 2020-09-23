/*
  A sample program to read sensors data and show on a LCD
*/

#include <MTEC_smartfarm.h>

cSmartFarm smartfarm;

uint32_t time_lcd = 0             // the last time sensor data are shown on LCD (milliseconds)
  , time_wifi_status = -1000000;  // the last time Wi-Fi status is checked (milliseconds)

char wifi_status[32] = "";        // A variable to keep Wi-Fi status

// A funtion to check Wi-Fi status
void get_wifi_status();

void setup(){
  // Use Serial for showing messages from Smartfarm library
  Serial.begin(9600);

  /*
    Set log level to LOG_WARN.
    You can change to LOG_INFO for more information.
  */
  smartfarm.set_log_level( LOG_WARN );

  // Disable uploading data to server since this is a sample program
  smartfarm.enable_upload( false );

  /*
   Set parameters to convert voltage from pH meter to pH value.
   In this case : pH value = 1.94247*voltage + 3.39571
   These parameters drift over time so they should be calibrated every month.
  */
  smartfarm._ph.set_param_volt2pH( 1.94247, 3.39571 );

  /*
    Initialize smartfarm library. Return false if failed.
    Replace "xxxxxxxxxxxx" with your station API key.
  */
  if( !smartfarm.init("xxxxxxxxxxxx") )
    while(1);

  /*
    Set Wi-Fi module's log level to LOG_WARN.
    You can change to LOG_INFO for more information.
  */
  smartfarm._wifi.set_log_level( LOG_WARN );

  Serial.println("Start");
}

void loop(){
  // Read sensor data and upload them to server
  smartfarm.update();

  // check Wi-Fi status
  get_wifi_status();

  // Acquire sensor data and show them on LCD every 2 seconds
  if( (uint32_t)(millis()-time_lcd) >= 2000 ){
    time_lcd = millis();
    // Acquire sensor data
    float temp = smartfarm._temp_humid.read_temperature()
      , humid = smartfarm._temp_humid.read_humidity()
      , light = smartfarm._light.read()
      , ec = smartfarm._ec.read()
      , ph = smartfarm._ph.read();

    // Show sensor data on LCD
    Serial.print("temp:");
    Serial.print(temp);
    Serial.print(", humid:");
    Serial.print(humid);
    Serial.print(", light:");
    Serial.print(light);
    Serial.print(", ec:");
    Serial.print(ec);
    Serial.print(", ph:");
    Serial.println(ph);

    // Clear the LCD
    smartfarm._lcd.clear();

    // Show temperature in the 1st row
    smartfarm._lcd.print(0, 0, "TEMP:");
    smartfarm._lcd.print(5, 0, temp);

    // Show humidity in the 2nd row
    smartfarm._lcd.print(0, 1, "HUMID:");
    smartfarm._lcd.print(6, 1, humid);

    // Show light level in the 3rd row
    smartfarm._lcd.print(0, 2, "LIGHT:");
    smartfarm._lcd.print(6, 2, light);

    // Show Wi-Fi status in the 4th row
    smartfarm._lcd.print(0, 3, "WiFi:");
    smartfarm._lcd.print(5, 3, wifi_status);
  }
}


// A funtion to check Wi-Fi status
void get_wifi_status(){
  // update Wi-Fi status every 5 seconds
  if( uint32_t(millis()-time_wifi_status) < 5000 )
    return;
  time_wifi_status = millis();

  // get Wi-Fi status. It will return a status code.
  uint8_t wifi_status_code;
  if( !smartfarm._wifi.get_status(&wifi_status_code) ){
    wifi_status_code = 99;
  }
  // Convert status code to a string
  // for more details : https://www.arduino.cc/en/Reference/WiFiStatus
  switch(wifi_status_code){
    case WL_NO_SHIELD:
      strcpy( wifi_status, "NO_SHIELD" );
      break;
    case WL_IDLE_STATUS:
      strcpy( wifi_status, "IDLE" );
      break;
    case WL_NO_SSID_AVAIL:
      strcpy( wifi_status, "NO_SSID" );
      break;
    case WL_SCAN_COMPLETED:
      strcpy( wifi_status, "SCAN_COMPLETED" );
      break;
    case WL_CONNECTED:
      strcpy( wifi_status, "CONNECTED" );
      break;
    case WL_CONNECT_FAILED:
      strcpy( wifi_status, "CONNECT FAILED" );
      break;
    case WL_CONNECTION_LOST:
      strcpy( wifi_status, "CONNECTION LOST" );
      break;
    case WL_DISCONNECTED:
      strcpy( wifi_status, "DISCONNECTED" );
      break;
    case 99:
      strcpy( wifi_status, "CANNOT GET STATUS" );
      break;
    default:
      sprintf( wifi_status, "UNKNOWN %d", wifi_status_code );
  }
}
