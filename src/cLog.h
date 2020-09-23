#ifndef __CLOG_H__
#define __CLOG_H__

/*
  A class for logging
*/

#include "cLogBuffer.h"

// Log level
#define LOG_ESSENTIAL     0   // always shown
#define LOG_ERROR         1
#define LOG_WARN          2
#define LOG_INFO          3

class cLog{
public:
  cLog(Print *stream):       // Output stream. "Serial" in general
    _level(LOG_WARN),
    _stream(stream),
    _p_log_buffer(NULL)
  {}

  // set log level that will be shown
  void set_level( const uint8_t level ){
    if( level > LOG_INFO ){
      this->print(LOG_ERROR, "E:Invalid log level : ");
      this->println(LOG_ERROR, level);
    }
    else
      _level = level;
  }

  inline uint8_t get_level(){
    return _level;
  }

  // essential
  template <class T> void essential(const T &msg){
		print(LOG_ESSENTIAL, msg);
	}
  template <class T> void essentialln(const T &msg){
		println(LOG_ESSENTIAL, msg);
	}
	template <class T,class U> void essential(const T &msg, const U &param){
		print(LOG_ESSENTIAL, msg, param);
	}
  template <class T,class U> void essentialln(const T &msg, const U &param){
		println(LOG_ESSENTIAL, msg, param);
	}

  // error
  template <class T> void error(const T &msg){
		print(LOG_ERROR, msg);
	}
  template <class T> void errorln(const T &msg){
		println(LOG_ERROR, msg);
	}
	template <class T,class U> void error(const T &msg, const U &param){
		print(LOG_ERROR, msg, param);
	}
  template <class T,class U> void errorln(const T &msg, const U &param){
		println(LOG_ERROR, msg, param);
	}

	// warn
  template <class T> void warn(const T &msg){
		print(LOG_WARN, msg);
	}
  template <class T> void warnln(const T &msg){
		println(LOG_WARN, msg);
	}
	template <class T,class U> void warn(const T &msg, const U &param){
		print(LOG_WARN, msg, param);
	}
  template <class T,class U> void warnln(const T &msg, const U &param){
		println(LOG_WARN, msg, param);
	}

  // info
  template <class T> void info(const T &msg){
		print(LOG_INFO, msg);
	}
  template <class T> void infoln(const T &msg){
		println(LOG_INFO, msg);
	}
	template <class T,class U> void info(const T &msg, const U &param){
		print(LOG_INFO, msg, param);
	}
  template <class T,class U> void infoln(const T &msg, const U &param){
		println(LOG_INFO, msg, param);
	}

  // set log buffer instance
  void set_buffer( cLogBuffer *p_log_buffer ){ _p_log_buffer = p_log_buffer; }

protected:

  template <class T> void print(const uint8_t level, const T &msg){
		if (level <= _level){
			_stream->print(msg);
		}
    if( _p_log_buffer ){
      _p_log_buffer->print(level, msg);
    }
	}

	template <class T, class U> void print(const uint8_t level, const T &msg, const U &param){
		if (level <= _level){
			_stream->print(msg, param);
		}
    if( _p_log_buffer ){
      _p_log_buffer->print(level, msg, param);
    }
	}

  template <class T> void println(const uint8_t level, const T &msg){
		if (level <= _level){
			_stream->println(msg);
		}
    if( _p_log_buffer ){
      _p_log_buffer->println(level, msg);
    }
	}
  template <class T, class U> void println(const uint8_t level, const T &msg, const U &param){
		if (level <= _level){
      _stream->println(msg, param);
		}
    if( _p_log_buffer ){
      _p_log_buffer->println(level, msg, param);
    }
	}

  uint8_t _level;    // log level that will be shown
  Print *_stream;    // Output stream. "Serial" in general
  cLogBuffer *_p_log_buffer;    // an instance for uploading log messages to server
};

extern cLog Log;

#endif
