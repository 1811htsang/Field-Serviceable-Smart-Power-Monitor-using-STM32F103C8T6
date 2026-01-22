/*
 * lib_iwdg_impl.c
 *
 *  Created on: Jan 14, 2026
 *      Author: shanghuang
 */

// Khai báo các thư viện sử dụng chung

  #include <stdint.h>
  #include <stdio.h>
  #include "lib_keyword_def.h"
  #include "lib_iwdg_def.h"
  #include "lib_iwdg_hal.h"

  #ifndef UNIT_TEST
    #include "lib_clock_hal.h"
  #else
    #include "header_dependency.h"
  #endif
  

// Định nghĩa các hàm thành phần

  RETR_STAT IWDG_Init(IWDG_Init_Param *init_param) {

    /**
     * Dependency của hàm:
     * - RETR của RCC_IsLSIReady
     * - RETR của RCC_CLK_Init
     * - RETR của IWDG_IsPrescalerUpdated
     * - RETR của IWDG_IsReloadValueUpdated
     */

    if (DEBUG_MODE == ENABLE) {
      printf("IWDG_Init, DBG1: Check Null pointer.\n");
    }

    if (init_param == NULL) {
      return STAT_ERROR;
    }

    if (DEBUG_MODE == ENABLE) {
      printf("IWDG_Init, DBG2: Assert parameter.\n");
    }

    assert_param(IS_IWDG_PRESCALER(init_param->Prescaler));
    assert_param(IS_IWDG_RELOAD(init_param->Reload));

    if (DEBUG_MODE == ENABLE) {
      printf("IWDG_Init, DBG3: Setup LSI clock.\n");
    }

    // Bật LSI nếu chưa được bật

    if (RCC_IsLSIReady() != STAT_RDY) {
      RCC_CLK_Init_Param rcc_lsi_init;
      rcc_lsi_init.CLK_Source = RCC_IWDG_SOURCE_LSI;
      RCC_RDYFLG_Typdef rdy_flg;
      if (RCC_CLK_Init(&rcc_lsi_init, &rdy_flg) != STAT_OK) {
        return STAT_ERROR;
      }
    }

    if (DEBUG_MODE == ENABLE) {
      printf("IWDG_Init, DBG4: Enable write access.\n");
    }

    // Kích hoạt quyền ghi vào các thanh ghi cấu hình IWDG
    IWDG_EnableWriteAccess();

    if (DEBUG_MODE == ENABLE) {
      printf("IWDG_Init, DBG5: Configure prescaler and reload value.\n");
    }

    // Cấu hình bộ chia tần số
    if (IWDG_IsPrescalerUpdated() == STAT_RDY) {
      IWDG_REGS_PTR->PR.PR = init_param->Prescaler;
    } else {
      return STAT_NRDY;
    }

    // Cấu hình giá trị nạp lại
    if (IWDG_IsReloadValueUpdated() == STAT_RDY) {
      IWDG_REGS_PTR->RLR.RL = init_param->Reload;
    } else {
      return STAT_NRDY;
    }

    if (DEBUG_MODE == ENABLE) {
      printf("IWDG_Init, DBG6: Finish initialization.\n");
    }

    /**
     * Ghi chú:
     * Ở công đoạn này, 
     * chúng ta chưa khởi động IWDG ngay lập tức,
     * để cho phép người dùng có thể thực hiện các cấu hình bổ sung khác
     * trước khi IWDG bắt đầu hoạt động.
     * Tại line này, hàm khởi tạo IWDG được xem là hoàn tất thành công.
     * Thực hiện tắt quyền ghi.
     */

    // Tắt quyền ghi vào các thanh ghi cấu hình IWDG
    IWDG_DisableWriteAccess();

    return STAT_DONE;
  }

  RETR_STAT IWDG_DeInit(IWDG_Init_Param *init_param) {

    /**
     * Ghi chú:
     * IWDG là một ngoại vi không thể tắt sau khi được khởi động.
     * Do đó, hàm DeInit này chỉ thực hiện việc
     * - Reload bộ đếm IWDG về giá trị mặc định
     * - Khóa quyền ghi vào các thanh ghi cấu hình IWDG
     */
    
    // Reload bộ đếm IWDG về giá trị mặc định
    IWDG_Reload();

    // Khóa quyền ghi vào các thanh ghi cấu hình IWDG
    IWDG_DisableWriteAccess();

    return STAT_DONE;
  }

  void IWDG_Start(void) {
    IWDG_REGS_PTR->KR.KEY = IWDG_KR_REG_KEY_START;
  }

  void IWDG_Reload(void) {
    IWDG_REGS_PTR->KR.KEY = IWDG_KR_REG_KEY_RELOAD_COUNTER;
  }

  RETR_STAT IWDG_IsReloadValueUpdated(void) {
    if (IWDG_REGS_PTR->SR.RVU == SET) {
      return STAT_NRDY;
    }
    return STAT_RDY;
  }

  RETR_STAT IWDG_IsPrescalerUpdated(void) {
    if (IWDG_REGS_PTR->SR.PVU == SET) {
      return STAT_NRDY;
    }
    return STAT_RDY;
  }

  void IWDG_EnableWriteAccess(void) {
    IWDG_REGS_PTR->KR.KEY = IWDG_KR_REG_KEY_ENABLE_ACCESS;
  }

  void IWDG_DisableWriteAccess(void) {
    IWDG_REGS_PTR->KR.KEY = IWDG_KR_REG_KEY_DISABLE_ACCESS;
  }

  void IWDG_ResetEvent_Catch(void) {
    /**
     * Ghi chú: 
     * Chỗ này sẽ có bổ sung hoạt động xử lý sự kiện reset do IWDG
     * thông qua bổ sung driver cho reset 
     */
  }
