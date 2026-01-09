/*
 * lib_reset_management.h
 *
 *  Created on: Jan 9, 2026
 *      Author: shanghuang
 */

#ifndef LIB_RESET_MANAGEMENT_H_
  #define LIB_RESET_MANAGEMENT_H_

  // Khai báo các thư viện sử dụng chung

  #include "lib_keyword_define.h"
  #include "lib_clock_management.h"

  /**
   * Ghi chú:
   * Do các thành phần kiểm soát reset cũng có trong 
   * RCC_SCR nên cần include lib_clock_management.h
   */

  // Khai báo địa chỉ ngoại vi

  #define SCB_REGS_BASEADDR 0xE000ED00ul

  // Khai báo địa chỉ thanh ghi cụ thể

  #define SCB_AIRCR_REG_ADDR (SCB_REGS_BASEADDR + 0x0Cul)

  // Khai báo cấu trúc thanh ghi

  typedef struct SCB_AIRCR_REG
  {
    __vo unsigned int VECTRESET : 1;
    __vo unsigned int VECTCLRACTIVE : 1;
    __vo unsigned int SYSRESETREQ : 1;
    __vo unsigned int RESERVED0 : 5;
    __vo unsigned int PRIGROUP : 3;
    __vo unsigned int RESERVED1 : 4;
    __vo unsigned int ENDIANNESS : 1;
    __vo unsigned int VECTKEY : 16;
  } SCB_AIRCR_REG;

  // >> Tạo con trỏ tới thanh ghi

  #define SCB_AIRCR_REG_PTR ((SCB_AIRCR_REG *)SCB_AIRCR_REG_ADDR)

  // Khai báo các định nghĩa bit cần sử dụng trên SCB_AIRCR_REG

  #define SCB_AIRCR_REG_VECTKEY_RESET 0xFA05
  #define SCB_AIRCR_REG_VECTKEY_SET 0x05FA
  #define SCB_AIRCR_REG_SYSRESETREQ_RESET RESET
  #define SCB_AIRCR_REG_SYSRESETREQ_SET SET

  

#endif /* LIB_RESET_MANAGEMENT_H_ */
