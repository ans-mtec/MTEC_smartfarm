/*
  ตัวอย่างโปรแกรมอ่านข้อมูลเซนเซอร์และแสดงผลบนจอ LCD
*/

#include <MTEC_smartfarm.h>

cSmartFarm smartfarm;

uint32_t time_lcd = 0             // เวลาที่แสดงค่าใน LCD ครั้งสุดท้าย (milliseconds)
  , time_wifi_status = -1000000;  // เวลาที่ตรวจสอบสถานะของ Wi-Fi ครั้งสุดท้าย (milliseconds)

char wifi_status[32] = "";        // ตัวแปรเก็บค่าสถานะของ Wi-Fi

// ฟังค์ชั่นตรวจสอบสถานะของ Wi-Fi
void get_wifi_status();

void setup(){
  // เปิดใช้ Serial เพื่อแสดงข้อความจาก smartfarm library
  Serial.begin(9600);

  // ลบข้อมูลเก่าที่ค้างอยู่ของ Serial ทิ้ง
  while(Serial.available())
    Serial.read();

  /*
    เปลี่ยนระดับการแสดงข้อความเป็น "LOG_WARN".
    หากต้องการให้แสดงข้อมูลอย่างละเอียดสามารถเปลี่ยนเป็น "LOG_INFO" ได้
  */
  smartfarm.set_log_level( LOG_WARN );

  // เนื่องจากเป็นโปรแกรมสำหรับทดสอบจึงปิดการ upload ข้อมูลเซนเซอร์ขึ้นเซิฟเวอร์
  smartfarm.enable_upload( false );

  /*
    เริ่มการทำงาน smartfarm library หากมีปัญหาจะคืนค่าเป็น false
    ใส่ station API key แทนข้อความ "xxxxxxxxxxxx"
  */
  if( !smartfarm.init("xxxxxxxxxxxx") )
    while(1);

  /*
    เริ่มการทำงาน smartfarm library หากล้มเหลวจะคืนค่าเป็น false
    ใส่ station API key แทนข้อความ "xxxxxxxxxxxx"
  */
  smartfarm._wifi.set_log_level( LOG_WARN );

  Serial.println("Start");
}

void loop(){
  // ทำงานหลายๆอย่างเบื้องหลัง เช่น อ่านค่าเซนเซอร์, ส่งข้อมูลขึ้นเซิฟเวอร์
  smartfarm.update();

  // ตรวจสอบสถานะของ Wi-Fi
  get_wifi_status();

  // อ่านค่าจากเซนเซอร์และแสดงบน LCD ทุก 2 วินาที
  if( (uint32_t)(millis()-time_lcd) >= 2000 ){
    time_lcd = millis();
    // อ่านค่าจากเซนเซอร์
    float temp = smartfarm._temp_humid.read_temperature()
      , humid = smartfarm._temp_humid.read_humidity()
      , light = smartfarm._light.read()
      , ec = smartfarm._ec.read()
      , ph = smartfarm._ph.read();

    // แสดงค่าจากเซนเซอร์บน Serial Monitor
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

    // ลบข้อความบน LCD ทั้งหมด
    smartfarm._lcd.clear();

    // แถวที่ 1 แสดงค่าอุณหภูมิ
    smartfarm._lcd.print(0, 0, "TEMP:");
    smartfarm._lcd.print(5, 0, temp);

    // แถวที่ 2 แสดงค่าความชื้น
    smartfarm._lcd.print(0, 1, "HUMID:");
    smartfarm._lcd.print(6, 1, humid);

    // แถวที่ 3 แสดงค่าความสว่าง
    smartfarm._lcd.print(0, 2, "LIGHT:");
    smartfarm._lcd.print(6, 2, light);

    // แถวที่ 4 แสดงสถานะ Wi-Fi
    smartfarm._lcd.print(0, 3, "WiFi:");
    smartfarm._lcd.print(5, 3, wifi_status);
  }
}

// ตรวจสอบสถานะของ Wi-Fi
void get_wifi_status(){
  // ตรวจสอบสถานะทุก 5 วินาที
  if( uint32_t(millis()-time_wifi_status) < 5000 )
    return;
  time_wifi_status = millis();

  // ค่าสถานะจะถูกส่งกลับมาเป็นตัวเลข สามารถดูรายละเอียดสถานะได้ใน MTEC_smartfarm/cWiFi.h
  uint8_t wifi_status_id;
  if( !smartfarm._wifi.get_status(&wifi_status_id) ){
    wifi_status_id = 99;
  }
  // แปลงตัวเลขสถานะเป็นข้อความ
  switch(wifi_status_id){
    case WL_NO_SHIELD:
      strcpy( wifi_status, "NO_SHIELD" );   // ไม่พบ Wi-Fi module
      break;
    case WL_IDLE_STATUS:
      strcpy( wifi_status, "IDLE" );        // กำลังพยายามเชื่อมต่อ Wi-Fi
      break;
    case WL_NO_SSID_AVAIL:
      strcpy( wifi_status, "NO_SSID" );     // ไม่พบ SSID ของ Wi-Fi ที่ระบุ
      break;
    case WL_SCAN_COMPLETED:
      strcpy( wifi_status, "SCAN_COMPLETED" );  // ทำการค้นหา Wi-Fi เรียบร้อยแล้ว
      break;
    case WL_CONNECTED:
      strcpy( wifi_status, "CONNECTED" );     // เชื่อมต่อ Wi-Fi เรียบร้อยแล้ว
      break;
    case WL_CONNECT_FAILED:
      strcpy( wifi_status, "CONNECT FAILED" );   // เชื่อมต่อ Wi-Fi ล้มเหลว
      break;
    case WL_CONNECTION_LOST:
      strcpy( wifi_status, "CONNECTION LOST" );  // การเชื่อมต่อ Wi-Fi หลุด
      break;
    case WL_DISCONNECTED:
      strcpy( wifi_status, "DISCONNECTED" );   // ตัดการเชื่อมต่อ Wi-Fi
      break;
    case 99:
      strcpy( wifi_status, "CANNOT GET STATUS" );   // ไม่สามารถรับค่าสถานะของ Wi-Fi ได้
      break;
    default:
      sprintf( wifi_status, "UNKNOWN %d", wifi_status_id );   // พบค่าสถานะของ Wi-Fi ที่ไม่รู้จัก
  }
}
