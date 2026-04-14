/*
 * lib_i2c_impl.c
 *
 *  Created on: Apr 14, 2026
 *      Author: shanghuang
 */

// Khai báo các thư viện sử dụng chung

  #ifdef UNIT_TEST
    #include "lib_keyword_def.h"
    #include "lib_condition_def.h"
    #include "lib_i2c_def.h"
    #include "lib_i2c_hal.h"
    #include "header_dependency.h"
  #endif

  #include <stdint.h>
  #include <stdio.h>
  #include <string.h>

  #ifndef UNIT_TEST
    #include "generic/lib_keyword_def.h"
    #include "generic/lib_condition_def.h"
    #include "systick/lib_systick_def.h"
    #include "systick/lib_systick_hal.h"
    #include "i2c/lib_i2c_def.h"
    #include "i2c/lib_i2c_hal.h"
    #include "clock/lib_clock_def.h"
    #include "clock/lib_clock_hal.h"
  #endif

// Khai báo biến quản lý thời gian

  __vo ui32 ms_ticks = 0;

// Khai báo các hàm nội bộ

  // >> Hàm vô hiệu hóa với polling
  sta void I2C_DisablePolling(
    I2C_Handle_Param *hi2c,
    I2C_DIRECTION_Enum direction
  );
  
  // >> Hàm chờ cờ với timeout, trả về STAT_OK nếu cờ đạt trạng thái mong muốn, trả về STAT_TIMEOUT nếu timeout xảy ra
  sta RETR_STAT I2C_FlagTimeout(
    I2C_Handle_Param *hi2c, 
    ui32 flag_mask,      // Mặt nạ bit của cờ cần chờ
    ui32 desired_status, // Trạng thái mong muốn của cờ sau khi chờ (0 hoặc != 0)
    ui32 timeout,        // Thời gian chờ tối đa tính bằng ms
    ui32 tickstart       // Thời điểm bắt đầu chờ tính bằng tick của SysTick
  );

  // >> Hàm request master write
  sta RETR_STAT I2C_M_ReqWrite(
    I2C_Handle_Param *hi2c, 
    ui16 DevAddress, 
    ui16 Size, 
    ui32 Timeout
  );

  // >> Hàm request master read
  sta RETR_STAT I2C_M_ReqRead(
    I2C_Handle_Param *hi2c, 
    ui16 DevAddress, 
    ui16 Size, 
    ui32 Timeout
  );

// Định nghĩa các hàm thành phần

  sta void I2C_DisablePolling(
    I2C_Handle_Param *hi2c, 
    I2C_DIRECTION_Enum direction
  ) {
    
    ui32 mode = hi2c->Init.Mode;

    switch (mode) {
      case I2C_MODE_MASTER:
        if (direction == I2C_DIRECTION_TX) {
          
          // Đợi cờ TXE = 1 và BTF = 1
        
            while (!READ_BIT(hi2c->Instance->I2C_SR1, I2C_SR1_TXE_MASK));
            while (!READ_BIT(hi2c->Instance->I2C_SR1, I2C_SR1_BTF_MASK));

          // Đợi cờ TRA

            while (!READ_BIT(hi2c->Instance->I2C_SR2, I2C_SR2_TRA_MASK));

          // Thực hiện STOP để kết thúc giao tiếp

            SET_BIT(hi2c->Instance->I2C_CR1, I2C_CR1_STOP_MASK);

          // Đợi cờ STOPF = 1 để đảm bảo giao tiếp đã kết thúc hoàn toàn trước khi tắt

            while (!READ_BIT(hi2c->Instance->I2C_SR1, I2C_SR1_STOPF_MASK));

          // Đợi cờ BSY = 0 để đảm bảo giao tiếp đã kết thúc hoàn toàn trước khi tắt

            while (READ_BIT(hi2c->Instance->I2C_SR2, I2C_SR2_BUSY_MASK)); 

          // Tắt I2C

            CLEAR_BIT(hi2c->Instance->I2C_CR1, I2C_CR1_PE_MASK);

          /**
           * Ghi chú:
           * Cờ STOPF tự động clear khi PE = 0 
           * nên không cần phải clear thủ công trước khi tắt I2C.
           */

        } else {
          // Đợi cờ RXNE = 1 (Receive Buffer Not Empty) để đảm bảo byte cuối cùng đã được nhận đầy đủ trước khi tắt
          while (!READ_BIT(hi2c->Instance->I2C_SR1, I2C_SR1_RXNE_MASK));
        }

        break;

      case I2C_MODE_SLAVE:
        /* code */
        break;
      
      default:
        return; // Trường hợp này sẽ không xảy ra vì đã được kiểm tra ở Dispatcher, nhưng vẫn cần có để đảm bảo an toàn
        break;
    }
  }

  sta RETR_STAT I2C_FlagTimeout(
    I2C_Handle_Param *hi2c, 
    ui32 flag_mask, 
    ui32 desired_status, 
    ui32 timeout, 
    ui32 tickstart
  ) {

  }