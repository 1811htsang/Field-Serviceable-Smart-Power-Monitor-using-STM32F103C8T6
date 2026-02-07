/*
 * lib_reset_def.h
 *
 *  Created on: Jan 9, 2026
 *      Author: shanghuang
 */

#ifndef LIB_RESET_DEF_H_
  #define LIB_RESET_DEF_H_

  // Khai báo các thư viện sử dụng chung

		#ifndef UNIT_TEST
			#include "generic/lib_keyword_def.h"
			#include "clock/lib_clock_def.h"
		#else
			#include "lib_keyword_def.h"
			#include "lib_clock_def.h"
		#endif

  // Khai báo địa chỉ ngoại vi

    #define SCB_REGS_BASEADDR 0xE000ED00ul

  // Khai báo địa chỉ thanh ghi cụ thể

    #define SCB_AIRCR_REG_ADDR (SCB_REGS_BASEADDR + 0x0Cul)

  // Khai báo cấu trúc thanh ghi

    tdf_strc SCB_AIRCR_REG
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

    #ifndef UNIT_TEST
      #define SCB_AIRCR_REG_PTR ((SCB_AIRCR_REG *) SCB_AIRCR_REG_ADDR)
    #else
      extern SCB_AIRCR_REG MOCK_SCB_AIRCR_REG;
      #define SCB_AIRCR_REG_PTR (&MOCK_SCB_AIRCR_REG)
    #endif

    /**
     * Ghi chú:
     * Do chỉ sử dụng 1 thanh ghi duy nhất nên không cần tạo cấu trúc toàn bộ ngoại vi SCB.
     */

  // Khai báo các định nghĩa bit cần sử dụng trên RCC_CSR_REG

    #define RCC_CSR_REG_RMVF_NO_EFFECT RESET
		#define RCC_CSR_REG_RMVF_CLEAR SET

		#define RCC_CSR_REG_PINRSTF_NO_RESET_OCCURRED RESET
		#define RCC_CSR_REG_PINRSTF_RESET_OCCURRED SET

		#define RCC_CSR_REG_PORRSTF_NO_RESET_OCCURRED RESET
		#define RCC_CSR_REG_PORRSTF_RESET_OCCURRED SET

		#define RCC_CSR_REG_SFTRSTF_NO_RESET_OCCURRED RESET
		#define RCC_CSR_REG_SFTRSTF_RESET_OCCURRED SET

		#define RCC_CSR_REG_IWDGRSTF_NO_RESET_OCCURRED RESET
		#define RCC_CSR_REG_IWDGRSTF_RESET_OCCURRED SET

		#define RCC_CSR_REG_WWDGRSTF_NO_RESET_OCCURRED RESET
		#define RCC_CSR_REG_WWDGRSTF_RESET_OCCURRED SET

		#define RCC_CSR_REG_LPWRRSTF_NO_RESET_OCCURRED RESET
		#define RCC_CSR_REG_LPWRRSTF_RESET_OCCURRED SET

  // Khai báo các định nghĩa bit cần sử dụng trên SCB_AIRCR_REG

    #define SCB_AIRCR_REG_VECTKEY_RESET 0xFA05
    #define SCB_AIRCR_REG_VECTKEY_SET 0x05FA
    #define SCB_AIRCR_REG_SYSRESETREQ_RESET RESET
    #define SCB_AIRCR_REG_SYSRESETREQ_SET SET

#endif /* LIB_RESET_DEF_H_ */
