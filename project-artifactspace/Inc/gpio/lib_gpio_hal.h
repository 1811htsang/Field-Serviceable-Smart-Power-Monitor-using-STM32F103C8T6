/*
 * lib_gpio_hal.h
 *
 *  Created on: Feb 10, 2026
 *      Author: shanghuang
 */

#ifndef LIB_GPIO_HAL_H_
  #define LIB_GPIO_HAL_H_

  // Khai báo các thư viện sử dụng chung

    #ifndef UNIT_TEST
      #include "generic/lib_keyword_def.h"
      #include "gpio/lib_gpio_def.h"
    #else
      #include "lib_keyword_def.h"
      #include "lib_gpio_def.h"
    #endif

  // Khai báo cấu trúc tham số hàm khởi tạo

    #ifndef GPIO_INIT_PARAM_TYPE
      #define GPIO_INIT_PARAM_TYPE
      tdf_strc GPIO_Init_Param{
        ui8 Pin;       // Chọn chân GPIO cần cấu hình
        ui8 Mode;      // Chọn chế độ hoạt động cho chân GPIO
        ui8 Pull;      // Chọn chế độ kéo lên / kéo xuống cho chân GPIO
        ui8 Speed;     // Chọn tốc độ cho chân GPIO
      } GPIO_Init_Param;
    #endif

  // Khai báo kiểu dữ liệu trả về

    #ifndef PIN_RETR
      #define PIN_RETR
        tdf_enum PIN_RETR_Enum {
          GPIO_PIN_RESET = 0u,
          GPIO_PIN_SET = 1u
        } PIN_RETR_Enum;
    #endif

  // Khai báo các kiểm tra tham số đầu vào nội bộ

    #define IS_GPIO_PIN(PIN) (((PIN) == GPIO_PIN_0)   || \
                              ((PIN) == GPIO_PIN_1)   || \
                              ((PIN) == GPIO_PIN_2)   || \
                              ((PIN) == GPIO_PIN_3)   || \
                              ((PIN) == GPIO_PIN_4)   || \
                              ((PIN) == GPIO_PIN_5)   || \
                              ((PIN) == GPIO_PIN_6)   || \
                              ((PIN) == GPIO_PIN_7)   || \
                              ((PIN) == GPIO_PIN_8)   || \
                              ((PIN) == GPIO_PIN_9)   || \
                              ((PIN) == GPIO_PIN_10)  || \
                              ((PIN) == GPIO_PIN_11)  || \
                              ((PIN) == GPIO_PIN_12)  || \
                              ((PIN) == GPIO_PIN_13)  || \
                              ((PIN) == GPIO_PIN_14)  || \
                              ((PIN) == GPIO_PIN_15)  || \
                              ((PIN) == GPIO_PIN_ALL))
    
    #define IS_GPIO_INPUT(INPUT) (((INPUT) == GPIO_CNF_INPUT_ANALOG)    || \
                                  ((INPUT) == GPIO_CNF_INPUT_FLOATING)    || \
                                  ((INPUT) == GPIO_CNF_INPUT_PU_PD))

    #define IS_GPIO_OUTPUT(OUTPUT) (((OUTPUT) == GPIO_CNF_OUTPUT_PP)   || \
                                    ((OUTPUT) == GPIO_CNF_OUTPUT_OD))
                                    
    #define IS_GPIO_MODE(MODE) (((MODE) == GPIO_MODE_INPUT)        || \
                                ((MODE) == GPIO_MODE_OUTPUT_10MHz)   || \
                                ((MODE) == GPIO_MODE_OUTPUT_2MHz)   || \
                                ((MODE) == GPIO_MODE_OUTPUT_50MHz))

  // Khai báo các hàm thành phần

    // >> Hàm khởi tạo GPIO
    RETR_STAT GPIO_Init(GPIO_REGS_Typedef *GPIOx, GPIO_Init_Param *init_param);

    // >> Hàm vô hiệu hóa GPIO
    RETR_STAT GPIO_DeInit(GPIO_REGS_Typedef *GPIOx, ui8 Pin);

    // >> Hàm đọc trạng thái chân GPIO
    PIN_RETR_Enum GPIO_ReadPin(GPIO_REGS_Typedef *GPIOx, ui8 Pin);

    // >> Hàm ghi trạng thái chân GPIO
    void GPIO_WritePin(
      GPIO_REGS_Typedef *GPIOx, 
      ui8 Pin, 
      PIN_RETR_Enum PinState
    );

    // >> Hàm đảo trạng thái chân GPIO
    void GPIO_TogglePin(GPIO_REGS_Typedef *GPIOx, ui8 Pin);

    // >> Hàm khóa cấu hình chân GPIO
    RETR_STAT GPIO_LockPin(GPIO_REGS_Typedef *GPIOx, ui8 Pin);

    // >> Hàm xử lý ngắt ngoại vi GPIO
    void GPIO_EXTI_IRQHandler(ui8 Pin);

    // >> Hàm callback ngắt ngoại vi GPIO weak
    __weak void GPIO_EXTI_Callback(ui8 Pin);
    

#endif /* LIB_GPIO_HAL_H_ */
