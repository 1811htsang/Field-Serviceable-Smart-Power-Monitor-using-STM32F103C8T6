/*
 * lib_afio_hal.h
 *
 *  Created on: Feb 28, 2026
 *      Author: shanghuang
 */

#ifndef LIB_AFIO_HAL_H_
  #define LIB_AFIO_HAL_H_

  // Khai báo các thư viện sử dụng chung

    #ifndef UNIT_TEST
      #include "generic/lib_keyword_def.h"
      #include "afio/lib_afio_def.h"
      #include "gpio/lib_gpio_def.h"
    #else
      #include "lib_keyword_def.h"
      #include "lib_afio_def.h"
      #include "lib_gpio_def.h"
    #endif

    #include <stdint.h>

  // Khai báo cấu trúc tham số khởi tạo với ngắt ngoài

    #ifndef AFIO_EXTI_INIT_PARAM_TYPE
      #define AFIO_EXTI_INIT_PARAM_TYPE
        tdf_strc AFIO_EXTI_Init_Param {
          ui8 Port;      // Chọn GPIOx cần cấu hình ngắt ngoài
          ui8 Shift;     // Chọn vị trí chân GPIO cần cấu hình ngắt ngoài (0-3/4-7/8-11/12-15)
        } AFIO_EXTI_Init_Param;
    #endif

  // Khai báo các kiểm tra tham số đầu vào nội bộ

    #define IS_AFIO_EXTI_PORT(PORT) (((PORT) == AFIO_EXTICR_PORTA) || \
                                  ((PORT) == AFIO_EXTICR_PORTB) || \
                                  ((PORT) == AFIO_EXTICR_PORTC) || \
                                  ((PORT) == AFIO_EXTICR_PORTD) || \
                                  ((PORT) == AFIO_EXTICR_PORTE) || \
                                  ((PORT) == AFIO_EXTICR_PORTF) || \
                                  ((PORT) == AFIO_EXTICR_PORTG))

    #define IS_AFIO_EXTI_SHIFT(SHIFT) (((SHIFT) == M4R0_SHIFT) || \
                                    ((SHIFT) == M4R1_SHIFT) || \
                                    ((SHIFT) == M4R2_SHIFT) || \
                                    ((SHIFT) == M4R3_SHIFT))

  // Khai báo các hàm thành phần

    // >> Hàm gọi xử lý mapping 
    RETR_STAT AFIO_PinRemap(ui32 Peri);

    // >> Hàm khởi tạo ngắt ngoài
    RETR_STAT AFIO_EXTI_Init(AFIO_EXTI_Init_Param *init_param);

    // >> Hàm vô hiệu hóa ngắt ngoài
    RETR_STAT AFIO_EXTI_DeInit(AFIO_EXTI_Init_Param *init_param);

#endif /* LIB_AFIO_HAL_H_ */
