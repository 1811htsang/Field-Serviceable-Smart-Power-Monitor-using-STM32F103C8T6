/*
 * lib_exti_def.h
 *
 *  Created on: Mar 1, 2026
 *      Author: shanghuang
 */

#ifndef LIB_EXTI_DEF_H_
  #define LIB_EXTI_DEF_H_

  // Khai báo các thư viện sử dụng chung

    #include <stdint.h>

    #ifndef UNIT_TEST
      #include "generic/lib_keyword_def.h"
    #else
      #include "lib_keyword_def.h"
    #endif

  // Khai báo địa chỉ ngoại vi EXTI

    #define EXTI_REGS_BASEADDR 0x40010400ul

  // Khai báo bộ thanh ghi của ngoại vi EXTI

    tdf_strc EXTI_REGS_Typedef {
      __vo BLANK_REG EXTI_IMR;        // Offset 0x00, reset 0x0000 0000
      __vo BLANK_REG EXTI_EMR;        // Offset 0x04, reset 0x0000 0000
      __vo BLANK_REG EXTI_RTSR;       // Offset 0x08, reset 0x0000 0000
      __vo BLANK_REG EXTI_FTSR;       // Offset 0x0C, reset 0x0000 0000
      __vo BLANK_REG EXTI_SWIER;      // Offset 0x10, reset 0x0000 0000
      __vo BLANK_REG EXTI_PR;         // Offset 0x14, reset 0x0000 0000
    } EXTI_REGS_Typedef;

  // >> Tạo con trỏ phần cứng thật và con trỏ phần cứng giả tới ngoại vi

    #ifndef UNIT_TEST
      #define EXTI_REGS_PTR ((EXTI_REGS_Typedef *) EXTI_REGS_BASEADDR)
    #else
      extern EXTI_REGS_Typedef MOCK_EXTI_REGS;
      #define EXTI_REGS_PTR (&MOCK_EXTI_REGS)
    #endif

  // Khai báo các định nghĩa cần sử dụng trên EXTI_IMR

    #define EXTI_IMR_MASK_ALL 0xFFFFu

  // Khai báo các định nghĩa cần sử dụng trên EXTI_EMR

    #define EXTI_EMR_MASK_ALL 0xFFFFu

  // Khai báo các định nghĩa cần sử dụng trên EXTI_RTSR

    #define EXTI_RTSR_MASK_ALL 0xFFFFu

  // Khai báo các định nghĩa cần sử dụng trên EXTI_FTSR

    #define EXTI_FTSR_MASK_ALL 0xFFFFu

  // Khai báo các định nghĩa cần sử dụng trên EXTI_SWIER

    #define EXTI_SWIER_MASK_ALL 0xFFFFu

  // Khai báo các định nghĩa cần sử dụng trên EXTI_PR

    #define EXTI_PR_MASK_ALL 0xFFFFu

#endif /* LIB_EXTI_DEF_H_ */
