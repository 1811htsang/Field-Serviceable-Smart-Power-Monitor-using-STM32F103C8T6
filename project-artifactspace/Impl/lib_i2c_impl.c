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
    I2C_Handle_Param *hi2c, ui16 DevAddress, 
    ui16 Size, ui32 Timeout
  );

  // >> Hàm request master read
  sta RETR_STAT I2C_M_ReqRead(
    I2C_Handle_Param *hi2c, ui16 DevAddress, 
    ui16 Size, ui32 Timeout
  );

// Định nghĩa các hàm thành phần

  sta RETR_STAT I2C_FlagTimeout(
    I2C_Handle_Param *hi2c, 
    ui32 flag_mask, 
    ui32 desired_status, 
    ui32 timeout, 
    ui32 tickstart
  ) {

    // Khai báo biến điều chỉnh timeout

      __vo ui32 timeout_adjusted = 0u;
      ui32 elapsed = 0u;

    // Điều chỉnh timeout để bảo vệ khi Systick bị tắt

      timeout_adjusted = timeout * (1000u * 32u);

    // Phân biệt CR2_BSY và CR1_ADDR

      ui32 tmp = 0;

      if (flag_mask == I2C_SR1_ADDR_MASK) {
        tmp = flag_mask;
        tmp += 0xF0u; // +1 số đơn vị để đảm bảo không nhầm lẫn với các cờ khác trên cùng thanh ghi I2C_SR1
      }

    // Phân loại theo thanh ghi để tránh đọc nhầm

      switch (tmp) {
        case I2C_SR1_ADDR_MASK:
        case I2C_SR1_BTF_MASK:
        case I2C_SR1_STOPF_MASK:
        case I2C_SR1_RXNE_MASK:
        case I2C_SR1_TXE_MASK:
          
          // Vòng lặp chờ cờ đạt trạng thái mong muốn hoặc timeout

            while ( // Đọc kiểm tra xem thanh ghi và mask có khớp với mask và trạng thái mong muốn hay không
              ((READ_BIT(hi2c->Instance->I2C_SR1, flag_mask) == flag_mask) ? SET: RESET) != desired_status
            ) {
              if (timeout != SYSTICK_LOAD_MAX_RELOAD_VALUE) {
                elapsed = SYSTICK_GetTick() - tickstart; // Tính thời gian đã trôi qua kể từ khi bắt đầu chờ
                if (
                  (elapsed >= timeout) // Nếu thời gian đã trôi qua lớn hơn hoặc bằng timeout ban đầu thì trả về lỗi timeout
                  ||
                  (timeout == 0u) // Trường hợp timeout ban đầu là 0, tức là không chờ, sẽ trả về timeout ngay lập tức
                  ||
                  (timeout_adjusted == 0u) // Nếu timeout đã được điều chỉnh về 0 do Systick bị tắt thì trả về timeout ngay lập tức
                ) {
                  hi2c->ErrorCode = I2C_ERR_TIMEOUT; // Cập nhật mã lỗi vào handle_param
                  return STAT_TIMEOUT; // Thao tác thất bại do timeout
                }
              }
            }
          break;

        // Đã được điều chỉnh để tránh nhầm lẫn với cờ trên SR1, thực tế vẫn dùng I2C_SR2_BUSY_MASK để đọc cờ BUSY trên thanh ghi SR2
        case I2C_SR2_BUSY_MASK + 0xF0u: 
        case I2C_SR2_MSL_MASK:

          // Vòng lặp chờ cờ đạt trạng thái mong muốn hoặc timeout

            while ( // Đọc kiểm tra xem thanh ghi và mask có khớp với mask và trạng thái mong muốn hay không
              ((READ_BIT(hi2c->Instance->I2C_SR2, flag_mask) == flag_mask) ? SET: RESET) != desired_status
            ) {
              if (timeout != SYSTICK_LOAD_MAX_RELOAD_VALUE) {
                elapsed = SYSTICK_GetTick() - tickstart; // Tính thời gian đã trôi qua kể từ khi bắt đầu chờ
                if (
                  (elapsed >= timeout) // Nếu thời gian đã trôi qua lớn hơn hoặc bằng timeout ban đầu thì trả về lỗi timeout
                  ||
                  (timeout == 0u) // Trường hợp timeout ban đầu là 0, tức là không chờ, sẽ trả về timeout ngay lập tức
                  ||
                  (timeout_adjusted == 0u) // Nếu timeout đã được điều chỉnh về 0 do Systick bị tắt thì trả về timeout ngay lập tức
                ) {
                  hi2c->ErrorCode = I2C_ERR_TIMEOUT; // Cập nhật mã lỗi vào handle_param
                  return STAT_TIMEOUT; // Thao tác thất bại do timeout
                }
              }
            }
          break;
        
        default:
          hi2c->ErrorCode = I2C_ERR_UNKNOWN; // Cập nhật mã lỗi vào handle_param
          return STAT_ERROR; // Thao tác thất bại do cờ không hợp lệ
          break;
      }

    return STAT_OK; // Thao tác thành công, cờ đã đạt trạng thái mong muốn
  }

  sta RETR_STAT I2C_M_ReqWrite(
    I2C_Handle_Param *hi2c, 
    ui16 DevAddress, 
    ui16 Size, 
    ui32 Timeout
  ) {
    // Bật Acknowledge

      I2C_ACK_ENABLE(hi2c);

    // Gửi Start condition

      I2C_GENERATE_START(hi2c);

    // Đợi cờ SB để đảm bảo đã bắt đầu được transaction

      if (
        I2C_FlagTimeout(
          hi2c,I2C_SR1_SB_MASK,SET,
          Timeout,SYSTICK_GetTick()
        ) != STAT_OK
      ) {
        hi2c->ErrorCode = I2C_ERR_START; // Cập nhật mã lỗi vào handle_param
        return STAT_TIMEOUT; // Yêu cầu thất bại do timeout khi chờ cờ SB
      }

    // Gửi địa chỉ thiết bị 

      hi2c->Instance->I2C_DR = (ui8)(DevAddress << 1u) & 0xFEu; // Gửi địa chỉ thiết bị với bit R/W = 0 (viết)

      /**
       * Ghi chú:
       * Đối với chế độ 7-bit thì chỉ cần gửi 7 bit địa chỉ
       */

    // Đợi cờ ADDR và kiểm tra cờ AF song song để đảm bảo đã gửi địa chỉ thành công

      uint32_t tickstart = SYSTICK_GetTick();
      while (!READ_BIT(hi2c->Instance->I2C_SR1, I2C_SR1_ADDR_MASK)) {
        if (READ_BIT(hi2c->Instance->I2C_SR1, I2C_SR1_AF_MASK)) {
          I2C_GENERATE_STOP(hi2c); // Gửi STOP để giải phóng bus
          I2C_CLEAR_AF(hi2c);      // Xóa cờ báo lỗi NACK
          hi2c->ErrorCode = I2C_ERR_ADDR_NACK;
          return STAT_ERROR;
        }
        if ((SYSTICK_GetTick() - tickstart) >= Timeout) {
          return STAT_TIMEOUT;
        }
      }

    // Xóa cờ ADDR (SR1 đã đọc ở trên)

      __vo ui32 tmpreg = hi2c->Instance->I2C_SR2;

    return STAT_OK; // Yêu cầu thành công, đã gửi địa chỉ và sẵn sàng truyền dữ liệu
  }

  sta RETR_STAT I2C_M_ReqRead(
    I2C_Handle_Param *hi2c, 
    ui16 DevAddress, 
    ui16 Size, 
    ui32 Timeout
  ) {
    // Bật Acknowledge

      I2C_ACK_ENABLE(hi2c);

    // Gửi Start condition

      I2C_GENERATE_START(hi2c);

    // Đợi cờ SB để đảm bảo đã bắt đầu được transaction

      if (
        I2C_FlagTimeout(
          hi2c,I2C_SR1_SB_MASK,SET,
          Timeout,SYSTICK_GetTick()
        ) != STAT_OK
      ) {
        hi2c->ErrorCode = I2C_ERR_START; // Cập nhật mã lỗi vào handle_param
        return STAT_TIMEOUT; // Yêu cầu thất bại do timeout khi chờ cờ SB
      }

    // Gửi địa chỉ thiết bị 

      hi2c->Instance->I2C_DR = ((ui8)(DevAddress << 1u) & 0xFEu) | 0x01u; // Gửi địa chỉ thiết bị với bit R/W = 1 (đọc)

      /**
       * Ghi chú:
       * Đối với chế độ 7-bit thì chỉ cần gửi 7 bit địa chỉ
       */

    // Đợi cờ ADDR và kiểm tra cờ AF song song để đảm bảo đã gửi địa chỉ thành công

      uint32_t tickstart = SYSTICK_GetTick();
      while (!READ_BIT(hi2c->Instance->I2C_SR1, I2C_SR1_ADDR_MASK)) {
        if (READ_BIT(hi2c->Instance->I2C_SR1, I2C_SR1_AF_MASK)) {
          I2C_GENERATE_STOP(hi2c); // Gửi STOP để giải phóng bus
          I2C_CLEAR_AF(hi2c);      // Xóa cờ báo lỗi NACK
          hi2c->ErrorCode = I2C_ERR_ADDR_NACK;
          return STAT_ERROR;
        }
        if ((SYSTICK_GetTick() - tickstart) >= Timeout) {
          return STAT_TIMEOUT;
        }
      }

    // Kiểm tra trường hợp đọc 1 byte và đọc N byte để xử lý ACK/NACK phù hợp

      if (Size == 1u) {
        // Tắt ACK

          I2C_ACK_DISABLE(hi2c); 

        // Xóa cờ ADDR (SR1 đã đọc ở trên)

          __vo ui32 tmpreg = hi2c->Instance->I2C_SR2;

        // Gửi STOP sau khi nhận byte cuối cùng để giải phóng bus

          I2C_GENERATE_STOP(hi2c);

      } else {
        // Xóa cờ ADDR (SR1 đã đọc ở trên)

          __vo ui32 tmpreg = hi2c->Instance->I2C_SR2;

        // Vẫn giữ ACK = 1 để nhận tiếp các byte sau
      }

    return STAT_OK; // Yêu cầu thành công, đã gửi địa chỉ và sẵn sàng nhận dữ liệu
  }