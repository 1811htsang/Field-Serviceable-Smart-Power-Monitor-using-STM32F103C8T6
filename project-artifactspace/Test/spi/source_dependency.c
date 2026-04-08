/*
 * source_dependency.c
 *
 *  Created on: Mar 5, 2026
 *      Author: shanghuang
 */

// Khai báo các thư viện cho unit test

  #include <string.h>
  #include "lib_keyword_def.h"
  #include "lib_spi_def.h"
  #include "header_dependency.h"

// Định nghĩa các hàm mock tương ứng

  // >> Từ lib_systick_hal

    ui32 SYSTICK_GetTick(void) {
      return ms_ticks;
    }

    void SYSTICK_IncTick(void) {
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
      if (periph == SPI1) {
        memset(&MOCK_SPI_REGS, 0, sizeof(MOCK_SPI_REGS));
      }

      // Hàm mock này giả lập việc reset thành công và đưa thanh ghi SPI1 về trạng thái reset
      return STAT_OK;
    }
