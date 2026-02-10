/*
 * lib_gpio_def.h
 *
 *  Created on: Feb 10, 2026
 *      Author: shanghuang
 */

#ifndef LIB_GPIO_DEF_H_
  #define LIB_GPIO_DEF_H_

  // Khai báo các thư viện sử dụng chung

    #include <stdint.h>

    #ifndef UNIT_TEST
      #include "generic/lib_keyword_def.h"
    #else
      #include "lib_keyword_def.h"
    #endif

  // Khai báo địa chỉ ngoại vi

    #define AFIO_REGS_BASEADDR 0x40010000ul
    #define GPIOA_REGS_BASEADDR 0x40010800ul
    #define GPIOB_REGS_BASEADDR 0x40010C00ul
    #define GPIOC_REGS_BASEADDR 0x40011000ul
    #define GPIOD_REGS_BASEADDR 0x40011400ul
    #define GPIOE_REGS_BASEADDR 0x40011800ul
    #define GPIOF_REGS_BASEADDR 0x40011C00ul
    #define GPIOG_REGS_BASEADDR 0x40012000ul    
  
	// Khai báo cấu trúc thanh ghi

    

	// >> Tạo con trỏ phần cứng thật và con trỏ phần cứng giả tới ngoại vi

	// Khai báo các định nghĩa bit cần sử dụng trên các thanh ghi

#endif /* LIB_GPIO_DEF_H_ */
