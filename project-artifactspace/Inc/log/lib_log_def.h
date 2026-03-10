/*
 * lib_log_def.h
 *
 *  Created on: Mar 6, 2026
 *      Author: shanghuang
 */

#ifndef LIB_LOG_DEF_H_
  #define LIB_LOG_DEF_H_

  // Khai báo các thư viện sử dụng chung

    #include <stdio.h>
    #include <stdint.h>
    #include <stdarg.h>

  // Cấu hình mức log hệ thống 

    typedef enum {
      LOG_LEVEL_NONE    = 0,
      LOG_LEVEL_ERROR   = 1,
      LOG_LEVEL_WARNING = 2,
      LOG_LEVEL_INFO    = 3,
      LOG_LEVEL_DEBUG   = 4,
      LOG_LEVEL_VERBOSE = 5
    } LOG_Enum;

  // Khai báo mức log hệ thống

    #ifndef LOG_GLOBAL_LEVEL
      #define LOG_GLOBAL_LEVEL LOG_LEVEL_DEBUG
    #endif

  // Khai báo hàm xử lý lõi

    void LOG_Write(LOG_Enum level, const char* tag, const char* format, ...);

  // Khai báo macro tiện lợi cho các mức log

    #define LOG_N(tag, format, ...) LOG_Write(LOG_LEVEL_NONE, tag, format, ##__VA_ARGS__)
    #define LOG_E(tag, format, ...) LOG_Write(LOG_LEVEL_ERROR, tag, format, ##__VA_ARGS__)
    #define LOG_W(tag, format, ...) LOG_Write(LOG_LEVEL_WARNING, tag, format, ##__VA_ARGS__)
    #define LOG_I(tag, format, ...) LOG_Write(LOG_LEVEL_INFO, tag, format, ##__VA_ARGS__)
    #define LOG_D(tag, format, ...) LOG_Write(LOG_LEVEL_DEBUG, tag, format, ##__VA_ARGS__)
    #define LOG_V(tag, format, ...) LOG_Write(LOG_LEVEL_VERBOSE, tag, format, ##__VA_ARGS__)

#endif /* LIB_LOG_DEF_H_ */
