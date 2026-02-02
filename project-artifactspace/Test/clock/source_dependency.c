/*
 * source_dependency.c
 *
 *  Created on: Feb 2, 2026
 *      Author: shanghuang
 */

// Khai báo các thư viện cho unit test

  #include "generic/lib_keyword_def.h"
  #include "clock/header_dependency.h"

// Khai báo các dependency variables của các hàm cần định nghĩa

  RETR_STAT IWDG_Init_Expect = STAT_DONE;
  ui IWDG_Start_Called = 0;
  ui IWDG_Reload_Called = 0;

// Định nghĩa các hàm mock tương ứng

  RETR_STAT IWDG_Init(IWDG_Init_Param *init_param) {
    return IWDG_Init_Expect;
  }

  void IWDG_Start(void) {
    IWDG_Start_Called++;
  }

  void IWDG_Reload(void) {
    IWDG_Reload_Called++;
  }
