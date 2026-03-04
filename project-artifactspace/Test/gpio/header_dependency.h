/*
 * header_dependency.h
 *
 *  Created on: Feb 26, 2026
 *      Author: shanghuang
 */

#ifndef GPIO_HEADER_DEPENDENCY_H_
  #define GPIO_HEADER_DEPENDENCY_H_

  // Khai báo các thư viện chung cho unit test

    #include <stdint.h>
    #include <stdbool.h>
    #include "lib_keyword_def.h"
    #include "lib_condition_def.h"

  // Khai báo các dependency definition từ các file header cần thiết

    // >> Từ lib_gpio_hal

      #ifndef GPIO_INIT_PARAM_TYPE
        #define GPIO_INIT_PARAM_TYPE
        tdf_strc GPIO_Init_Param{
          ui8 Pin;       // Chọn chân GPIO cần cấu hình
          ui8 Mode;      // Chọn chế độ hoạt động cho chân GPIO
          bool Pull;     // 1: Kích hoạt pull-up, 0: Kích hoạt pull-down (chỉ áp dụng cho chế độ input)
        } GPIO_Init_Param;
      #endif

      #ifndef PIN_RETR
        #define PIN_RETR
          tdf_enum PIN_RETR_Enum {
            GPIO_PIN_RESET = 0u,
            GPIO_PIN_SET = 1u,
            GPIO_PIN_UNF = 0xFFu
          } PIN_RETR_Enum;
      #endif

  /**
   * Ghi chú:
   * Do toàn bộ driver gpio không sử dụng các định nghĩa hay hàm ngoài module gpio,
   * nên không cần khai báo thêm dependency nào khác.
   */

#endif /* GPIO_HEADER_DEPENDENCY_H_ */
