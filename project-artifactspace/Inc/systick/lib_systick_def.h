/*
 * lib_systick_def.h
 *
 *  Created on: Mar 17, 2026
 *      Author: shanghuang
 */

#ifndef LIB_SYSTICK_DEF_H_
  #define LIB_SYSTICK_DEF_H_

  // Khai báo các thư viện sử dụng

    #include <stdint.h>

    #ifndef UNIT_TEST
      #include "generic/lib_keyword_def.h"
    #else
      #include "lib_keyword_def.h"
    #endif

  // Khai báo địa chỉ ngoại vi

    #define SYSTICK_REGS_BASEADDR 0xE000E010ul

  // Khai báo bộ thanh ghi ngoại vi

    tdf_strc SYSTICK_REGS_Typedef {
      __vo BLANK_REG_16B SYSTICK_CTRL;         // Offset 0x00, reset 0x0000 0000
      __vo BLANK_REG_16B SYSTICK_LOAD;         // Offset 0x04, reset 0x0000 0000
      __vo BLANK_REG_16B SYSTICK_VAL;          // Offset 0x08, reset 0x0000 0000
      __vo BLANK_REG_16B SYSTICK_CALIB;          // Offset 0x0C, reset 0x0000 0000
    } SYSTICK_REGS_Typedef;

  // >> Tạo con trỏ phần cứng thật và con trỏ phần cứng giả tới ngoại vi

    #ifndef UNIT_TEST
      #define SYSTICK_REGS_PTR ((SYSTICK_REGS_Typedef *) SYSTICK_REGS_BASEADDR)
    #else
      extern SYSTICK_REGS_Typedef MOCK_SYSTICK_REGS;
      #define SYSTICK_REGS_PTR (&MOCK_SYSTICK_REGS)
    #endif

  // Khai báo các định nghĩa bit cần sử dụng trên thanh ghi SYSTICK_CTRL

    #define SYSTICK_CTRL_ENABLE_POS 0u
    #define SYSTICK_CTRL_ENABLE_MASK (1u << SYSTICK_CTRL_ENABLE_POS)
    #define SYSTICK_CTRL_ENABLE SYSTICK_CTRL_ENABLE_MASK

    #define SYSTICK_CTRL_TICKINT_POS 1u
    #define SYSTICK_CTRL_TICKINT_MASK (1u << SYSTICK_CTRL_TICKINT_POS)
    #define SYSTICK_CTRL_TICKINT SYSTICK_CTRL_TICKINT_MASK

    #define SYSTICK_CTRL_CLKSOURCE_POS 2u
    #define SYSTICK_CTRL_CLKSOURCE_MASK (1u << SYSTICK_CTRL_CLKSOURCE_POS)
    #define SYSTICK_CTRL_CLKSOURCE SYSTICK_CTRL_CLKSOURCE_MASK

    #define SYSTICK_CTRL_COUNTFLAG_POS 16u
    #define SYSTICK_CTRL_COUNTFLAG_MASK (1u << SYSTICK_CTRL_COUNTFLAG_POS)
    #define SYSTICK_CTRL_COUNTFLAG SYSTICK_CTRL_COUNTFLAG_MASK

  // Khai báo các định nghĩa bit cần sử dụng trên SYSTICK_LOAD

    #define SYSTICK_LOAD_MAX_RELOAD_VALUE 0x00FFFFFFu
    #define SYSTICK_LOAD_MIN_RELOAD_VALUE 0x00000001u

#endif /* LIB_SYSTICK_DEF_H_ */
