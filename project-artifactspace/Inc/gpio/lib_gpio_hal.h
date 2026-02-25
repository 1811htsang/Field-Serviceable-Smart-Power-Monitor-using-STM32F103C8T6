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

    #include <stdint.h>
    #include <stdbool.h>

  // Khai báo cấu trúc tham số hàm khởi tạo

    #ifndef GPIO_INIT_PARAM_TYPE
      #define GPIO_INIT_PARAM_TYPE
      tdf_strc GPIO_Init_Param{
        ui8 Pin;       // Chọn chân GPIO cần cấu hình
        ui8 Mode;      // Chọn chế độ hoạt động cho chân GPIO
        bool Pull;     // 1: Kích hoạt pull-up, 0: Kích hoạt pull-down (chỉ áp dụng cho chế độ input)
      } GPIO_Init_Param;
    #endif

  // Khai báo kiểu dữ liệu trả về

    #ifndef PIN_RETR
      #define PIN_RETR
        tdf_enum PIN_RETR_Enum {
          GPIO_PIN_RESET = 0u,
          GPIO_PIN_SET = 1u,
          GPIO_PIN_UNF = 0xFFu
        } PIN_RETR_Enum;
    #endif

  // Khai báo các kiểm tra tham số đầu vào nội bộ

    #define IS_GPIO_INSTANCE(GPIOx) (((GPIOx) == GPIOA_REGS_PTR) || \
                                  ((GPIOx) == GPIOB_REGS_PTR) || \
                                  ((GPIOx) == GPIOC_REGS_PTR) || \
                                  ((GPIOx) == GPIOD_REGS_PTR) || \
                                  ((GPIOx) == GPIOE_REGS_PTR) || \
                                  ((GPIOx) == GPIOF_REGS_PTR) || \
                                  ((GPIOx) == GPIOG_REGS_PTR))

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
    
    #define IS_GPIO_CONFIG(INPUT) (((INPUT) == GPIO_CNF_INPUT_ANALOG)    || \
                                  ((INPUT) == GPIO_CNF_INPUT_FLOATING)   || \
                                  ((INPUT) == GPIO_CNF_INPUT_PU_PD)      || \
                                  ((INPUT) == GPIO_CNF_OUTPUT_PP)        || \
                                  ((INPUT) == GPIO_CNF_OUTPUT_OD)        || \
                                  ((INPUT) == AFIO_OUTPUT_PP)            || \
                                  ((INPUT) == AFIO_OUTPUT_OD))
                                    
    #define IS_GPIO_AFIO_MODE(MODE) (((MODE) == GPIO_MODE_INPUT_ANALOG)     || \
                              ((MODE) == GPIO_MODE_INPUT_FLOATING)          || \
                              ((MODE) == GPIO_MODE_INPUT_PU_PD)             || \
                              ((MODE) == GPIO_MODE_OUTPUT_10MHz_PP)         || \
                              ((MODE) == GPIO_MODE_OUTPUT_10MHz_OD)         || \
                              ((MODE) == GPIO_MODE_OUTPUT_2MHz_PP)          || \
                              ((MODE) == GPIO_MODE_OUTPUT_2MHz_OD)          || \
                              ((MODE) == GPIO_MODE_OUTPUT_50MHz_PP)         || \
                              ((MODE) == GPIO_MODE_OUTPUT_50MHz_OD)         || \
                              ((MODE) == AFIO_MODE_OUTPUT_10MHz_PP)         || \
                              ((MODE) == AFIO_MODE_OUTPUT_2MHz_PP)          || \
                              ((MODE) == AFIO_MODE_OUTPUT_50MHz_PP)         || \
                              ((MODE) == AFIO_MODE_OUTPUT_10MHz_OD)         || \
                              ((MODE) == AFIO_MODE_OUTPUT_2MHz_OD)          || \
                              ((MODE) == AFIO_MODE_OUTPUT_50MHz_OD))
                              

    

    #define IS_AFIO_GPIO_SUPPORT(GPIOx) (((GPIOx) == GPIOA_REGS_PTR) || \
                                  ((GPIOx) == GPIOB_REGS_PTR)        || \
                                  ((GPIOx) == GPIOC_REGS_PTR)        || \
                                  ((GPIOx) == GPIOD_REGS_PTR)        || \
                                  ((GPIOx) == GPIOE_REGS_PTR)        || \
                                  ((GPIOx) == GPIOF_REGS_PTR))
    
    #define IS_GPIO_PULL(PULL) (((PULL) == GPIO_NOPULL) || \
                                ((PULL) == GPIO_PULLUP) || \
                                ((PULL) == GPIO_PULLDOWN))

    #define IS_PINRETR_ENUM(STATE) (((STATE) == GPIO_PIN_RESET) || \
                                  ((STATE) == GPIO_PIN_SET)   || \
                                  ((STATE) == GPIO_PIN_UNF))

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
      ui16 Pin, 
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
