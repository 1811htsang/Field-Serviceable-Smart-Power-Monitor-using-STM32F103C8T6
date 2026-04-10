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

  // >> Tạo con trỏ tới thanh ghi

    #ifndef UNIT_TEST
      extern __vo BLANK_REG_32B* SCB_AIRCR_REG_PTR;
      
    #else
      /**
       * Ghi chú:
       * Trong môi trường unit test, không thể khai báo biến con trỏ tới thanh ghi thực tế,
       * do cần sử dụng biến giả để mô phỏng giá trị thanh ghi và kiểm tra trong các test case.
       */  

      extern __vo ui32 mock_aircr_reg; 
      extern __vo BLANK_REG_32B* SCB_AIRCR_REG; // Trỏ vào biến giả lập
      
    #endif

    /**
     * Ghi chú:
     * Do chỉ sử dụng 1 thanh ghi duy nhất nên không cần tạo cấu trúc toàn bộ ngoại vi SCB.
     */

  // Khai báo các định nghĩa bit cần sử dụng trên RCC_CSR_REG

    #define RCC_CSR_REG_PINRSTF_OCCURRED  ((ui32)1u << 26)
    #define RCC_CSR_REG_PORRSTF_OCCURRED  ((ui32)1u << 27)
    #define RCC_CSR_REG_SFTRSTF_OCCURRED  ((ui32)1u << 28)
    #define RCC_CSR_REG_IWDGRSTF_OCCURRED ((ui32)1u << 29)
    #define RCC_CSR_REG_WWDGRSTF_OCCURRED ((ui32)1u << 30)
    #define RCC_CSR_REG_LPWRRSTF_OCCURRED ((ui32)1u << 31)
    #define RCC_CSR_REG_RMVF_SET ((ui32)1u << 24)

  // Khai báo các định nghĩa bit cần sử dụng trên SCB_AIRCR_REG

    #define SCB_AIRCR_REG_VECTKEY_RESET ((ui32)0xFA05 << 16)
    #define SCB_AIRCR_REG_VECTKEY_SET   ((ui32)0x05FA << 16)
    #define SCB_AIRCR_REG_SYSRESETREQ_SET ((ui32)1u << 2)
    #define SCB_AIRCR_REG_SYSRESETREQ_CLEAR ~((ui32)1u << 2)

#endif /* LIB_RESET_DEF_H_ */
