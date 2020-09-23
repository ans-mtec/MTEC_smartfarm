#ifndef __CLOGBUFFER_H__
#define __CLOGBUFFER_H__

/*
  A class for storing error log messages before uploading to server
*/

#define LOGBUFFER_MAX_NUM   (8)     // the max number of log messages
#define LOGBUFFER_MAX_LEN   (64)    // the max log message length

class cLogBuffer : public Print {
public:
  cLogBuffer(const uint8_t level);

  // write message to the buffer
  template <class T> void print(const uint8_t level, const T &msg){
		if (level <= _level && level!=0){
			Print::print(msg);
		}
	}

	template <class T, class U> void print(const uint8_t level, const T &msg, const U &param){
		if (level <= _level && level!=0){
			Print::print(msg, param);
		}
	}

  template <class T> void println(const uint8_t level, const T &msg){
		if (level <= _level && level!=0){
			Print::println(msg);
		}
	}
  template <class T, class U> void println(const uint8_t level, const T &msg, const U &param){
		if (level <= _level && level!=0){
      Print::println(msg, param);
		}
	}

  // get all log messages from the buffer
  uint16_t get_all_logs(char *str, const uint16_t max_len);

  // write a character to the current buffer
  size_t write(uint8_t c);

  // clear buffer
  void clear();

protected:
  // go next message
  inline void next(){
    _n_buffer = (_n_buffer+1) % LOGBUFFER_MAX_NUM;
    _buffer_len = 0;
  }


  uint8_t _level;         // log level that will be stored & uploaded
  char _buffer[LOGBUFFER_MAX_NUM][LOGBUFFER_MAX_LEN+1]; // log message buffer
  uint16_t _buffer_len;   // length of the message in buffer
  uint8_t _n_buffer;      // current buffer index
};

#endif
