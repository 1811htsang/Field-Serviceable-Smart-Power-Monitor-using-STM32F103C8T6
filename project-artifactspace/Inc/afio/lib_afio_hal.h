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
        tdf_strc AFIO_EXTI_Init_Param { // Cấu trúc tham số để khởi tạo thông tin line ngắt ngoài trong AFIO
          ui16 Port;      // Chọn GPIOx cần cấu hình ngắt ngoài
          ui16 Pin;       // Chọn vị trí chân GPIO cần cấu hình ngắt ngoài (0-3/4-7/8-11/12-15)
          ui16 Line;      // Lưu thông tin về Line EXTI tương ứng với chân GPIO đã chọn, sử dụng cho param của EXTI
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

    #define IS_AFIO_PERI_REMAP(PERI) (((PERI) == AFIO_MAPR_SPI1_REMAP) || \
                                  ((PERI) == AFIO_MAPR_I2C1_REMAP) || \
                                  ((PERI) == AFIO_MAPR_USART1_REMAP) || \
                                  ((PERI) == AFIO_SWJ_CFG_FULL_REMAP) || \
                                  ((PERI) == AFIO_SWJ_CFG_NO_NJTRST) || \
                                  ((PERI) == AFIO_SWJ_CFG_JTAG_NO_SW) || \
                                  ((PERI) == AFIO_SWJ_CFG_NO_JTAG_NO_SW))

  // Khai báo các hàm thành phần

    // >> Hàm gọi xử lý mapping 
    RETR_STAT AFIO_PinRemap(ui32 Peri);

    // >> Hàm khởi tạo thông tin line ngắt ngoài
    RETR_STAT AFIO_EXTI_Line_Init(AFIO_EXTI_Init_Param *init_param);

    // >> Hàm vô hiệu hóa thông tin line ngắt ngoài
    RETR_STAT AFIO_EXTI_Line_DeInit(AFIO_EXTI_Init_Param *init_param);

#endif /* LIB_AFIO_HAL_H_ */
