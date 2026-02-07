/*
 * lib_iwdg_impl.c
 *
 *  Created on: Jan 14, 2026
 *      Author: shanghuang
 */

// Khai báo các thư viện sử dụng chung

  #ifdef UNIT_TEST
		#include "lib_keyword_def.h"
		#include "lib_condition_def.h"
		#include "lib_clock_def.h"
		#include "lib_clock_hal.h"
    #include "header_dependency.h"
  #endif

  #include <stdint.h>
  #include <stdio.h>
 	#include <string.h>

  #ifndef UNIT_TEST
		#include "generic/lib_keyword_def.h"
  	#include "generic/lib_condition_def.h"
  	#include "clock/lib_clock_def.h"
  	#include "clock/lib_clock_hal.h"
		#include "iwdg/lib_iwdg_def.h"
		#include "iwdg/lib_iwdg_hal.h"
  #endif

// Định nghĩa các hàm thành phần

/*
 * Hàm khởi tạo ngoại vi IWDG.
 *
 * Tham số:
 *   init_param - Con trỏ tới cấu trúc tham số khởi tạo IWDG.
 *
 * Trả về:
 *   RETR_STAT - Trạng thái thực thi (STAT_DONE, STAT_ERROR, STAT_NRDY).
 */
RETR_STAT IWDG_Init(IWDG_Init_Param *init_param) {
  // Kiểm tra con trỏ đầu vào
  if (init_param == NULL) {
    return STAT_ERROR;
  }

  // Kiểm tra giá trị tham số đầu vào
  assert_param(IS_IWDG_PRESCALER(init_param->Prescaler));
  assert_param(IS_IWDG_RELOAD(init_param->Reload));

  // Bật LSI nếu chưa được bật
  if (RCC_IsLSIReady() != STAT_RDY) {
    RCC_CLK_Init_Param rcc_lsi_init;
    rcc_lsi_init.CLK_Source = RCC_IWDG_SOURCE_LSI;
    RCC_RDYFLG_Typdef rdy_flg;
    if (RCC_CLK_Init(&rcc_lsi_init, &rdy_flg) != STAT_OK) {
      return STAT_ERROR;
    }
  }

  // Kích hoạt quyền ghi vào các thanh ghi cấu hình IWDG
  IWDG_EnableWriteAccess();

  // Cấu hình bộ chia tần số
  if (IWDG_IsPrescalerUpdated() == STAT_RDY) {
    IWDG_REGS_PTR->PR.PR = init_param->Prescaler;
  } else {
    IWDG_DisableWriteAccess();
    return STAT_NRDY;
  }

  // Cấu hình giá trị nạp lại
  if (IWDG_IsReloadValueUpdated() == STAT_RDY) {
    IWDG_REGS_PTR->RLR.RL = init_param->Reload;
  } else {
    IWDG_DisableWriteAccess();
    return STAT_NRDY;
  }

  // Tắt quyền ghi vào các thanh ghi cấu hình IWDG
  IWDG_DisableWriteAccess();

  return STAT_DONE;
}

/*
 * Hàm de-initialize ngoại vi IWDG.
 *
 * Tham số:
 *   init_param - Không sử dụng, chỉ để tương thích giao diện.
 *
 * Trả về:
 *   RETR_STAT - Luôn trả về STAT_DONE.
 */
RETR_STAT IWDG_DeInit(IWDG_Init_Param *init_param) {
  // Reload bộ đếm IWDG về giá trị mặc định
  IWDG_Reload();

  // Khóa quyền ghi vào các thanh ghi cấu hình IWDG
  IWDG_DisableWriteAccess();

  return STAT_DONE;
}

/*
 * Hàm khởi động IWDG.
 * Không trả về giá trị.
 */
void IWDG_Start(void) {
  IWDG_REGS_PTR->KR.KEY = IWDG_KR_REG_KEY_START;
}

/*
 * Hàm tải lại bộ đếm IWDG.
 * Không trả về giá trị.
 */
void IWDG_Reload(void) {
  IWDG_REGS_PTR->KR.KEY = IWDG_KR_REG_KEY_RELOAD_COUNTER;
}

/*
 * Hàm kiểm tra trạng thái bộ nạp lại giá trị của IWDG.
 *
 * Trả về:
 *   RETR_STAT - STAT_RDY nếu đã cập nhật xong, STAT_NRDY nếu đang cập nhật.
 */
RETR_STAT IWDG_IsReloadValueUpdated(void) {
  if (IWDG_REGS_PTR->SR.RVU == SET) {
    return STAT_NRDY;
  }
  return STAT_RDY;
}

/*
 * Hàm kiểm tra trạng thái bộ chia tần số của IWDG.
 *
 * Trả về:
 *   RETR_STAT - STAT_RDY nếu đã cập nhật xong, STAT_NRDY nếu đang cập nhật.
 */
RETR_STAT IWDG_IsPrescalerUpdated(void) {
  if (IWDG_REGS_PTR->SR.PVU == SET) {
    return STAT_NRDY;
  }
  return STAT_RDY;
}

/*
 * Hàm bật quyền ghi vào thanh ghi cấu hình IWDG.
 * Không trả về giá trị.
 */
void IWDG_EnableWriteAccess(void) {
  IWDG_REGS_PTR->KR.KEY = IWDG_KR_REG_KEY_ENABLE_ACCESS;
}

/*
 * Hàm tắt quyền ghi vào thanh ghi cấu hình IWDG.
 * Không trả về giá trị.
 */
void IWDG_DisableWriteAccess(void) {
  IWDG_REGS_PTR->KR.KEY = IWDG_KR_REG_KEY_DISABLE_ACCESS;
}

/*
 * Hàm xử lý sự kiện reset do IWDG.
 * (Chưa triển khai, placeholder cho mở rộng driver reset)
 */
void IWDG_ResetEvent_Catch(void) {
  // TODO: Bổ sung xử lý sự kiện reset do IWDG nếu cần
}
