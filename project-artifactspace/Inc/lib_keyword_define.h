/**
 ******************************************************************************
 * @file           : lib_keyword_define.h
 * @author         : Shang Huang
 * @brief          : Header for define keyword
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#ifndef LIB_KEYWORD_DEFINE_H_
  #define LIB_KEYWORD_DEFINE_H_

  // Khai báo các thư viện sử dụng chung

  #include <stdint.h>

  // Khai báo các định nghĩa dùng chung

  #define __vo volatile
  #define HIGH 1
  #define LOW 0
  #define SET HIGH
  #define RESET LOW
  #define ENABLE HIGH
  #define DISABLE LOW
  #define SUCCESS HIGH
  #define ERROR LOW
  #define ON HIGH
  #define OFF LOW
  #define TRUE HIGH
  #define FALSE LOW
  #define u unsigned
  #define ul unsigned long
  #define BLANK_REG uint32_t
  #define __weak __attribute__((weak))

  #ifdef  USE_FULL_ASSERT
    #define assert_param(expr) ((expr) ? (void)0u : assert_failed((uint8_t *)__FILE__, __LINE__))
    void assert_failed(uint8_t* file, uint32_t line);
  #else
    #define assert_param(expr) ((void)0u)
  #endif /* USE_FULL_ASSERT */

  // Khai báo kiểu trả về chung

  typedef enum
  {
    STAT_OK       = 0x00U,
    STAT_ERROR    = 0x01U,
    STAT_BUSY     = 0x02U,
    STAT_TIMEOUT  = 0x03U
  } RETR_STAT;

#endif /* LIB_KEYWORD_DEFINE_H_ */

