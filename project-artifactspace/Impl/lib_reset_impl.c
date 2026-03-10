/*
 * lib_reset_impl.c
 *
 *  Created on: Jan 29, 2026
 *      Author: shanghuang
 */

// Khai báo các thư viện sử dụng chung

  #ifdef UNIT_TEST
    #include "lib_keyword_def.h"
    #include "lib_condition_def.h"
    #include "lib_reset_def.h"
    #include "lib_reset_hal.h"
    #include "header_dependency.h"
  #endif

  #include <stdint.h>
  #include <stdio.h>
  #include <string.h>

  #ifndef UNIT_TEST
    #include "clock/lib_clock_def.h"
    #include "clock/lib_clock_hal.h"
    #include "generic/lib_keyword_def.h"
    #include "generic/lib_condition_def.h"
    #include "reset/lib_reset_def.h"
    #include "reset/lib_reset_hal.h"
  #endif

// Định nghĩa thanh ghi

  #ifndef UNIT_TEST
    __vo BLANK_REG* SCB_AIRCR_REG_PTR = (__vo BLANK_REG *)SCB_AIRCR_REG_ADDR;
  #else
    __vo ui32 mock_aircr_reg = 0;
    __vo BLANK_REG* SCB_AIRCR_REG_PTR = (__vo BLANK_REG *)&mock_aircr_reg;
  #endif

// Định nghĩa các hàm thành phần

  /*
   * Ghi chú:
   * Các hàm của module Reset hiện được triển khai dạng inline trong
   * `lib_reset_hal.h`, nên file impl này đang giữ vai trò khai báo biến
   * thanh ghi dùng chung cho cả môi trường thực thi và unit test.
   */
