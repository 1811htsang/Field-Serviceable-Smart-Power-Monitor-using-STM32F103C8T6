/*
 * header_dependency.h
 *
 *  Created on: Apr 28, 2026
 *      Author: shanghuang
 */

#ifndef I2C_HEADER_DEPENDENCY_H_
  #define I2C_HEADER_DEPENDENCY_H_

  // Khai báo các thư viện chung cho unit test

    #include "lib_keyword_def.h"
    #include "lib_condition_def.h"

  // Khai báo các hằng số dùng riêng cho bộ testcase I2C

    #define I2C_TEST_PCLK1_FREQ_HZ 36000000u
    #define I2C_TEST_STANDARD_CLOCK_SPEED_HZ 100000u
    #define I2C_TEST_FAST_CLOCK_SPEED_HZ 400000u
    #define I2C_TEST_OWN_ADDRESS 0x52u
    #define I2C_TEST_TARGET_ADDRESS 0x27u
    #define I2C_TEST_TIMEOUT_MS 5u

  // Khai báo các dependency definition từ các file header cần thiết

    // >> Từ lib_systick_hal

      extern __vo ui32 ms_ticks;
      ui32 SYSTICK_GetTick(void);
      void SYSTICK_IncTick(void);

    // >> Từ lib_clock_hal

      extern ui32 mock_pclk1_freq_hz;
      ui32 RCC_Get_PCLK1_Freq(void);

  /**
   * Ghi chú:
   * I2C dùng SysTick để tính timeout và dùng clock APB1 để cấu hình CR2, TRISE, CCR.
   * Vì vậy testcase cần khai báo rõ các dependency trên để mô phỏng thời gian và tần số clock.
   */

#endif /* I2C_HEADER_DEPENDENCY_H_ */
