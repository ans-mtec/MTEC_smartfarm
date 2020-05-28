
#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "common.h"

// convert string to integer
// return false if error
bool string_to_int(const char *buf, int *val){
  int n = strlen(buf);
  if( n==0 ){
    return false;
  }
  bool b_num = false;
  for(int i=0;i<n;i++){
    char c = buf[i];

    if( c < '0' || c > '9' ){
      if( c!='-' ){
        Log.error("E:string_to_int() : Non-number character found : ");
        Log.errorln(buf);
        return false;
      }
    }
    else{
      b_num = true;
    }
  }
  if( !b_num ){
    Log.error("E:string_to_int() : no digit found : ");
    Log.errorln(buf);
    return false;
  }
  *val = atoi(buf);
  return true;
}
