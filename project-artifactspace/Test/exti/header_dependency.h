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

          /*
           * Cấu trúc tham số cho handle EXTI.
           *
           * Trường:
           *   Line - Line EXTI cần xử lý.
           *   Callback - Hàm callback được gọi khi có ngắt.
           */
          tdf_strc EXTI_Handle_Param{
            ui16 Line;
            void (*Callback)(void);
          } EXTI_Handle_Param;
      #endif

      #ifndef EXTI_CALLBACK_EVENT_PARAM_TYPE
        #define EXTI_CALLBACK_EVENT_PARAM_TYPE

          /*
           * Bộ mã callback sự kiện EXTI.
           *
           * Trường:
           *   EXTI_COMMON_CB_ID - ID callback chung cho mọi line EXTI.
           */
          tdf_enum EXTI_Callback_Event_Param {
            EXTI_COMMON_CB_ID = 0xFFU
          } EXTI_Callback_Event_Param;
      #endif

    // >> Từ lib_afio_hal


      /*
       * Cấu trúc tham số khởi tạo AFIO EXTI.
       *
       * Trường:
       *   Port - Cổng GPIO được ánh xạ sang EXTI.
       *   Pin - Số pin GPIO được ánh xạ sang EXTI.
       *   Line - Line EXTI tương ứng.
       */
      tdf_strc AFIO_EXTI_Init_Param {
        ui16 Port;
        ui16 Pin;
        ui16 Line;
      } AFIO_EXTI_Init_Param;

    // >> Từ lib_gpio_def

      #define GPIO_TRIGGER_NONE            0xA0u
      #define GPIO_TRIGGER_RISING          0xA1u
      #define GPIO_TRIGGER_FALLING         0xA2u
      #define GPIO_TRIGGER_BOTH            0xA3u
  
    // >> Từ lib_gpio_hal


      /*
       * Cấu trúc tham số khởi tạo GPIO dùng cho EXTI.
       *
       * Trường:
       *   Pin - Chân GPIO cần cấu hình.
       *   Mode - Chế độ hoạt động của chân GPIO.
       *   Pull - Trạng thái pull-up/pull-down.
       *   Trigger - Kiểu trigger ngắt ngoài.
       */
      tdf_strc GPIO_Init_Param{
        ui16 Pin;
        ui8 Mode;
        ui8 Pull;
        ui8 Trigger;
      } GPIO_Init_Param;

      #define IS_GPIO_TRIGGER(TRIGGER) (((TRIGGER) == GPIO_TRIGGER_NONE)   || \
                                  ((TRIGGER) == GPIO_TRIGGER_RISING) || \
                                  ((TRIGGER) == GPIO_TRIGGER_FALLING)|| \
                                  ((TRIGGER) == GPIO_TRIGGER_BOTH))

#endif /* EXIT_HEADER_DEPENDENCY_H_ */
