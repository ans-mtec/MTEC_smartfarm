/*
  A sample program to send a LINE message
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

  // Disable sensors since they are not used in this sample program
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

  /*
    Set Wi-Fi module's log level to LOG_WARN.
    You can change to LOG_INFO for more information.
  */
  smartfarm._wifi.set_log_level( LOG_WARN );
  
  // Wait until Wi-Fi connected.
  Serial.println("Wait until Wi-Fi connected ...");
  while( !smartfarm._wifi.is_connected() ){
    /*
      smartfarm.delay() does the same thing as delay(); 
      stop running program for xx milliseconds.
      The difference is sensor data are still being collected while stopped by smartfarm.delay().
    */
    smartfarm.delay(1000);
  }

  // send a LINE message
  // Replace "xxxxxxxxx" with your LINE's access token
  Serial.println("Sending message to LINE ...");
  if( smartfarm.send_line_message( "xxxxxxxxx", "Message from Smartfarm" ) ){
    Serial.println("LINE message has been sent.");
  }
  else{
    Serial.println("Sending LINE message failed");
  }
}

void loop(){

}
