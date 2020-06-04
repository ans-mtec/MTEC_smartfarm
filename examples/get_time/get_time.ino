/*
  A sample program to receive date & time from Wi-Fi module
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

  // A variable to receive date & time.
  struct tm timeinfo;

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

  // Receive date & time from Wi-Fi module. Return false if failed.
  Serial.println("Receiving time from Wi-Fi module ...");
  while( !smartfarm.get_time(&timeinfo) ){
    smartfarm.delay(2000);
  }
  

  // Show date & time on Serial monitor
  Serial.print("Time: year=");
  Serial.print(timeinfo.tm_year + 1900);  // tm_year is a number of year since 1900.
  Serial.print(", month=");
  Serial.print(timeinfo.tm_mon + 1);      // tm_mon is 0~11 so you have to add 1
  Serial.print(", day=");
  Serial.print(timeinfo.tm_mday);
  Serial.print(", hour=");
  Serial.print(timeinfo.tm_hour);
  Serial.print(", minute=");
  Serial.print(timeinfo.tm_min);
  Serial.print(", second=");
  Serial.println(timeinfo.tm_sec);
}

void loop(){

}
