/*
 * header_dependency.h
 *
 *  Created on: Mar 2, 2026
 *      Author: shanghuang
 */

#ifndef EXIT_HEADER_DEPENDENCY_H_
  #define EXIT_HEADER_DEPENDENCY_H_

  // Khai báo các thư viện chung cho unit test

    #include "lib_keyword_def.h"
    #include "lib_condition_def.h"

  // Khai báo các dependency definition từ các file header cần thiết

    // >> Từ lib_exti_hal

      #ifndef EXTI_HANDLE_PARAM_TYPE
        #define EXTI_HANDLE_PARAM_TYPE
          tdf_strc EXTI_Handle_Param{
            ui16 Line;      // Chọn line EXTI cần xử lý
            void (*Callback)(void); // Con trỏ hàm callback để gọi khi có ngắt EXTI xảy ra
          } EXTI_Handle_Param;
      #endif

      #ifndef EXTI_CALLBACK_EVENT_PARAM_TYPE
        #define EXTI_CALLBACK_EVENT_PARAM_TYPE
          tdf_enum EXTI_Callback_Event_Param {
            EXTI_COMMON_CB_ID = 0xFFU // ID callback chung cho tất cả các line EXTI, sử dụng khi không cần phân biệt line EXTI nào được kích hoạt
          } EXTI_Callback_Event_Param;
      #endif

    // >> Từ lib_afio_hal

      tdf_strc AFIO_EXTI_Init_Param {
        ui16 Port;      // Chọn GPIOx cần cấu hình ngắt ngoài
        ui16 Pin;       // Chọn vị trí chân GPIO cần cấu hình ngắt ngoài (0-3/4-7/8-11/12-15)
        ui16 Line;      // Lưu thông tin về Line EXTI tương ứng với chân GPIO đã chọn, sử dụng cho param của EXTI
      } AFIO_EXTI_Init_Param;

    // >> Từ lib_gpio_def

      #define GPIO_TRIGGER_NONE            0xA0u
      #define GPIO_TRIGGER_RISING          0xA1u
      #define GPIO_TRIGGER_FALLING         0xA2u
      #define GPIO_TRIGGER_BOTH            0xA3u
  
    // >> Từ lib_gpio_hal

      tdf_strc GPIO_Init_Param{
        ui16 Pin;       // Chọn chân GPIO cần cấu hình
        ui8 Mode;       // Chọn chế độ hoạt động cho chân GPIO
        ui8 Pull;       // 1: Kích hoạt pull-up, 0: Kích hoạt pull-down (chỉ áp dụng cho chế độ input)
        ui8 Trigger;    // Chọn kiểu trigger ngắt ngoài (rising/falling/both) nếu chân GPIO được cấu hình cho EXTI, ngược lại sẽ bỏ qua tham số này
      } GPIO_Init_Param;

      #define IS_GPIO_TRIGGER(TRIGGER) (((TRIGGER) == GPIO_TRIGGER_NONE)   || \
                                  ((TRIGGER) == GPIO_TRIGGER_RISING) || \
                                  ((TRIGGER) == GPIO_TRIGGER_FALLING)|| \
                                  ((TRIGGER) == GPIO_TRIGGER_BOTH))

#endif /* EXIT_HEADER_DEPENDENCY_H_ */
