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
  #include "header_dependency.h"

// Khai báo các dependency của các hàm IWDG_Init

  RETR_STAT RCC_IsLSIReady_Expect = STAT_RDY;
  RETR_STAT RCC_CLK_Init_Expect = STAT_OK;
  RETR_STAT IWDG_IsPrescalerUpdated_Expect = STAT_RDY;
  RETR_STAT IWDG_IsReloadValueUpdated_Expect = STAT_RDY;

// Định nghĩa các hàm mock tương ứng

  RETR_STAT RCC_IsLSIReady(void) {
    return RCC_IsLSIReady_Expect;
  }

  RETR_STAT RCC_CLK_Init(RCC_CLK_Init_Param *init_param, RCC_RDYFLG_Typdef *rdy_flg) {
    if (init_param->CLK_Source != RCC_IWDG_SOURCE_LSI) {
      return STAT_ERROR;
    }

    if (rdy_flg->LSI_RDY_FLG != SET) {
      return STAT_ERROR;
    } else {
      return RCC_CLK_Init_Expect;
    }
  }
