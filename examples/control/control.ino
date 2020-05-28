/*
  ตัวอย่างโปรแกรมอ่านควบคุมพัดลม ปั๊มน้ำระบบระบายความร้อนและแสง LED
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

  /*
    เริ่มการทำงาน smartfarm library หากมีปัญหาจะคืนค่าเป็น false
    ใส่ station API key แทนข้อความ "xxxxxxxxxxxx"
  */

  // ปิดการทำงานของ Wi-Fi และเซนเซอร์เพราะไม่ได้ใช้ในโปรแกรมนี้
  smartfarm._wifi.enable(false);
  smartfarm._temp_humid.enable(false);
  smartfarm._light.enable(false);
  smartfarm._ec.enable(false);
  smartfarm._ph.enable(false);

  // เนื่องจากเป็นโปรแกรมสำหรับทดสอบจึงปิดการ upload ข้อมูลเซนเซอร์ขึ้นเซิฟเวอร์
  smartfarm.enable_upload( false );

  /*
    เริ่มการทำงาน smartfarm library หากล้มเหลวจะคืนค่าเป็น false
    ใส่ station API key แทนข้อความ "xxxxxxxxxxxx"
  */
  if( !smartfarm.init("xxxxxxxxxxxx") )
    while(1);

  // สั่งเปิดปั๊มน้ำระบบระบายความร้อน
  digitalWrite( PIN_OUTPUT_PUMP_EVAPORATOR, true );

  // สั่งเปิดพัดลม
  digitalWrite( PIN_OUTPUT_FAN_TOP, true );     // แถวบน
  digitalWrite( PIN_OUTPUT_FAN_MIDDLE, true );  // แถวกลาง

  // สั่งปิดพัดลม
  digitalWrite( PIN_OUTPUT_FAN_BOTTOM, false );  // แถวล่าง

  // ตั้งจำนวนเม็ดหลอดไฟ LED เป็น 5 เม็ต
  smartfarm._led.set_pixel_num( 5 );

  // เปลี่ยนสี LED โดยให้ความสว่างแสงสีแดง(R)เป็น 50 สีเขียว(G)เป็น 100 สีน้ำเงิน(B)เป็น 250
  // ค่าความสว่าง RGB ที่สามารถเป็นได้ : 0~255
  smartfarm._led.set_color( 50, 100, 250 );
}

void loop(){

}
