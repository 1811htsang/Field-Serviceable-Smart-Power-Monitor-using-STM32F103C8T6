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

// Định nghĩa các hàm thành phần

  /*
   * Hàm thực hiện reset hệ thống bằng phần mềm (Software System Reset).
   *
   * Tham số: Không có.
   *
   * Logic:
   *   - Ghi giá trị VECTKEY vào thanh ghi SCB_AIRCR để mở khóa ghi.
   *   - Set bit SYSRESETREQ trong SCB_AIRCR để yêu cầu reset hệ thống.
   *   - Vào vòng lặp vô hạn chờ hệ thống reset.
   *   - Sau khi reset, hệ thống sẽ khởi động lại từ đầu.
   *
   * Trả về:
   *   Không có (void) - Hàm không bao giờ trả về vì hệ thống sẽ reset.
   *
   * Phụ thuộc ngoài module Reset:
   *   - SET_BIT() - Macro set bit thanh ghi
   *   - SCB_AIRCR_REG_PTR - Con trỏ tới thanh ghi AIRCR của SCB
   *   - SCB_AIRCR_REG_VECTKEY_SET - Giá trị key để mở khóa ghi
   *   - SCB_AIRCR_REG_SYSRESETREQ_SET - Bit yêu cầu reset hệ thống
   */
  void RST_SYS_SW_Reset(void) {
    // Ghi giá trị VECTKEY và SYSRESETREQ vào thanh ghi AIRCR để thực hiện reset phần mềm
    SET_BIT(*SCB_AIRCR_REG_PTR, SCB_AIRCR_REG_VECTKEY_SET);
    SET_BIT(*SCB_AIRCR_REG_PTR, SCB_AIRCR_REG_SYSRESETREQ_SET); // Yêu cầu reset hệ thống

    // Chờ đợi cho đến khi hệ thống được reset
    while (1) {
      // Vòng lặp vô hạn chờ reset xảy ra
    }
  }

  /*
   * Hàm capture (ghi lại) nguồn gây ra reset hệ thống sau khi khởi động lại.
   *
   * Tham số:
   *   reset_source - Con trỏ tới cấu trúc lưu trữ các cờ reset.
   *
   * Logic:
   *   - Kiểm tra con trỏ reset_source hợp lệ.
   *   - Đọc các bit cờ reset từ thanh ghi RCC_CSR:
   *       + PINRSTF: Reset từ chân NRST
   *       + PORRSTF: Power-On Reset
   *       + SFTRSTF: Software Reset
   *       + IWDGRSTF: Independent Watchdog Reset
   *       + WWDGRSTF: Window Watchdog Reset
   *       + LPWRRSTF: Low-Power Reset
   *   - Lưu trạng thái các cờ vào cấu trúc reset_source.
   *   - Ghi bit RMVF để clear (xóa) tất cả các cờ reset.
   *
   * Trả về:
   *   Không có (void).
   *
   * Phụ thuộc ngoài module Reset:
   *   - __NULL_PTR_CHECK() - Macro kiểm tra con trỏ NULL
   *   - READ_BIT() - Macro đọc bit thanh ghi
   *   - SET_BIT() - Macro set bit thanh ghi
   *   - RCC_REGS_PTR - Con trỏ tới cấu trúc thanh ghi RCC
   *   - __DEBUG_GET_MODE() - Macro kiểm tra chế độ debug
   */
  void RST_SRC_Capture(RCC_RSTFLG_Typedef *reset_source) {
    // Kiểm tra con trỏ đầu vào
    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("RESET_CaptureResetSource, DBG1: Check Null pointer.\n");
    }
      if (__NULL_PTR_CHECK(reset_source)) {
        return;
      }

    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("RESET_CaptureResetSource, DBG2: Capture reset source flags.\n");
    }

      // Đọc trạng thái các cờ reset từ RCC_CSR_REG và gán vào cấu trúc reset_source
      reset_source->IsPinReset = (READ_BIT(RCC_REGS_PTR->CSR, RCC_CSR_REG_PINRSTF_OCCURRED) == RCC_CSR_REG_PINRSTF_OCCURRED) ? SET : RESET;
      reset_source->IsPorReset = (READ_BIT(RCC_REGS_PTR->CSR, RCC_CSR_REG_PORRSTF_OCCURRED) == RCC_CSR_REG_PORRSTF_OCCURRED) ? SET : RESET;
      reset_source->IsSftReset = (READ_BIT(RCC_REGS_PTR->CSR, RCC_CSR_REG_SFTRSTF_OCCURRED) == RCC_CSR_REG_SFTRSTF_OCCURRED) ? SET : RESET;
      reset_source->IsIwdgReset = (READ_BIT(RCC_REGS_PTR->CSR, RCC_CSR_REG_IWDGRSTF_OCCURRED) == RCC_CSR_REG_IWDGRSTF_OCCURRED) ? SET : RESET;
      reset_source->IsWwdgReset = (READ_BIT(RCC_REGS_PTR->CSR, RCC_CSR_REG_WWDGRSTF_OCCURRED) == RCC_CSR_REG_WWDGRSTF_OCCURRED) ? SET : RESET;
      reset_source->IsLowPwrReset = (READ_BIT(RCC_REGS_PTR->CSR, RCC_CSR_REG_LPWRRSTF_OCCURRED) == RCC_CSR_REG_LPWRRSTF_OCCURRED) ? SET : RESET;

    if (__DEBUG_GET_MODE(ENABLE)) {
      printf("RESET_CaptureResetSource, DBG3: Clear reset source flags.\n");
    }

      // Xóa các cờ reset đã đọc bằng cách ghi RMVF
      SET_BIT(RCC_REGS_PTR->CSR, RCC_CSR_REG_RMVF_SET);
  }
