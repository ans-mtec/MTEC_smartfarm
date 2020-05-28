/*
  ตัวอย่างโปรแกรมส่งข้อความไปยัง LINE
*/

#include <MTEC_smartfarm.h>

cSmartFarm smartfarm;

void setup(){
  // เปิดใช้ Serial เพื่อแสดงข้อความจาก smartfarm library
  Serial.begin(9600);

  /*
    เปลี่ยนระดับการแสดงข้อความเป็น "LOG_WARN".
    หากต้องการให้แสดงข้อมูลอย่างละเอียดสามารถเปลี่ยนเป็น "LOG_INFO" ได้
  */
  smartfarm.set_log_level( LOG_WARN );

  // ปิดการทำงานของเซนเซอร์เพราะไม่ได้ใช้ในโปรแกรมนี้
  smartfarm._temp_humid.enable(false);
  smartfarm._light.enable(false);
  smartfarm._ec.enable(false);
  smartfarm._ph.enable(false);

  // เนื่องจากเป็นโปรแกรมสำหรับทดสอบจึงปิดการ upload ข้อมูลเซนเซอร์ขึ้นเซิฟเวอร์
  smartfarm.enable_upload( false );
  
  /*
    เริ่มการทำงาน smartfarm library หากมีปัญหาจะคืนค่าเป็น false
    ใส่ station API key แทนข้อความ "xxxxxxxxxxxx"
  */
  if( !smartfarm.init("xxxxxxxxxxxx") )
    while(1);

  /*
    เปลี่ยนระดับการแสดงข้อความของ Wi-Fi module เป็น "LOG_WARN".
    หากต้องการให้แสดงข้อมูลอย่างละเอียดสามารถเปลี่ยนเป็น "LOG_INFO" ได้
  */
  smartfarm._wifi.set_log_level( LOG_WARN );
  
  // รอจนกว่าจะต่อ Wi-Fi ได้
  Serial.println("Wait until Wi-Fi connected ...");
  while( !smartfarm._wifi.is_connected() ){
    /*
     smartfarm.delay() ทำงานคล้ายกับ delay() คือหยุดการทำงานเป็นเวลา xx millisecond
     ข้อแตกต่างคือ smartfarm.delay() จะทำการเก็บข้อมูลเซนเซอร์ต่างๆในระหว่างหยุดการทำงานนั้น
    */
    smartfarm.delay(1000);
  }

  // ส่งข้อความไปยัง LINE
  // ใส่ Access token ของ LINE แทน "xxxxxxxxx"
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