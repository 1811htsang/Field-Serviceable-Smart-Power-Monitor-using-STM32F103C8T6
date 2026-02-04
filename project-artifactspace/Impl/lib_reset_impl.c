/*
 * lib_reset_impl.c
 *
 *  Created on: Jan 29, 2026
 *      Author: shanghuang
 */

// Khai báo các thư viện sử dụng chung

  #ifdef UNIT_TEST
  
  #endif

  #include <stdint.h>
  #include <stdio.h>
  #include "generic/lib_keyword_def.h"
  #include "generic/lib_condition_def.h"
  #include "reset/lib_reset_def.h"
  #include "reset/lib_reset_hal.h"

  #ifndef UNIT_TEST
    #include "clock/lib_clock_def.h"
    #include "clock/lib_clock_hal.h"
  #endif

// Định nghĩa các hàm thành phần

  void RST_SYS_SW_Reset(void) {
    // Ghi giá trị VECTKEY và SYSRESETREQ vào thanh ghi AIRCR để thực hiện reset phần mềm
    SCB_AIRCR_REG_PTR->VECTKEY = 0x5FA; // Giá trị khóa
    SCB_AIRCR_REG_PTR->SYSRESETREQ = SET; // Yêu cầu reset hệ thống

    // Chờ đợi cho đến khi hệ thống được reset
    while (1) {
      // Vòng lặp vô hạn chờ reset xảy ra
    }
  }

  // >> Hàm catch reset event sau khi khởi động lại hệ thống
  void RST_SRC_Capture(RST_FLG_Typdef *reset_source) {
    // Kiểm tra con trỏ đầu vào
    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("RESET_CaptureResetSource, DBG1: Check Null pointer.\n");
    }
    if (__NULL_PTR_CHECK(reset_source)) {
      return;
    }

    // Đọc trạng thái các cờ reset từ RCC_CSR_REG và gán vào cấu trúc reset_source
    reset_source->IsPinReset = (RCC_REGS_PTR->CSR.PINRSTF == RCC_CSR_REG_PINRSTF_RESET_OCCURRED) ? SET : RESET;
    reset_source->IsPorReset = (RCC_REGS_PTR->CSR.PORRSTF == RCC_CSR_REG_PORRSTF_RESET_OCCURRED) ? SET : RESET;
    reset_source->IsSftReset = (RCC_REGS_PTR->CSR.SFTRSTF == RCC_CSR_REG_SFTRSTF_RESET_OCCURRED) ? SET : RESET;
    reset_source->IsIwdgReset = (RCC_REGS_PTR->CSR.IWDGRSTF == RCC_CSR_REG_IWDGRSTF_RESET_OCCURRED) ? SET : RESET;
    reset_source->IsWwdgReset = (RCC_REGS_PTR->CSR.WWDGRSTF == RCC_CSR_REG_WWDGRSTF_RESET_OCCURRED) ? SET : RESET;
    reset_source->IsLowPwrReset = (RCC_REGS_PTR->CSR.LPWRRSTF == RCC_CSR_REG_LPWRRSTF_RESET_OCCURRED) ? SET : RESET;

    // Xóa các cờ reset đã đọc bằng cách ghi RMVF
    RCC_REGS_PTR->CSR.RMVF = RCC_CSR_REG_RMVF_CLEAR;
  }