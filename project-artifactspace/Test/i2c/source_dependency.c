/*
 * source_dependency.c
 *
 *  Created on: Apr 28, 2026
 *      Author: shanghuang
 */

// Khai báo các thư viện cho unit test

  #include "lib_keyword_def.h"
  #include "header_dependency.h"

// Khai báo các dependency variables của các hàm cần định nghĩa

  ui32 mock_pclk1_freq_hz = I2C_TEST_PCLK1_FREQ_HZ;

// Định nghĩa các hàm mock tương ứng

  ui32 SYSTICK_GetTick(void) {
    return ms_ticks++;
  }

  void SYSTICK_IncTick(void) {
    ms_ticks++;
  }

  ui32 RCC_Get_PCLK1_Freq(void) {
    return mock_pclk1_freq_hz;
  }

  /**
   * Ghi chú:
   * SysTick được mô phỏng bằng bộ đếm tăng tự động để các nhánh timeout trong I2C
   * có thể thoát ra trong môi trường unit test mà không cần ngắt phần cứng thật.
   */
