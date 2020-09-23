#ifndef __CONSTANT_H__
#define __CONSTANT_H__

// Time interval in milliseconds to upload data and error log messages to server
const uint32_t INTERVAL_UPLOAD_DATA = 180000  // for sensor data
  , INTERVAL_UPLOAD_LOG = 60000;              // for error log messages


// host to upload data
const char HOSTNAME[] = "farm.mtec.or.th";

// LCD for user
const uint8_t LCD_ADDR = 0x26    // LCD address (I2C)
  , LCD_COLUMNS = 20             // the number of columns that the display has
  , LCD_ROWS = 4;                // the number of rows that the display has

// LCD for showing error
const uint8_t LCDERROR_ADDR = 0x27  // LCD address (I2C)
  , LCDERROR_COLUMNS = 16           // the number of columns
  , LCDERROR_ROWS = 2;              // the number of rows

// number of pixels in LED strip
const uint16_t LED_NUMPIXELS = 900;

// length of station API key
const uint16_t STATION_API_KEY_LEN = 12;


// Output pin for LED
const uint8_t PIN_OUTPUT_LED = 4;

// Output pins to control relays
const uint8_t PIN_OUTPUT_RELAYS[] = {6,8,10,11,12,13};
const uint8_t PIN_OUTPUT_FAN_TOP = PIN_OUTPUT_RELAYS[0] // 3 fans in the top row
  , PIN_OUTPUT_FAN_MIDDLE = PIN_OUTPUT_RELAYS[1]        // 3 fans in the middle row
  , PIN_OUTPUT_FAN_BOTTOM = PIN_OUTPUT_RELAYS[2]        // 3 fans in the bottom row
  , PIN_OUTPUT_PUMP_PLANT = PIN_OUTPUT_RELAYS[3]        // water pump for plants
  , PIN_OUTPUT_PUMP_EVAPORATOR = PIN_OUTPUT_RELAYS[4]   // water pump for evaporator
  , PIN_OUTPUT_CAMERA = PIN_OUTPUT_RELAYS[5];           // camera

// Input pins for sensors
const uint8_t PIN_TEMP_HUMID = 5    // Temperature & humidity sensor
  , PIN_PH = A2         // pH meter
  , PIN_EC = A3         // EC meter
  , PIN_CURRENT = A4    // current sensor
  , PIN_VIN = A5;       // Input voltage (12V)

// Pins for Wi-Fi module
const uint8_t PIN_WIFI_SS = 28
  , PIN_WIFI_ACK = 29
  , PIN_WIFI_RESET = 37;

// Pins for internal use
const uint8_t PIN_INPUT_RELAYS[] = { 22, 23, 24, 25, 26, 27 };
const uint8_t PIN_INPUT_FAN_TOP = PIN_INPUT_RELAYS[0]
  , PIN_INPUT_FAN_MIDDLE = PIN_INPUT_RELAYS[1]
  , PIN_INPUT_FAN_BOTTOM = PIN_INPUT_RELAYS[2]
  , PIN_INPUT_PUMP_PLANT = PIN_INPUT_RELAYS[3]
  , PIN_INPUT_PUMP_EVAPORATOR = PIN_INPUT_RELAYS[4];

// Pins to check pumps' status if they are running.
// The signals will be LOW when the pumps are activated by user and there is enough water in the tanks
const uint8_t PIN_STATUS_PUMP_PLANT = 2
  , PIN_STATUS_PUMP_EVAPORATOR = 3;

#endif
