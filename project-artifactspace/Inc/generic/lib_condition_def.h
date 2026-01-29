/**
 ******************************************************************************
 * @file           : lib_condition_def.h
 * @author         : shanghuang
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

#ifndef LIB_CONDITION_DEF_H_
#define LIB_CONDITION_DEF_H_

// Khai báo các thư viện sử dụng chung

  #include <stdint.h>
  #include "lib_keyword_def.h"

// Khai báo các điều kiện dùng chung

  #define __DEBUG_GET_MODE(__DBG__) DEBUG_MODE == __DBG__
  #define __NULL_PTR_CHECK(__PTR__) ((__PTR__) == NULL)
  #define __RESET_FLAG_CHECK(__FLAG__) ((__FLAG__) == RESET)
  #define __SET_FLAG_CHECK(__FLAG__) ((__FLAG__) == SET)
  #define __RDY_CHECK(__RDY__) ((__RDY__) == STAT_RDY)
  #define __NRDY_CHECK(__NRDY__) ((__NRDY__) == STAT_NRDY)
  #define __DONE_CHECK(__DONE__) ((__DONE__) == STAT_DONE)
  #define __ERROR_CHECK(__ERR__) ((__ERR__) == STAT_ERROR)
  #define __OK_CHECK(__OK__) ((__OK__) == STAT_OK)
  #define __BUSY_CHECK(__BUSY__) ((__BUSY__) == STAT_BUSY)
  #define __DIFF_CHECK(__A__, __B__) ((__A__) != (__B__))

#endif /* LIB_CONDITION_DEF_H_ */
