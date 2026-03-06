/*
 * lib_nvic_def.h
 *
 *  Created on: Mar 1, 2026
 *      Author: shanghuang
 */

#ifndef LIB_NVIC_DEF_H_
  #define LIB_NVIC_DEF_H_

  // Khai báo các thư viện sử dụng chung

    #ifndef UNIT_TEST
      #include "generic/lib_keyword_def.h"
      #include "nvic/lib_nvic_def.h"
    #else
      #include "lib_keyword_def.h"
      #include "lib_nvic_def.h"
    #endif

    #include <stdint.h>

  // Khai báo địa chỉ ngoại vi NVIC

    #define NVIC_REGS_BASEADDR 0xE000E100ul

  // Khai báo bộ thanh ghi của ngoại vi NVIC

    tdf_strc NVIC_REGS_Typedef {
      __vo BLANK_REG NVIC_ISER[3];    // Offset 0x00, reset 0x0000 0000
      __vo BLANK_REG RESERVED0[29];    // Offset 0x0C - 0x7F, reserved
      __vo BLANK_REG NVIC_ICER[3];    // Offset 0x80, reset 0x0000 0000
      __vo BLANK_REG RESERVED1[29];    // Offset 0x8C - 0xFF, reserved
      __vo BLANK_REG NVIC_ISPR[3];    // Offset 0x100, reset 0x0000 0000
      __vo BLANK_REG RESERVED2[29];    // Offset 0x10C - 0x17F, reserved
      __vo BLANK_REG NVIC_ICPR[3];    // Offset 0x180, reset 0x0000 0000
      __vo BLANK_REG RESERVED3[29];    // Offset 0x18C - 0x1FF, reserved
      __vo BLANK_REG NVIC_IABR[3];    // Offset 0x200, reset 0x0000 0000
      __vo BLANK_REG RESERVED4[61];    // Offset 0x20C - 0x2FF, reserved
      __vo ui8 NVIC_IPRE[68];     // Offset 0x300, reset 0x0000 0000
      __vo BLANK_REG RESERVED5[752];   // Offset 0x344 - 0xBFF, reserved
      __vo BLANK_REG NVIC_STIR;       // Offset 0xE00, reset 0x0000 0000
    } NVIC_REGS_Typedef;

    /**
     * Ghi chú:
     * Trong thiết kế phần cứng là sử dụng các thanh ghi IPR
     * Ở đây thiết kế là IPR-Element để ám chỉ một phần tử trong mảng IPR, tức là một thanh ghi IPR cụ thể
     * Mỗi thanh ghi IPR có thể chứa thông tin ưu tiên của 4 ngắt khác nhau, 
     * do đó ta sẽ sử dụng IPR-Element để chỉ một thanh ghi IPR cụ thể trong mảng IPR
     */

  // >> Tạo con trỏ phần cứng thật và con trỏ phần cứng giả tới ngoại vi

    #ifndef UNIT_TEST
      #define NVIC_REGS_PTR ((NVIC_REGS_Typedef *) NVIC_REGS_BASEADDR)
    #else
      extern NVIC_REGS_Typedef MOCK_NVIC_REGS;
      #define NVIC_REGS_PTR (&MOCK_NVIC_REGS)
    #endif

  // Khai báo các định nghĩa cần sử dụng trên NVIC_IPR
    
    #define NVIC_IP_NOT_IMPL_OFFSET 4u // Số bit không được sử dụng trong mỗi phần tử IPR (mỗi phần tử IPR có 8 bit nhưng chỉ sử dụng 4 bit để lưu thông tin ưu tiên)

#endif /* LIB_NVIC_DEF_H_ */
