/**
 ******************************************************************************
 * @file           : lib_keyword_define.h
 * @author         : Shang Huang
 * @brief          : Header for define keywor
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

#define __vo volatile
#define HIGH 1
#define LOW 0
#define SET HIGH
#define RESET LOW
#define ul unsigned long

typedef enum
{
  STAT_OK       = 0x00U,
  STAT_ERROR    = 0x01U,
  STAT_BUSY     = 0x02U,
  STAT_TIMEOUT  = 0x03U
} RETR_STAT;

#endif /* LIB_KEYWORD_DEFINE_H_ */

