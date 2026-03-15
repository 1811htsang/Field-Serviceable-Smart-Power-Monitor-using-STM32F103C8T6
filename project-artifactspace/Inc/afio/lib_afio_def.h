/*
 * lib_afio_def.h
 *
 *  Created on: Feb 28, 2026
 *      Author: shanghuang
 */

#ifndef LIB_AFIO_DEF_H_
  #define LIB_AFIO_DEF_H_

  // Khai báo các thư viện sử dụng chung

    #include <stdint.h>

    #ifndef UNIT_TEST
      #include "generic/lib_keyword_def.h"
    #else
      #include "lib_keyword_def.h"
    #endif

  // Khai báo địa chỉ ngoại vi AFIO

    #define AFIO_REGS_BASEADDR  0x40010000ul

  // Khai báo bộ thanh ghi của ngoại vi AFIO

    tdf_strc AFIO_REGS_Typedef {
      __vo BLANK_REG_32B AFIO_EVCR;       // Offset 0x00
      __vo BLANK_REG_32B AFIO_MAPR;       // Offset 0x04
      __vo BLANK_REG_32B AFIO_EXTICR[4];  // Offset 0x08 - 0x14
      __vo BLANK_REG_32B RESERVED0;       // Offset 0x18
      __vo BLANK_REG_32B AFIO_MAPR2;      // Offset 0x1c
    } AFIO_REGS_Typedef;

  // Tạo con trỏ phần cứng thật và con trỏ phần cứng giả tới ngoại vi

    #ifndef UNIT_TEST
      #define AFIO_REGS_PTR ((AFIO_REGS_Typedef *) AFIO_REGS_BASEADDR)
    #else
      extern AFIO_REGS_Typedef MOCK_AFIO_REGS;
      #define AFIO_REGS_PTR (&MOCK_AFIO_REGS)
    #endif

  // Khai báo các định nghĩa bit cần sử dụng trên thanh ghi AFIO_MAPR

    #define AFIO_MAPR_SPI1_REMAP             (ui32)(1u << 0)  // Bit 0
    #define AFIO_MAPR_I2C1_REMAP             (ui32)(1u << 1)  // Bit 1
    #define AFIO_MAPR_USART1_REMAP           (ui32)(1u << 2)  // Bit 2
    #define AFIO_SWJ_CFG_FULL_REMAP          (ui32)(0x00u << 24) // Bits 24-26 = 000: Full SWJ (JTAG-DP + SW-DP): Reset State
    #define AFIO_SWJ_CFG_NO_NJTRST           (ui32)(0x01u << 24) // Bits 24-26 = 001: Full SWJ (JTAG-DP + SW-DP) but without NJTRST
    #define AFIO_SWJ_CFG_JTAG_NO_SW          (ui32)(0x02u << 24) // Bits 24-26 = 010: JTAG-DP disabled and SW-DP enabled
    #define AFIO_SWJ_CFG_NO_JTAG_NO_SW       (ui32)(0x04u << 24) // Bits 24-26 = 100: Full SWJ disabled (JTAG-DP + SW-DP)

    /**
     * Ghi chú:
     * Hiện tại chỉ sử dụng đến SPI, I2C và USART1 nên chỉ khai báo 3 bit remap này.
     * Nếu cần remap thêm ngoại vi khác trong tương lai thì sẽ bổ sung thêm các bit remap tương ứng.
     */

  // Khai báo các định nghĩa bit cần sử dụng trên các thanh ghi AFIO_EXTICR

    #define AFIO_EXTICR_PORTA               (ui8)(0x00u)
    #define AFIO_EXTICR_PORTB               (ui8)(0x01u)
    #define AFIO_EXTICR_PORTC               (ui8)(0x02u)
    #define AFIO_EXTICR_PORTD               (ui8)(0x03u)
    #define AFIO_EXTICR_PORTE               (ui8)(0x04u)
    #define AFIO_EXTICR_PORTF               (ui8)(0x05u)
    #define AFIO_EXTICR_PORTG               (ui8)(0x06u)
  
  // Khai báo các chế độ mapping chân sử dụng

    #define SPI_REMAP AFIO_MAPR_SPI1_REMAP
    #define I2C_REMAP AFIO_MAPR_I2C1_REMAP
    #define USART_REMAP AFIO_MAPR_USART1_REMAP

  // Khai báo bộ nhảy điều chỉnh trên các thanh ghi AFIO_EXTICR

    #define M4R0_SHIFT          (ui8)(0u)   // Không cần dịch chuyển vì đã ở vị trí bit 0-3
    #define M4R1_SHIFT          (ui8)(4u)   // Dịch chuyển 4 bit vì M4R1 nằm ở vị trí bit 4-7
    #define M4R2_SHIFT          (ui8)(8u)   // Dịch chuyển 8 bit vì M4R2 nằm ở vị trí bit 8-11
    #define M4R3_SHIFT          (ui8)(12u)  // Dịch chuyển 12 bit vì M4R3 nằm ở vị trí bit 12-15

#endif /* LIB_AFIO_DEF_H_ */
