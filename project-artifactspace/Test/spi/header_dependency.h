/*
 * header_dependency.h
 *
 *  Created on: Mar 24, 2026
 *      Author: shanghuang
 */

#ifndef SPI_HEADER_DEPENDENCY_H_
  #define SPI_HEADER_DEPENDENCY_H_

  // Khai báo các thư viện sử dụng chung cho unit test

    #include "lib_keyword_def.h"
    #include "lib_condition_def.h"

  // Khai báo các dependency definition từ các file header cần thiết

    /**
     * Ghi chú:
     * Unit test lần này thử nghiệm việc
     * không bổ sung khai báo phụ thuộc nào của lib_spi_hal
     * các file header của module SPI đã được thiết kế tốt về mặt độc lập 
     * và không có phụ thuộc ngầm nào vào các module khác hay chưa.
     */

    // >> Từ lib_systick_def

      #define SYSTICK_LOAD_MAX_RELOAD_VALUE 0x00FFFFFFu
      #define SYSTICK_LOAD_MIN_RELOAD_VALUE 0x00000001u

    // >> Từ lib_systick_hal

      extern __vo ui32 ms_ticks; // Biến đếm số lượng tick đã trôi qua kể từ khi khởi động hệ thống, được cập nhật trong hàm xử lý ngắt của SysTick
      ui32 SYSTICK_GetTick(void);
      void SYSTICK_IncTick(void);

    // >> Từ lib_clock_def

      #define SPI (0x01ul << 12) // Bit 12 trong RCC_APB2ENR để bật clock cho SPI1
      #define SPI1 SPI

    // >> Từ lib_clock_hal

      RETR_STAT RCC_PCLK_Reset(ul periph);

#endif /* SPI_HEADER_DEPENDENCY_H_ */
