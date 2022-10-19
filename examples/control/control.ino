/*
  A sample program to control fans, evaporator' water pump and LED
*/

#include <MTEC_smartfarm.h>

cSmartFarm smartfarm;

void setup(){
  // Use Serial for showing message from smartfarm library
  Serial.begin(9600);

  /*
    Set log level to LOG_WARN.
    You can change to LOG_INFO for more information.
  */
  smartfarm.set_log_level( LOG_WARN );

  // Disable Wi-Fi module and sensors since they are not used in this sample program
  smartfarm._wifi.enable(false);
  smartfarm._temp_humid.enable(false);
  smartfarm._light.enable(false);
  smartfarm._ec.enable(false);
  smartfarm._ph.enable(false);

  // Disable uploading data to server
  smartfarm.enable_upload( false );

  /*
    Initialize smartfarm library. Return false if failed.
    Replace "xxxxxxxxxxxx" with your station API key.
  */
  if( !smartfarm.init("xxxxxxxxxxxx") )
    while(1);

  // Turn on evaporator's water pump
  digitalWrite( PIN_OUTPUT_PUMP_EVAPORATOR, true );

  // Turn on fans
  digitalWrite( PIN_OUTPUT_FAN_TOP, true );     // upper
  digitalWrite( PIN_OUTPUT_FAN_MIDDLE, true );  // middle

  // Turn off fans
  digitalWrite( PIN_OUTPUT_FAN_BOTTOM, false );  // lower

  /*
    Change all pixels' RGBW color.
    RGBW varies between 0~255
  */
  smartfarm._led.set_color( 50, 100, 250, 255 ); // R(red) = 50, G(green) = 100, B(blue) = 250, W(white) = 255
}

void loop(){

}
