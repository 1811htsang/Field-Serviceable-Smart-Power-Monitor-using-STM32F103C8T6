/*
 * source_depenency.c
 *
 *  Created on: Jan 21, 2026
 *      Author: shanghuang
 */

/**
 * Ghi chú:
 * - File này dùng để include các file header định nghĩa thư viện ngoại vi 
 * cần thiết cho việc test đơn vị (unit test).
 * - Mỗi file header định nghĩa thư viện ngoại vi sẽ được include trong file này.
 * - Khi thêm thư viện ngoại vi mới, chỉ cần thêm dòng include tương ứng vào file
 */

// Khai báo các thư viện cho unit test

  #include "lib_keyword_def.h"
  #include "iwdg/lib_iwdg_def.h"
  #include "iwdg/lib_iwdg_hal.h"
  #include "Test/iwdg/header_dependency.h"

// Khai báo các dependency của các hàm IWDG_Init

  RETR_STAT MOCK_RCC_IsLSIReady = STAT_RDY;
  RETR_STAT MOCK_RCC_CLK_Init = STAT_OK;
  RETR_STAT MOCK_IWDG_IsPrescalerUpdated = STAT_RDY;
  RETR_STAT MOCK_IWDG_IsReloadValueUpdated = STAT_RDY;

// Định nghĩa các hàm mock tương ứng

  RETR_STAT RCC_IsLSIReady(void) {
    return MOCK_RCC_IsLSIReady;
  }

  RETR_STAT RCC_CLK_Init(RCC_CLK_Init_Param *init_param) {
    return MOCK_RCC_CLK_Init;
  }

  RETR_STAT IWDG_IsPrescalerUpdated(void) {
    return MOCK_IWDG_IsPrescalerUpdated;
  }

  RETR_STAT IWDG_IsReloadValueUpdated(void) {
    return MOCK_IWDG_IsReloadValueUpdated;
  }
