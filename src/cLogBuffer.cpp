
#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "cLogBuffer.h"

cLogBuffer::cLogBuffer(const uint8_t level):
  _level(level)
{
  clear();
}


// write a character to the current buffer
size_t cLogBuffer::write(uint8_t c) {
  char *buf = &_buffer[_n_buffer][0];

  // escape quotes
  if( c=='\"' ){
    if( _buffer_len<LOGBUFFER_MAX_LEN-4 ){
      buf[_buffer_len++] = '\\';
      buf[_buffer_len++] = '\"';
      return 1;
    }
    else
      return 0;
  }
  else{
    // end message and go next if '\r'
    if( c=='\r' ){
      if( _buffer_len>LOGBUFFER_MAX_LEN-3 )
        _buffer_len = LOGBUFFER_MAX_LEN-3;
      strcpy( buf + _buffer_len, "\\n" );   // end message with newline
      next();
      return 1;
    }
    // do nothing if '\n'
    else if( c=='\n' ){
      return 0;
    }
    else{
      if( _buffer_len<LOGBUFFER_MAX_LEN-3 ){
        buf[_buffer_len++] = c;
        return 1;
      }
      else
        return 0;
    }
  }
}

// get all log messages from the buffer
uint16_t cLogBuffer::get_all_logs(char *str, const uint16_t max_len){
  uint16_t str_len = 0;
  for(uint8_t i=1;i<LOGBUFFER_MAX_NUM;i++){
    const uint8_t n = (_n_buffer+i) % LOGBUFFER_MAX_NUM;
    const char *buf = &_buffer[n][0];
    const uint16_t len = strlen(buf);
    if( len>0 ){
      if( str_len + len >= max_len ){
        memcpy( str + str_len, buf, max_len - str_len - 1 );
        str[max_len-1] = 0;
        return max_len;
      }
      else{
        strcpy( str + str_len, buf );
        str_len+= len;
      }
    }
  }
  return str_len;
}

// clear buffer
void cLogBuffer::clear(){
  _n_buffer = 0;
  _buffer_len = 0;
  memset( _buffer, 0, sizeof(_buffer) );
}
