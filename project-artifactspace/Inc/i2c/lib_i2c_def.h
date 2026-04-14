/*
 * lib_i2c_def.h
 *
 *  Created on: Apr 14, 2026
 *      Author: shanghuang
 */

#ifndef LIB_I2C_DEF_H_
  #define LIB_I2C_DEF_H_

  // Khai báo các thư viện sử dụng chung

    #include <stdint.h>

    #ifndef UNIT_TEST
      #include "generic/lib_keyword_def.h"
    #else
      #include "lib_keyword_def.h"
    #endif

  // Khai báo địa chỉ ngoại vi

    #define I2C_REGS_BASEADDR 0x40005400ul 

    /**
     * Ghi chú:
     * I2C trên dòng F1 có 2 cái là I2C1, I2C2.
     * Tuy nhiên I2C2 chỉ có trên dòng F1 cao cấp nên sẽ không hỗ trợ sử dụng trong thư viện I2C này.
     */

  // Khai báo bộ thanh ghi của ngoại vi

    tdf_strc I2C_REGS_Typedef {
      __vo BLANK_REG_16B I2C_CR1;         // Offset 0x00, reset 0x0000
      __vo BLANK_REG_16B I2C_CR2;         // Offset 0x04, reset 0x0000
      __vo BLANK_REG_16B I2C_OAR1;        // Offset 0x08, reset 0x0000
      __vo BLANK_REG_16B RESERVED0;       // Offset 0x0C, reset 0x0004 (OAR2 không sử dụng)
      __vo BLANK_REG_16B I2C_DR;          // Offset 0x10, reset 0x0000
      __vo BLANK_REG_16B I2C_SR1;         // Offset 0x14, reset 0x0000
      __vo BLANK_REG_16B I2C_SR2;         // Offset 0x18, reset 0x0000
      __vo BLANK_REG_16B I2C_CCR;         // Offset 0x1C, reset 0x0000
      __vo BLANK_REG_16B RESERVED1;       // Offset 0x20, reset 0x0002 (TRISE không sử dụng)
    } I2C_REGS_Typedef;

  // >> Tạo con trỏ phần cứng thật và con trỏ phần cứng giả tới ngoại vi

    #ifndef UNIT_TEST
      #define I2C_REGS_PTR ((I2C_REGS_Typedef *) I2C_REGS_BASEADDR)
    #else
      extern I2C_REGS_Typedef MOCK_I2C_REGS;
      #define I2C_REGS_PTR (&MOCK_I2C_REGS)
    #endif

  // Khai báo các định nghĩa bit cần sử dụng trên thanh ghi I2C_CR1

    #define I2C_CR1_PE_POS 0u
    #define I2C_CR1_PE_MASK (1u << I2C_CR1_PE_POS)

    #define I2C_CR1_SMBUS_POS 1u
    #define I2C_CR1_SMBUS_MASK (1u << I2C_CR1_SMBUS_POS)

    #define I2C_CR1_SMBTYPE_POS 3u
    #define I2C_CR1_SMBTYPE_MASK (1u << I2C_CR1_SMBTYPE_POS)

    #define I2C_CR1_ENARP_POS 4u
    #define I2C_CR1_ENARP_MASK (1u << I2C_CR1_ENARP_POS)

    #define I2C_CR1_ENPEC_POS 5u
    #define I2C_CR1_ENPEC_MASK (1u << I2C_CR1_ENPEC_POS)

    #define I2C_CR1_ENGC_POS 6u
    #define I2C_CR1_ENGC_MASK (1u << I2C_CR1_ENGC_POS)

    #define I2C_CR1_NOSTRETCH_POS 7u
    #define I2C_CR1_NOSTRETCH_MASK (1u << I2C_CR1_NOSTRETCH_POS)

    #define I2C_CR1_START_POS 8u
    #define I2C_CR1_START_MASK (1u << I2C_CR1_START_POS)

    #define I2C_CR1_STOP_POS 9u
    #define I2C_CR1_STOP_MASK (1u << I2C_CR1_STOP_POS)

    #define I2C_CR1_ACK_POS 10u
    #define I2C_CR1_ACK_MASK (1u << I2C_CR1_ACK_POS)

    #define I2C_CR1_POS_POS 11u
    #define I2C_CR1_POS_MASK (1u << I2C_CR1_POS_POS)

    #define I2C_CR1_PEC_POS 12u
    #define I2C_CR1_PEC_MASK (1u << I2C_CR1_PEC_POS)

    #define I2C_CR1_ALERT_POS 13u
    #define I2C_CR1_ALERT_MASK (1u << I2C_CR1_ALERT_POS)

    #define I2C_CR1_SWRST_POS 15u
    #define I2C_CR1_SWRST_MASK (1u << I2C_CR1_SWRST_POS)

  // Khai báo các định nghĩa bit cần sử dụng trên thanh ghi I2C_CR2

    #define I2C_CR2_FREQ_POS 0u
    #define I2C_CR2_FREQ_MASK (0x3Fu << I2C_CR2_FREQ_POS)

    #define I2C_CR2_ITERREN_POS 8u
    #define I2C_CR2_ITERREN_MASK (1u << I2C_CR2_ITERREN_POS)

    #define I2C_CR2_ITEVTEN_POS 9u
    #define I2C_CR2_ITEVTEN_MASK (1u << I2C_CR2_ITEVTEN_POS)

    #define I2C_CR2_ITBUFEN_POS 10u
    #define I2C_CR2_ITBUFEN_MASK (1u << I2C_CR2_ITBUFEN_POS)

  // Khai báo các định nghĩa bit cần sử dụng trên thanh ghi I2C_OAR1

    #define I2C_OAR1_ADDMODE_POS 15u
    #define I2C_OAR1_ADDMODE_MASK (1u << I2C_OAR1_ADDMODE_POS)

    #define I2C_OAR1_ADD_POS 0u
    #define I2C_OAR1_ADD_MASK (0x3FFu << I2C_OAR1_ADD_POS)

    /**
     * Ghi chú:
     * Trong tài liệu thì bit 14 cần được giữ 1 mặc dù là reserved.
     * 0x3FFF tương đương 0011 1111 1111, đảm bảo dãy địa chỉ 10bit và 7bit cover
     */

  // Khai báo các định nghĩa bit cần sử dụng trên thanh ghi I2C_OAR2

    /**
     * Ghi chú:
     * Chế độ Dual Addressing dành cho slave.
     * Trong thiết kế hoạt động thì STM32 đóng vai trò là master
     * nên sẽ không hỗ trợ chế độ này trong thư viện I2C này.
     */

  // Khai báo các định nghĩa bit cần sử dụng trên thanh ghi I2C_SR1

    #define I2C_SR1_SB_POS 0u
    #define I2C_SR1_SB_MASK (1u << I2C_SR1_SB_POS)

    #define I2C_SR1_ADDR_POS 1u
    #define I2C_SR1_ADDR_MASK (1u << I2C_SR1_ADDR_POS)

    #define I2C_SR1_BTF_POS 2u
    #define I2C_SR1_BTF_MASK (1u << I2C_SR1_BTF_POS)

    #define I2C_SR1_ADD10_POS 3u
    #define I2C_SR1_ADD10_MASK (1u << I2C_SR1_ADD10_POS)

    #define I2C_SR1_STOPF_POS 4u
    #define I2C_SR1_STOPF_MASK (1u << I2C_SR1_STOPF_POS)

    #define I2C_SR1_RXNE_POS 6u
    #define I2C_SR1_RXNE_MASK (1u << I2C_SR1_RXNE_POS)

    #define I2C_SR1_TXE_POS 7u
    #define I2C_SR1_TXE_MASK (1u << I2C_SR1_TXE_POS)

    #define I2C_SR1_BERR_POS 8u
    #define I2C_SR1_BERR_MASK (1u << I2C_SR1_BERR_POS)

    #define I2C_SR1_ARLO_POS 9u
    #define I2C_SR1_ARLO_MASK (1u << I2C_SR1_ARLO_POS)

    #define I2C_SR1_AF_POS 10u
    #define I2C_SR1_AF_MASK (1u << I2C_SR1_AF_POS)

    #define I2C_SR1_OVR_POS 11u
    #define I2C_SR1_OVR_MASK (1u << I2C_SR1_OVR_POS)

    #define I2C_SR1_PECERR_POS 12u
    #define I2C_SR1_PECERR_MASK (1u << I2C_SR1_PECERR_POS)

    #define I2C_SR1_TIMEOUT_POS 14u
    #define I2C_SR1_TIMEOUT_MASK (1u << I2C_SR1_TIMEOUT_POS)

  // Khai báo các định nghĩa bit cần sử dụng trên thanh ghi I2C_SR2

    #define I2C_SR2_MSL_POS 0u
    #define I2C_SR2_MSL_MASK (1u << I2C_SR2_MSL_POS)

    #define I2C_SR2_BUSY_POS 1u
    #define I2C_SR2_BUSY_MASK (1u << I2C_SR2_BUSY_POS)

    #define I2C_SR2_TRA_POS 2u
    #define I2C_SR2_TRA_MASK (1u << I2C_SR2_TRA_POS)

  // Khai báo các định nghĩa bit cần sử dụng trên thanh ghi I2C_CCR

    #define I2C_CCR_CCR_POS 0u
    #define I2C_CCR_CCR_MASK (0xFFFu << I2C_CCR_CCR_POS)

    #define I2C_CCR_DUTY_POS 14u
    #define I2C_CCR_DUTY_MASK (1u << I2C_CCR_DUTY_POS)

    #define I2C_CCR_F_S_POS 15u
    #define I2C_CCR_F_S_MASK (1u << I2C_CCR_F_S_POS)

  // Khai báo các chế độ duty 
    
    #define I2C_DUTY_2 0x00000000u
    #define I2C_DUTY_16_9 I2C_CCR_DUTY_MASK

  // Khai báo các chế độ tốc độ I2C

    #define I2C_SPEED_STANDARD 0x00000000u
    #define I2C_SPEED_FAST I2C_CCR_F_S_MASK

  // Khai báo các chế độ định địa chỉ

    #define I2C_ADDRESSINGMODE_7BIT 0x00000000u
    #define I2C_ADDRESSINGMODE_10BIT I2C_OAR1_ADDMODE_MASK

  // Khai báo chế độ streching 

    #define I2C_NOSTRETCH_DISABLE 0x00000000u
    #define I2C_NOSTRETCH_ENABLE I2C_CR1_NOSTRETCH_MASK

#endif /* LIB_I2C_DEF_H_ */
