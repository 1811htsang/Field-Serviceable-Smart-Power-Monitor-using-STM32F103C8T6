/*
 * lib_i2c_hal.h
 *
 *  Created on: Apr 14, 2026
 *      Author: shanghuang
 */

#ifndef LIB_I2C_HAL_H_
  #define LIB_I2C_HAL_H_

  // Khai báo các thư viện sử dụng chung

    #include <stdint.h>

    #ifndef UNIT_TEST
      #include "generic/lib_keyword_def.h";
      #include "i2c/lib_i2c_def.h";
    #else
      #include "lib_keyword_def.h";
      #include "lib_i2c_def.h";
    #endif

  // Khai báo cấu trúc tham số hàm khởi tạo

    #ifndef I2C_INIT_PARAM_TYPE
      #define I2C_INIT_PARAM_TYPE
        tdf_strc I2C_Init_Param { // Cấu trúc tham số để khởi tạo ngoại vi I2C
          ui32 Mode;              // Chọn chế độ hoạt động cho I2C (Master/Slave)
          ui32 ClockSpeed;        // Chọn tốc độ clock cho I2C (tối đa 400kHz)
          ui32 DutyCycle;         // Chọn chu kỳ nhiệm vụ trong chế độ fast mode
          ui32 OwnAddress;        // Chọn địa chỉ riêng của thiết bị (7-bit hoặc 10-bit)
          ui32 AddressingMode;    // Chọn chế độ định địa chỉ (7-bit hoặc 10-bit)
          ui32 NoStretchMode;     // Chọn chế độ không kéo dài tín hiệu (Enable/Disable)
        } I2C_Init_Param;
    #endif

  // Khai báo trạng thái trả về

    #ifndef I2C_STAT_ENUM_TYPE
      #define I2C_STAT_ENUM_TYPE
        tdf_enum I2C_STAT_Enum {
          I2C_STAT_OK     = 0x00u,
          I2C_READY       = 0x01u,
          I2C_BUSY        = 0x02u,
          I2C_BUSY_TX     = 0x03u,
          I2C_BUSY_RX     = 0x04u,
          I2C_ERROR       = 0x05u,
          I2C_TIMEOUT     = 0x06u,
          I2C_RESET       = 0x07u
        } I2C_STAT_Enum;
    #endif

  // Khai báo bộ tập hợp mã lỗi

    #ifndef I2C_ERR_ENUM_TYPE
      #define I2C_ERR_ENUM_TYPE
        tdf_enum I2C_ERR_Enum {
          I2C_ERR_NONE    = 0x00u,
          I2C_ERR_BERR    = 0x01u,    // Bus error
          I2C_ERR_ARLO    = 0x02u,    // Arbitration lost
          I2C_ERR_AF      = 0x03u,    // Acknowledge failure
          I2C_ERR_OVR     = 0x04u,    // Overrun/Underrun
          I2C_ERR_PEC     = 0x05u,    // PEC error in reception
          I2C_ERR_TIMEOUT = 0x06u,    // Timeout error
          I2C_ERR_START   = 0x07u,    // Start condition error
          I2C_ERR_STOP    = 0x08u,    // Stop condition error
          I2C_ERR_ADDR_NACK = 0x09u,  // NACK received after sending address
          I2C_ERR_UNKNOWN = 0xFFu     // Unknown error
        } I2C_ERR_Enum;
    #endif

  // Khai báo bộ tập hợp lựa chọn chế độ

    #ifndef I2C_MODE_ENUM_TYPE
      #define I2C_MODE_ENUM_TYPE
        tdf_enum I2C_MODE_Enum {
          I2C_MODE_MASTER = 0x00u,
          I2C_MODE_SLAVE  = 0x01u
        } I2C_MODE_Enum;
    #endif

  // Khai báo khối quản lý tham số

    #ifndef I2C_HANDLE_PARAM_TYPE
      #define I2C_HANDLE_PARAM_TYPE
        tdf_strc I2C_Handle_Param { // Cấu trúc tham số để quản lý trạng thái và quá trình truyền nhận của ngoại vi I2C
          I2C_REGS_Typedef *Instance; // Con trỏ tới bộ thanh ghi của ngoại vi I2C
          I2C_Init_Param Init;         // Tham số cấu hình khởi tạo cho ngoại vi I2C

          // Quản lý truyền nhận dữ liệu
          const ui8* Buff_Ptr;     // Con trỏ tới buffer chứa dữ liệu cần truyền
          ui16 Xfer_Size;          // Kích thước dữ liệu cần truyền (tính theo số lượng phần tử, không phải số byte)
          __vo ui16 Xfer_Count;    // Biến đếm số lượng phần tử đã truyền được

          /**
           * Ghi chú:
           * Khác với SPI, I2C là truyền half-duplex 
           * nên không cần phải quản lý buffer nhận và buffer truyền riêng biệt.
           */

          __vo I2C_STAT_Enum State;    // Trạng thái hiện tại của ngoại vi I2C
          __vo I2C_ERR_Enum ErrorCode; // Mã lỗi nếu có lỗi xảy ra
          __vo I2C_MODE_Enum CurrentMode; // Trạng thái hiện tại của I2C

          __vo ui16 TargetAddress;           // Địa chỉ của thiết bị I2C mục tiêu trong quá trình truyền nhận
        } I2C_Handle_Param;
    #endif

  // Khai báo định nghĩa chiều truyền nhận dữ liệu

    #ifndef I2C_DIRECTION_ENUM_TYPE
      #define I2C_DIRECTION_ENUM_TYPE
        tdf_enum I2C_DIRECTION_Enum {
          I2C_DIRECTION_TX = 0x00u, // Chế độ truyền dữ liệu
          I2C_DIRECTION_RX = 0x01u  // Chế độ nhận dữ liệu
        } I2C_DIRECTION_Enum;
    #endif

  // Khai báo các kiểm tra nội bộ

    #define IS_I2C_MODE(MODE) (((MODE) == I2C_MODE_MASTER) || \
                              ((MODE) == I2C_MODE_SLAVE))

    #define IS_I2C_CLOCKSPEED(SPEED) ((SPEED) <= 400000U)

    #define IS_I2C_DUTYCYCLE(DUTY) (((DUTY) == I2C_DUTY_2) || \
                                    ((DUTY) == I2C_DUTY_16_9))

    #define IS_I2C_OWNADDRESS(ADDRESS) ((ADDRESS) <= 0x3FFU)

    #define IS_I2C_ADDRESSINGMODE(MODE) (((MODE) == I2C_ADDRESSINGMODE_7BIT) || \
                                        ((MODE) == I2C_ADDRESSINGMODE_10BIT))

    #define IS_I2C_NOSTRETCH(NOSTRETCH) (((NOSTRETCH) == I2C_NOSTRETCH_ENABLE) || \
                                        ((NOSTRETCH) == I2C_NOSTRETCH_DISABLE))

    #define IS_I2C_FREQUENCY(FREQ, CFG) ((FREQ) <= (CFG))

    /**
     * Ghi chú:
     * 0x32 = 00110010 tương đương cấu hình tối đa 50MHz.
     */

  // Định nghĩa các macro hỗ trợ nội bộ

    #define I2C_SWRST_RESET(hi2c) do { \
      SET_BIT((hi2c)->Instance->I2C_CR1, I2C_CR1_SWRST_MASK); \
      CLEAR_BIT((hi2c)->Instance->I2C_CR1, I2C_CR1_SWRST_MASK); \
    } while(0)

    #define I2C_PE_ENABLE(hi2c) do { \
      SET_BIT((hi2c)->Instance->I2C_CR1, I2C_CR1_PE_MASK); \
    } while(0)

    #define I2C_PE_DISABLE(hi2c) do { \
      CLEAR_BIT((hi2c)->Instance->I2C_CR1, I2C_CR1_PE_MASK); \
    } while(0)

    #define I2C_GENERATE_START(hi2c) do { \
      SET_BIT((hi2c)->Instance->I2C_CR1, I2C_CR1_START_MASK); \
    } while(0)

    #define I2C_GENERATE_STOP(hi2c) do { \
      SET_BIT((hi2c)->Instance->I2C_CR1, I2C_CR1_STOP_MASK); \
    } while(0)

    #define I2C_ACK_ENABLE(hi2c) do { \
      SET_BIT((hi2c)->Instance->I2C_CR1, I2C_CR1_ACK_MASK); \
    } while(0)

    #define I2C_ACK_DISABLE(hi2c) do { \
      CLEAR_BIT((hi2c)->Instance->I2C_CR1, I2C_CR1_ACK_MASK); \
    } while(0)

    #define I2C_POS_ENABLE(hi2c) do { \
      SET_BIT((hi2c)->Instance->I2C_CR1, I2C_CR1_POS_MASK); \
    } while(0)

    #define I2C_POS_DISABLE(hi2c) do { \
      CLEAR_BIT((hi2c)->Instance->I2C_CR1, I2C_CR1_POS_MASK); \
    } while(0)

    #define I2C_ENPEC_ENABLE(hi2c) do { \
      SET_BIT((hi2c)->Instance->I2C_CR1, I2C_CR1_ENPEC_MASK); \
    } while(0)

    #define I2C_ENPEC_DISABLE(hi2c) do { \
      CLEAR_BIT((hi2c)->Instance->I2C_CR1, I2C_CR1_ENPEC_MASK); \
    } while(0)

    #define I2C_PEC_TX_ENABLE(hi2c) do { \
      SET_BIT((hi2c)->Instance->I2C_CR1, I2C_CR1_PEC_MASK); \
    } while(0)

    #define I2C_PEC_TX_DISABLE(hi2c) do { \
      CLEAR_BIT((hi2c)->Instance->I2C_CR1, I2C_CR1_PEC_MASK); \
    } while(0)

    #define I2C_FREQ_CONFIG(hi2c, FREQ) do { \
      MODIFY_REG( \
        (hi2c)->Instance->I2C_CR2, \
        I2C_CR2_FREQ_MASK, (FREQ) << I2C_CR2_FREQ_POS); \
    } while(0)

    #define I2C_ADDR_CONFIG(hi2c, ADDR) do { \
      MODIFY_REG((hi2c)->Instance->I2C_OAR1, \
      I2C_OAR1_ADD_MASK, (ADDR) << I2C_OAR1_ADD_POS); \
    } while(0)

    #define I2C_DATA_CONFIG(hi2c, DATA) do { \
      ((hi2c)->Instance->DR) = (DATA); \
    } while(0)

    #define I2C_CCR_GENERIC_CONFIG(PCLK, SPEED, COEFF) (((((PCLK) - 1u)/((SPEED) * (COEFF))) + 1u) & I2C_CCR_CCR_MASK)

    #define I2C_CCR_STANDARD(PCLK, SPEED) ((I2C_CCR_GENERIC_CONFIG((PCLK), (SPEED), 2u) < 4u)? 4u:I2C_CCR_GENERIC_CONFIG((PCLK), (SPEED), 2u))
 
    #define I2C_CCR_FAST(PCLK, SPEED, DUTY) (((DUTY) == I2C_DUTY_2)? I2C_CCR_GENERIC_CONFIG((PCLK), (SPEED), 3u) : (I2C_CCR_CALCULATION((PCLK), (SPEED), 25u) | I2C_DUTY_16_9))

    #define I2C_CCR_AUTO(PCLK, SPEED, DUTY) (((SPEED) <= 100000u)? (I2C_CCR_STANDARD((PCLK), (SPEED))) : \
                                            ((I2C_CCR_FAST((PCLK), (SPEED), (DUTY)) & I2C_CCR_CCR_MASK) == 0u)? 1u : \
                                            ((I2C_CCR_FAST((PCLK), (SPEED), (DUTY))) | I2C_CCR_F_S_MASK))

    /**
     * Ghi chú:
     * Data của I2C mặc định chỉ có 8-bit 
     * nên không cần phải cấu hình kích thước dữ liệu như SPI.
     */

    #define I2C_GET_FLAG(hi2c, FLAG) (READ_BIT((hi2c)->Instance->SR1, (FLAG)))

  // Khai báo các hàm thành phần

    // >> Khai báo hàm vô hiệu hóa I2C

    stinl void I2C_Disable(I2C_Handle_Param *hi2c) {
      CLEAR_BIT(
        hi2c->Instance->I2C_CR1,
        I2C_CR1_PE_MASK
      );
    }

    // >> Các hàm khởi tạo và giải phóng ngoại vi
    RETR_STAT I2C_Init(I2C_Handle_Param *hi2c);
    RETR_STAT I2C_DeInit(I2C_Handle_Param *hi2c);

    // >> Hàm truyền nhận dữ liệu cơ bản (blocking mode)

    RETR_STAT I2C_M_TX(
      I2C_Handle_Param *hi2c, 
      const ui8* pdata, 
      ui16 size, 
      ui32 timeout
    );

    RETR_STAT I2C_S_TX(
      I2C_Handle_Param *hi2c, 
      const ui8* pdata, 
      ui16 size, 
      ui32 timeout
    );

    // >> Hàm nhận dữ liệu cơ bản (blocking mode)

    RETR_STAT I2C_M_RX(
      I2C_Handle_Param *hi2c, 
      ui8* pdata, 
      ui16 size, 
      ui32 timeout
    );

    RETR_STAT I2C_S_RX(
      I2C_Handle_Param *hi2c, 
      ui8* pdata, 
      ui16 size, 
      ui32 timeout
    );

    // >> Hàm kiểm tra thiết bị 
    RETR_STAT I2C_IsDeviceReady(
      I2C_Handle_Param *hi2c, 
      ui16 DevAddress, 
      ui32 Trials, 
      ui32 Timeout
    );

#endif /* LIB_I2C_HAL_H_ */
