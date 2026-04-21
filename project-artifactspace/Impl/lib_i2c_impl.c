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
    ui16 Timeout, 
    ui32 Tickstart
  ) {
    // Bật Acknowledge

      I2C_ACK_ENABLE(hi2c);

    // Gửi Start condition

      I2C_GENERATE_START(hi2c);

    // Đợi cờ SB để đảm bảo đã bắt đầu được transaction

      if (
        I2C_FlagTimeout(
          hi2c,I2C_SR1_SB_MASK,SET,
          Timeout, Tickstart
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

      while (!READ_BIT(hi2c->Instance->I2C_SR1, I2C_SR1_ADDR_MASK)) {
        if (READ_BIT(hi2c->Instance->I2C_SR1, I2C_SR1_AF_MASK)) {
          I2C_GENERATE_STOP(hi2c); // Gửi STOP để giải phóng bus
          I2C_CLEAR_AF(hi2c);      // Xóa cờ báo lỗi NACK
          hi2c->ErrorCode = I2C_ERR_ADDR_NACK;
          return STAT_ERROR;
        }
        if ((SYSTICK_GetTick() - Tickstart) >= Timeout) {
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
    ui32 Timeout,
    ui32 Tickstart
  ) {
    // Bật Acknowledge

      I2C_ACK_ENABLE(hi2c);

    // Gửi Start condition

      I2C_GENERATE_START(hi2c);

    // Đợi cờ SB để đảm bảo đã bắt đầu được transaction

      if (
        I2C_FlagTimeout(
          hi2c,I2C_SR1_SB_MASK,SET,
          Timeout, Tickstart
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
        if ((SYSTICK_GetTick() - Tickstart) >= Timeout) {
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

  RETR_STAT I2C_Init(I2C_Handle_Param *hi2c) {
    // Assert tham số đầu vào

      if (hi2c == NULL) {
        return STAT_ERROR;
      }

    // Assert param của handler

      assert_param(hi2c->Instance);
      assert_param(IS_I2C_MODE(hi2c->Init.Mode));
      assert_param(IS_I2C_CLOCKSPEED(hi2c->Init.ClockSpeed));
      assert_param(IS_I2C_DUTYCYCLE(hi2c->Init.DutyCycle));
      assert_param(IS_I2C_OWNADDRESS(hi2c->Init.OwnAddress));
      assert_param(IS_I2C_ADDRESSINGMODE(hi2c->Init.AddressingMode));
      assert_param(IS_I2C_NOSTRETCH(hi2c->Init.NoStretchMode));

    // Báo bận

      hi2c->State = I2C_BUSY;

    // Tắt I2C trước
      
      I2C_Disable(hi2c);

    // Reset I2C

      I2C_SWRST_RESET(hi2c);

    // Kiểm tra PCLK1

      assert_param(IS_I2C_FREQUENCY(
        hi2c->Init.ClockSpeed, RCC_Get_PCLK1_Freq()
      ));

    // Nạp giá trị vào CR2

      ui32 freq = RCC_Get_PCLK1_Freq() / 1000000u;
      MODIFY_REG(hi2c->Instance->I2C_CR2, I2C_CR2_FREQ_MASK, freq);
      
    // Cấu hình rise time cho TRISE

      if (hi2c->Init.ClockSpeed <= 100000u) {
        // Standard mode

          MODIFY_REG(hi2c->Instance->I2C_TRISE, I2C_TRISE_MASK, freq + 1u);
      } else {
        // Fast mode

          MODIFY_REG(hi2c->Instance->I2C_TRISE, I2C_TRISE_MASK, (freq * 300u) / 1000u + 1u);
      }

    // Cấu hình CCR

      MODIFY_REG(
        hi2c->Instance->I2C_CCR, 
        (I2C_CCR_F_S_MASK | I2C_CCR_DUTY_MASK | I2C_CCR_CCR_MASK), 
        I2C_CCR_AUTO(freq, hi2c->Init.ClockSpeed, hi2c->Init.DutyCycle)
      );

    // Cấu hình No Stretch

      MODIFY_REG(
        hi2c->Instance->I2C_CR1, 
        (I2C_CR1_NOSTRETCH_MASK), 
        hi2c->Init.NoStretchMode
      );

    // Cấu hình OAR1

      MODIFY_REG(
        hi2c->Instance->I2C_OAR1, 
        (I2C_OAR1_ADDMODE_MASK || I2C_OAR1_ADD_MASK), 
        hi2c->Init.NoStretchMode
      );

    // Cấu hình OAR2

      memset(hi2c->Instance->RESERVED0, 0, sizeof(hi2c->Instance->RESERVED0));

    // Bật I2C

      I2C_PE_ENABLE(hi2c);

    // Cập nhật status

      hi2c->State = I2C_READY;
      hi2c->ErrorCode = I2C_ERR_NONE;

    return STAT_OK;
  }

  RETR_STAT I2C_DeInit(I2C_Handle_Param *hi2c) {
    // Kiểm tra param

      if (hi2c == NULL) {
        return STAT_ERROR;
      }

    // tắt I2C

      I2C_Disable(hi2c);

    // reset I2C

      I2C_SWRST_RESET(hi2c);

    // cập nhật status

      hi2c->State = I2C_READY;
      hi2c->ErrorCode = I2C_ERR_NONE;

    return STAT_OK;
  }

  RETR_STAT I2C_M_TX(
    I2C_Handle_Param *hi2c, const ui8* pdata, 
    ui16 size, ui32 timeout
  ) {
    // Bổ sung tickstart
      
      ui32 tickstart = SYSTICK_GetTick();

    // Kiểm tra state

      if (hi2c->State != I2C_READY) {
        return STAT_BUSY;
      }

    // Đợi cờ BSY
      
      if (I2C_FlagTimeout(hi2c, I2C_SR2_BUSY_MASK, SET, 100 , tickstart) != SET) {
        return STAT_BUSY;
      }

    // Kiểm tra lại I2C
      
      if (hi2c->Instance->I2C_CR1 & I2C_CR1_PE_MASK != I2C_CR1_PE_MASK) {
        I2C_PE_ENABLE(hi2c);
      }

    // Xóa cờ POS
      
      I2C_POS_DISABLE(hi2c);

    // Update state

      hi2c->State = I2C_BUSY_TX;
      hi2c->CurrentMode = I2C_MODE_MASTER;
      hi2c->ErrorCode = I2C_ERR_NONE;

    // Chuẩn bị tham số truyền tải

      hi2c->Buff_Ptr = pdata;
      hi2c->Xfer_Count = size;
      hi2c->Xfer_Size = size;

    // Gửi yêu cầu

      if (I2C_M_ReqWrite(hi2c, hi2c->TargetAddress, timeout, tickstart) != STAT_OK) {
        return STAT_ERROR;
      }

    // Bắt đầu vòng truyền

      while (hi2c->Xfer_Count > 0u) {
        // Đợi cờ TXE để đảm bảo data register đã sẵn sàng nhận dữ liệu mới

          if (
            I2C_FlagTimeout(
              hi2c,I2C_SR1_TXE_MASK,SET,
              timeout, tickstart
            ) != STAT_OK
          ) {
            if (READ_BIT(hi2c->Instance->I2C_SR1, I2C_SR1_AF_MASK)) {
              I2C_GENERATE_STOP(hi2c); // Gửi STOP để giải phóng bus
              I2C_CLEAR_AF(hi2c);      // Xóa cờ báo lỗi NACK
              hi2c->ErrorCode = I2C_ERR_AF; // Cập nhật mã lỗi vào handle_param
              return STAT_ERROR;
            }
          }

        // Gửi dữ liệu tiếp theo

          hi2c->Instance->I2C_DR = *(hi2c->Buff_Ptr++);
          hi2c->Xfer_Count--;
      }


  }

  RETR_STAT I2C_S_TX(
    I2C_Handle_Param *hi2c, const ui8* pdata, 
    ui16 size, ui32 timeout
  ) {

  }

  RETR_STAT I2C_M_RX(
    I2C_Handle_Param *hi2c, ui8* pdata, 
    ui16 size, ui32 timeout
  ) {

  }

  RETR_STAT I2C_S_RX(
    I2C_Handle_Param *hi2c, ui8* pdata, 
    ui16 size, ui32 timeout
  ) {

  }

  RETR_STAT I2C_IsDeviceReady(
    I2C_Handle_Param *hi2c, 
    ui16 DevAddress, 
    ui32 Trials, 
    ui32 Timeout
  ) {

  }