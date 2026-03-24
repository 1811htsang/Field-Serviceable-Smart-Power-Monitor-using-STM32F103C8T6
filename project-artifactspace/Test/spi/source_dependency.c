/*
 * source_dependency.c
 *
 *  Created on: Mar 5, 2026
 *      Author: shanghuang
 */

// Khai báo các thư viện cho unit test

  #include "lib_keyword_def.h"
  #include "header_dependency.h"

// Định nghĩa các hàm mock tương ứng

  // >> Từ lib_systick_hal

    stinl ui32 SYSTICK_GetTick(void) {
      return ms_ticks;
    }

    stinl void SYSTICK_IncTick(void) {
      ms_ticks++;
    }

    /**
     * Ghi chú:
     * Bổ sung hàm này để mô phỏng tăng tick trong quá trình chờ đợi cờ trạng thái của SPI,
     * giúp cho các hàm chờ flag trong lib_spi_impl có thể hoạt động đúng trong môi trường unit test 
     * mà không cần phải thực sự có ngắt từ SysTick để cập nhật ms_ticks.
     */

  // >> Từ lib_clock_hal

    RETR_STAT RCC_PCLK_Reset(ul periph) {
      // Hàm mock này sẽ không thực hiện thao tác reset clock thật nào cả, chỉ trả về STAT_OK để giả lập việc reset thành công
      return STAT_OK;
    }
