/*
 * lib_exti_hal.h
 *
 *  Created on: Mar 1, 2026
 *      Author: shanghuang
 */

#ifndef LIB_EXTI_HAL_H_
  #define LIB_EXTI_HAL_H_

  // Khai báo các thư viện sử dụng chung

    #ifndef UNIT_TEST
      #include "generic/lib_keyword_def.h"
      #include "exti/lib_exti_def.h"
      #include "afio/lib_afio_def.h"
      #include "afio/lib_afio_hal.h"
    #else
      #include "lib_keyword_def.h"
      #include "lib_exti_def.h"
      #include "lib_afio_def.h"
      #include "lib_afio_hal.h"
    #endif

    #include <stdint.h>

  // Khai báo cấu trúc tham số hàm xử lý ngắt

    #ifndef EXTI_HANDLE_PARAM_TYPE
      #define EXTI_HANDLE_PARAM_TYPE
        tdf_strc EXTI_Handle_Param{
          ui16 Line;      // Chọn line EXTI cần xử lý
          void (*Callback)(void); // Con trỏ hàm callback để gọi khi có ngắt EXTI xảy ra
        } EXTI_Handle_Param;
    #endif

  // Khai báo cấu trúc quản lý đa sự kiện

    #ifndef EXTI_CALLBACK_EVENT_PARAM_TYPE
      #define EXTI_CALLBACK_EVENT_PARAM_TYPE
        tdf_enum EXTI_Callback_Event_Param_Type {
          HAL_EXTI_COMMON_CB_ID = 0x00U // ID callback chung cho tất cả các line EXTI, sử dụng khi không cần phân biệt line EXTI nào được kích hoạt
        } EXTI_Callback_Event_Param_Type;
    #endif

  // Khai báo các kiểm tra tham số đầu vào nội bộ

    /**
     * Ghi chú:
     * Hiện tại các hàm xử lý EXTI sẽ sử dụng trực tiếp các tham số 
     * đã được kiểm tra trong hàm khởi tạo AFIO_EXTI_Line_Init
     */
    
  // Khai báo các hàm thành phần

    // >> Hàm cấu hình tham số EXTI theo thông tin line đã khởi tạo trong AFIO
    RETR_STAT EXTI_Config_Init(AFIO_EXTI_Init_Param *init_param);

    // >> Hàm vô hiệu hóa tham số EXTI theo thông tin line đã khởi tạo trong AFIO
    RETR_STAT EXTI_Config_DeInit(AFIO_EXTI_Init_Param *init_param);

    // >> Hàm xử lý ngắt EXTI
    void EXTI_IRQHandler(EXTI_Handle_Param *handle_param);

    // >> Hàm đăng ký callback xử lý ngắt EXTI
    RETR_STAT EXTI_RegisterCallback(
      EXTI_Handle_Param *handle_param,
      EXTI_Callback_Event_Param_Type callback_event_type,
      void (*callback_func)(void)
    );

#endif /* LIB_EXTI_HAL_H_ */
