#ifndef __CONSTANT_H__
#define __CONSTANT_H__

// Time interval in milliseconds to upload data to server
const unsigned long INTERVAL_UPLOAD_DATA = 300000;

// host to upload data
const char HOSTNAME[] = "farm.mtec.or.th";

// LCD for user
const uint8_t LCD_ADDR = 0x26    // LCD address (I2C)
  , LCD_COLUMNS = 20             // the number of columns that the display has
  , LCD_ROWS = 4;                // the number of rows that the display has

// LCD for showing error.
const uint8_t LCDERROR_ADDR = 0x27  // LCD address (I2C)
  , LCDERROR_COLUMNS = 16           // the number of columns
  , LCDERROR_ROWS = 2;              // the number of rows

// number of pixels in LED strip
const uint16_t LED_NUMPIXELS = 100;

// length of station API key
const uint16_t STATION_API_KEY_LEN = 12;


// Output pin for LED
const uint8_t PIN_OUTPUT_LED = 4;

// Output pins for relays
const uint8_t PIN_OUTPUT_RELAYS[] = {6,8,10,11,12,13};
const uint8_t PIN_OUTPUT_FAN_TOP = PIN_OUTPUT_RELAYS[4]
  , PIN_OUTPUT_FAN_MIDDLE = PIN_OUTPUT_RELAYS[2]
  , PIN_OUTPUT_FAN_BOTTOM = PIN_OUTPUT_RELAYS[3]
  , PIN_OUTPUT_PUMP_WATER = PIN_OUTPUT_RELAYS[0]
  , PIN_OUTPUT_PUMP_EVAPORATOR = PIN_OUTPUT_RELAYS[1];

// Input pins for sensors
const uint8_t PIN_TEMP_HUMID = 5
  , PIN_BATTERY = A1  // not used
  , PIN_PH = A2
  , PIN_EC = A3
  , PIN_CURRENT = A4;

// Pins for Wi-Fi module
const uint8_t PIN_WIFI_SS = 28
  , PIN_WIFI_ACK = 29
  , PIN_WIFI_RESET = 37;

// Pins for internal use
const uint8_t PIN_INPUT_RELAYS[] = { 22, 23, 24, 25, 26, 27 };
const uint8_t PIN_INPUT_FAN_TOP = PIN_INPUT_RELAYS[4]
  , PIN_INPUT_FAN_MIDDLE = PIN_INPUT_RELAYS[2]
  , PIN_INPUT_FAN_BOTTOM = PIN_INPUT_RELAYS[3]
  , PIN_INPUT_PUMP_EVAPORATOR = PIN_INPUT_RELAYS[1]
  , PIN_INPUT_PUMP_WATER = PIN_INPUT_RELAYS[0];

const uint8_t PIN_INPUT_LEVEL_WATER = -1
  , PIN_INPUT_LEVEL_EVAPORATOR = -1;

#endif
