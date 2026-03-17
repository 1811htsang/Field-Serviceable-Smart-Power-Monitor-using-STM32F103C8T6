/*
 * lib_systick_hal.h
 *
 *  Created on: Mar 17, 2026
 *      Author: shanghuang
 */

#ifndef LIB_SYSTICK_HAL_H_
  #define LIB_SYSTICK_HAL_H_

  // Khai báo các thư viện sử dụng

    #include <stdint.h>

    #ifndef UNIT_TEST
      #include "generic/lib_keyword_def.h"
      #include "systick/lib_systick_def.h"
    #else
      #include "lib_keyword_def.h"
      #include "lib_systick_def.h"
    #endif

  // Khai báo biến tham chiếu toàn cục

    __vo ui32 ms_ticks = 0; // Biến đếm số lượng tick đã trôi qua kể từ khi khởi động hệ thống, được cập nhật trong hàm xử lý ngắt của SysTick

  // Khai báo các kiểm tra nội bộ

    #define IS_SYSTICK_TICKS(TICKS) (((TICKS) >= SYSTICK_LOAD_MIN_RELOAD_VALUE) && ((TICKS) <= SYSTICK_LOAD_MAX_RELOAD_VALUE))

  // Khai báo các hàm thành phần

    // >> Hàm khởi tạo SysTick với số lượng tick cần thiết để tạo ra khoảng thời gian delay mong muốn
    void SYSTICK_Init(ui32 ticks);

    // >> Hàm giải phóng SysTick, đưa ngoại vi về trạng thái ban đầu
    void SYSTICK_DeInit(void);

    // >> Hàm tạo delay theo đơn vị ms, sử dụng biến ms_ticks để xác định thời gian đã trôi qua
    void SYSTICK_DelayMs(ui32 delay_ms);

    // >> Hàm tăng biến ms_ticks, được gọi trong hàm xử lý ngắt của SysTick để cập nhật thời gian đã trôi qua
    stinl void SYSTICK_IncTick(void) {
      ms_ticks++;
    }

    // >> Hàm xử lý ngắt của SysTick, được gọi khi có ngắt từ SysTick, sẽ gọi SYSTICK_IncTick để cập nhật biến ms_ticks
    void SYSTICK_Handler(void) {
      SYSTICK_IncTick();
    }

    // >> Hàm thu thập tick
    stinl ui32 SYSTICK_GetTick(void) {
      return ms_ticks;
    }



#endif /* LIB_SYSTICK_HAL_H_ */
