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
  #endif

// Khai báo biến quản lý thời gian

  __vo ui32 ms_ticks = 0;

// Khai báo các hàm nội bộ
  
  /*
   * Hàm chờ cờ I2C đạt trạng thái mong muốn trong giới hạn timeout.
   *
   * Tham số:
   *   hi2c - Con trỏ tới handle I2C đang thao tác.
   *   flag_mask - Mặt nạ cờ cần kiểm tra.
   *   desired_status - Trạng thái mong muốn của cờ sau khi chờ.
   *   timeout - Thời gian chờ tối đa tính bằng ms.
   *   tickstart - Tick khởi đầu để tính timeout.
   *   RegisterID - ID thanh ghi cần đọc, dùng để phân biệt SR1 và SR2.
   *
   * Logic:
   *   - Đọc thanh ghi tương ứng theo RegisterID.
   *   - Chờ tới khi cờ đạt trạng thái mong muốn hoặc timeout xảy ra.
   *   - Cập nhật mã lỗi timeout nếu vượt quá thời gian chờ.
   *
   * Trả về:
   *   RETR_STAT - STAT_OK nếu cờ hợp lệ, STAT_TIMEOUT nếu hết thời gian chờ.
   */
  sta RETR_STAT I2C_FlagTimeout(
    I2C_Handle_Param *hi2c, 
    ui32 flag_mask, 
    ui32 desired_status, 
    ui32 timeout, 
    ui32 tickstart,
    ui8 RegisterID
  );

  // >> Hàm request master write
  sta RETR_STAT I2C_M_ReqWrite(
    I2C_Handle_Param *hi2c, ui16 DevAddress, 
    ui16 Timeout, ui32 Tickstart
  );

  // >> Hàm request master read
  sta RETR_STAT I2C_M_ReqRead(
    I2C_Handle_Param *hi2c, ui16 DevAddress, 
    ui16 Size, ui32 Timeout, ui32 Tickstart
  );

// Định nghĩa các hàm thành phần

  sta RETR_STAT I2C_FlagTimeout(
    I2C_Handle_Param *hi2c, 
    ui32 flag_mask, 
    ui32 desired_status, 
    ui32 timeout, 
    ui32 tickstart,
    ui8 RegisterID
  ) {

    // Khởi tạo biến đếm thời gian đã trôi qua

      ui32 elapsed = 0u;

    // Vòng lặp chờ cờ với timeout

      while (1) {
        ui32 reg_value = (RegisterID == 1) ? hi2c->Instance->I2C_SR1 : hi2c->Instance->I2C_SR2;
        if (((READ_BIT(reg_value, flag_mask) != 0) ? SET : RESET) == desired_status) {
          return STAT_OK;
        }

        elapsed = SYSTICK_GetTick() - tickstart;
        if (elapsed >= timeout) {
          hi2c->ErrorCode = I2C_ERR_TIMEOUT;
          return STAT_TIMEOUT;
        }
      }
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
          Timeout, Tickstart, 1
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
          hi2c->ErrorCode = I2C_ERR_TIMEOUT;
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
          Timeout, Tickstart, 1
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
          hi2c->ErrorCode = I2C_ERR_TIMEOUT;
          return STAT_TIMEOUT;
        }
      }

    return STAT_OK; // Yêu cầu thành công, đã gửi địa chỉ và sẵn sàng nhận dữ liệu
  }

  RETR_STAT I2C_Init(I2C_Handle_Param *hi2c) {
    if (hi2c == NULL) {
      return STAT_ERROR;
    }

    I2C_Disable(hi2c);
    I2C_SWRST_RESET(hi2c);

    ui32 freq = RCC_Get_PCLK1_Freq() / 1000000u;
    MODIFY_REG(hi2c->Instance->I2C_CR2, I2C_CR2_FREQ_MASK, freq);

    ui32 trise = (hi2c->Init.ClockSpeed <= 100000u) ? (freq + 1u) : ((freq * 300u) / 1000u + 1u);
    MODIFY_REG(hi2c->Instance->I2C_TRISE, I2C_TRISE_MASK, trise);

    ui32 ccr = I2C_CCR_AUTO(freq, hi2c->Init.ClockSpeed, hi2c->Init.DutyCycle);
    MODIFY_REG(hi2c->Instance->I2C_CCR, I2C_CCR_CCR_MASK | I2C_CCR_F_S_MASK | I2C_CCR_DUTY_MASK, ccr);

    MODIFY_REG(
      hi2c->Instance->I2C_OAR1,
      I2C_OAR1_ADDMODE_MASK | I2C_OAR1_ADD_MASK,
      hi2c->Init.AddressingMode | (hi2c->Init.OwnAddress << I2C_OAR1_ADD_POS)
    );

    I2C_PE_ENABLE(hi2c);
    hi2c->State = I2C_READY;
    hi2c->ErrorCode = I2C_ERR_NONE;

    return STAT_OK;
  }

  RETR_STAT I2C_DeInit(I2C_Handle_Param *hi2c) {
    if (hi2c == NULL) {
      return STAT_ERROR;
    }

    I2C_Disable(hi2c);
    I2C_SWRST_RESET(hi2c);

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
      
      if (I2C_FlagTimeout(hi2c, I2C_SR2_BUSY_MASK, SET, 100 , tickstart, 2) != SET) {
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

      hi2c->Buff_Ptr = (ui8*)pdata;
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
              hi2c, I2C_SR1_TXE_MASK, SET,
              timeout, tickstart, 1
            ) != STAT_OK
          ) {
            if (READ_BIT(hi2c->Instance->I2C_SR1, I2C_SR1_AF_MASK)) {
              I2C_GENERATE_STOP(hi2c); // Gửi STOP để giải phóng bus
              I2C_CLEAR_AF(hi2c);      // Xóa cờ báo lỗi NACK
              hi2c->ErrorCode = I2C_ERR_AF; // Cập nhật mã lỗi vào handle_param
              return STAT_ERROR;
            }
          }

        // Gửi dữ liệu thứ N tiếp theo

          hi2c->Instance->I2C_DR = *(hi2c->Buff_Ptr++);
          hi2c->Xfer_Count--;
          hi2c->Xfer_Size--;

        // Kiểm ra cờ BTF với dữ liệu còn tiếp tục truyền để nạp dữ liệu thứ N + 1 tiếp theo

          /**
           * Ghi chú:
           * Trong trường hợp này kiểm tra khi byte thứ N đã rời khỏi thanh ghi dịch
           * trong pipeline thì cờ BTF bật lên để nạp tiếp byte thứ N + 1 fill up pipeline
           */

          if (
            I2C_FlagTimeout(
              hi2c, I2C_SR1_BTF_MASK, SET,
              timeout, tickstart, 1
            ) == STAT_OK
            &&
            hi2c->Xfer_Size != 0u
          ) {
            hi2c->Instance->I2C_DR = *(hi2c->Buff_Ptr++);
            hi2c->Xfer_Count--;
            hi2c->Xfer_Size--;
          }

        // Kiểm tra lại cờ BTF 1 lần nữa đối với byte thứ N + 1 này

          if (
            I2C_FlagTimeout(
              hi2c, I2C_SR1_BTF_MASK, SET,
              timeout, tickstart, 1
            ) != STAT_OK
          ) {
            if (READ_BIT(hi2c->Instance->I2C_SR1, I2C_SR1_AF_MASK)) {
              I2C_GENERATE_STOP(hi2c); // Gửi STOP để giải phóng bus
              I2C_CLEAR_AF(hi2c);      // Xóa cờ báo lỗi NACK
              hi2c->ErrorCode = I2C_ERR_AF; // Cập nhật mã lỗi vào handle_param
              return STAT_ERROR;
            }
          }
      }

    // Stop quá trình truyền

      I2C_GENERATE_STOP(hi2c);

    // Khai báo status trả về

      hi2c->State = I2C_READY;

    return STAT_OK;
  }

  RETR_STAT I2C_S_TX(
    I2C_Handle_Param *hi2c, const ui8* pdata, 
    ui16 size, ui32 timeout
  ) {
    // Initialize tickstart
    ui32 tickstart = SYSTICK_GetTick();

    // Check state
    if (hi2c->State != I2C_READY) {
      return STAT_BUSY;
    }

    // Update state
    hi2c->State = I2C_BUSY_TX;
    hi2c->CurrentMode = I2C_MODE_SLAVE;
    hi2c->ErrorCode = I2C_ERR_NONE;

    // Prepare transfer parameters
    hi2c->Buff_Ptr = pdata;
    hi2c->Xfer_Count = size;
    hi2c->Xfer_Size = size;

    // Transmit data
    while (hi2c->Xfer_Count > 0u) {
      // Wait for TXE flag
      if (
        I2C_FlagTimeout(
          hi2c, I2C_SR1_TXE_MASK, SET,
          timeout, tickstart, 1
        ) != STAT_OK
      ) {
        hi2c->ErrorCode = I2C_ERR_TXE;
        return STAT_ERROR;
      }

      // Write data to DR
      hi2c->Instance->I2C_DR = *(hi2c->Buff_Ptr++);
      hi2c->Xfer_Count--;
    }

    // Wait for BTF flag
    if (
      I2C_FlagTimeout(
        hi2c, I2C_SR1_BTF_MASK, SET,
        timeout, tickstart, 1
      ) != STAT_OK
    ) {
      hi2c->ErrorCode = I2C_ERR_BTF;
      return STAT_ERROR;
    }

    // Update state
    hi2c->State = I2C_READY;

    return STAT_OK;
  }

  RETR_STAT I2C_M_RX(
    I2C_Handle_Param *hi2c, ui8* pdata, 
    ui16 size, ui32 timeout
  ) {
    ui32 tickstart = SYSTICK_GetTick();

    if (hi2c->State != I2C_READY) {
      return STAT_BUSY;
    }

    hi2c->State = I2C_BUSY_RX;
    hi2c->CurrentMode = I2C_MODE_MASTER;
    hi2c->ErrorCode = I2C_ERR_NONE;

    if (
      I2C_M_ReqRead(
        hi2c, hi2c->TargetAddress, size, timeout, tickstart
      ) != STAT_OK
    ) {
      return STAT_ERROR;
    }

    if (size == 1u) {
      I2C_ACK_DISABLE(hi2c);
      __vo ui32 tmpreg = hi2c->Instance->I2C_SR1;
      tmpreg = hi2c->Instance->I2C_SR2; // Xóa ADDR
      I2C_GENERATE_STOP(hi2c);
    } else if (size == 2u) {
      I2C_POS_ENABLE(hi2c);
      I2C_ACK_DISABLE(hi2c);
      __vo ui32 tmpreg = hi2c->Instance->I2C_SR1;
      tmpreg = hi2c->Instance->I2C_SR2; // Xóa ADDR
    } else {
      __vo ui32 tmpreg = hi2c->Instance->I2C_SR1;
      tmpreg = hi2c->Instance->I2C_SR2;
    }

    while (hi2c->Xfer_Count > 0u) {
      // Đợi cờ RXNE
      if (I2C_FlagTimeout(hi2c, I2C_SR1_RXNE_MASK, SET, timeout, tickstart, 1) != STAT_OK) {
        hi2c->ErrorCode = I2C_ERR_RXNE;
        return STAT_ERROR;
      }

      if (hi2c->Xfer_Count == 3u) {
        // Khi còn 3 byte, đợi byte thứ N-2 xong (BTF) để tắt ACK cho byte cuối
        while(!READ_BIT(hi2c->Instance->I2C_SR1, I2C_SR1_BTF_MASK));
        I2C_ACK_DISABLE(hi2c);
      }
      else if (hi2c->Xfer_Count == 2u && size > 2u) {
        // Khi còn 2 byte, gửi STOP
        while(!READ_BIT(hi2c->Instance->I2C_SR1, I2C_SR1_BTF_MASK));
        I2C_GENERATE_STOP(hi2c);
      }

      *(hi2c->Buff_Ptr++) = (ui8)hi2c->Instance->I2C_DR;
      hi2c->Xfer_Count--;
    }

    hi2c->State = I2C_READY;
    hi2c->ErrorCode = I2C_ERR_NONE;

    return STAT_OK;
  }

  RETR_STAT I2C_S_RX(
    I2C_Handle_Param *hi2c, ui8* pdata, 
    ui16 size, ui32 timeout
  ) {
    // Initialize tickstart
    ui32 tickstart = SYSTICK_GetTick();

    // Check state
    if (hi2c->State != I2C_READY) {
      return STAT_BUSY;
    }

    // Update state
    hi2c->State = I2C_BUSY_RX;
    hi2c->CurrentMode = I2C_MODE_SLAVE;
    hi2c->ErrorCode = I2C_ERR_NONE;

    // Prepare transfer parameters
    hi2c->Buff_Ptr = pdata;
    hi2c->Xfer_Count = size;
    hi2c->Xfer_Size = size;

    // Receive data
    while (hi2c->Xfer_Count > 0u) {
      // Wait for RXNE flag
      if (
        I2C_FlagTimeout(
          hi2c, I2C_SR1_RXNE_MASK, SET,
          timeout, tickstart, 1
        ) != STAT_OK
      ) {
        hi2c->ErrorCode = I2C_ERR_RXNE;
        return STAT_ERROR;
      }

      // Read data from DR
      *(hi2c->Buff_Ptr++) = hi2c->Instance->I2C_DR;
      hi2c->Xfer_Count--;
    }

    // Update state
    hi2c->State = I2C_READY;

    hi2c->ErrorCode = I2C_ERR_NONE;

    return STAT_OK;
  }

  RETR_STAT I2C_IsDeviceReady(
    I2C_Handle_Param *hi2c, 
    ui16 DevAddress, 
    ui32 Trials, 
    ui32 Timeout
  ) {
    ui32 tickstart = SYSTICK_GetTick();

    if (hi2c->State != I2C_READY) {
      return STAT_BUSY;
    }

    hi2c->State = I2C_BUSY;
    hi2c->ErrorCode = I2C_ERR_NONE;

    for (ui32 trial = 0; trial < Trials; trial++) {
      if (I2C_M_ReqWrite(hi2c, DevAddress, Timeout, tickstart) == STAT_OK) {
        I2C_GENERATE_STOP(hi2c);
        hi2c->State = I2C_READY;
        return STAT_OK;
      } else {
        I2C_GENERATE_STOP(hi2c);
        I2C_CLEAR_AF(hi2c);
      }
    }

    hi2c->State = I2C_READY;
    hi2c->ErrorCode = I2C_ERR_ADDR_NACK;

    return STAT_ERROR;
  }