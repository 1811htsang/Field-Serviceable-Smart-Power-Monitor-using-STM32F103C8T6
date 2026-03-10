/*
 * lib_nvic_hal.h
 *
 *  Created on: Mar 1, 2026
 *      Author: shanghuang
 */

#ifndef LIB_NVIC_HAL_H_
  #define LIB_NVIC_HAL_H_

  // Khai báo các thư viện sử dụng chung

    #ifndef UNIT_TEST
      #include "generic/lib_keyword_def.h"
      #include "nvic/lib_nvic_def.h"
    #else
      #include "lib_keyword_def.h"
      #include "lib_nvic_def.h"
    #endif

    #include <stdint.h>

  // Khai báo bộ quản lý số IRQ

    #ifndef IRQ_POS_ENUM_TYPE
      #define IRQ_POS_ENUM_TYPE
        tdf_enum IRQ_POS_Enum_Type {
          NVIC_IRQ_POS_WWDG      = (ui16)0,
          NVIC_IRQ_POS_RCC       = (ui16)5,
          NVIC_IRQ_POS_EXTI0     = (ui16)6,
          NVIC_IRQ_POS_EXTI1     = (ui16)7,
          NVIC_IRQ_POS_EXTI2     = (ui16)8,
          NVIC_IRQ_POS_EXTI3     = (ui16)9,
          NVIC_IRQ_POS_EXTI4     = (ui16)10,
          NVIC_IRQ_POS_EXTI9_5   = (ui16)23,
          NVIC_IRQ_POS_EXTI15_10 = (ui16)40,
          NVIC_IRQ_POS_I2C1_EV   = (ui16)31,
          NVIC_IRQ_POS_I2C1_ER   = (ui16)32,
          NVIC_IRQ_POS_SPI1      = (ui16)35,
          NVIC_IRQ_POS_USART1    = (ui16)37
        } IRQ_POS_Enum_Type;
    #endif

  // Khai báo kiểu dữ liệu trả về

    #ifndef NVIC_INTR_STATUS_TYPE
      #define NVIC_INTR_STATUS_TYPE
      tdf_enum NVIC_INTR_Status_Type {
        INTR_STAT_DISABLE = (ui16)0xB0,
        INTR_STAT_ENABLE  = (ui16)0xB1,
        INTR_STAT_PENDING = (ui16)0xB2,
        INTR_STAT_UNF     = (ui16)0xFF
      } NVIC_INTR_Status_Type;
    #endif

  // Khai báo cấu trúc quản lý thông số ngắt

    #ifndef NVIC_INTR_PARAM_TYPE
      #define NVIC_INTR_PARAM_TYPE
        tdf_strc NVIC_INTR_Param { // Cấu trúc tham số để cấu hình ngắt cấp hệ thống trong NVIC
          IRQ_POS_Enum_Type Position;      // Chọn IRQ_POS cần xử lý
          ui16 Priority;                   // Chọn mức ưu tiên ngắt (0-15, 0 là ưu tiên cao nhất). Lưu ý rằng trong thiết kế tài liệu, mức ưu tiên chỉ sử dụng loại preempt và bỏ qua sub-preempt 
          NVIC_INTR_Status_Type Status;    // Chọn trạng thái kích hoạt ngắt (0: Disable, 1: Enable, 2: Pending)
        } NVIC_INTR_Param;
    #endif

  // Khai báo các kiểm tra tham số đầu vào nội bộ

    #define IS_NVIC_IRQ_POS(POS) (((POS) == NVIC_IRQ_POS_WWDG) || \
                                  ((POS) == NVIC_IRQ_POS_RCC) || \
                                  ((POS) == NVIC_IRQ_POS_EXTI0) || \
                                  ((POS) == NVIC_IRQ_POS_EXTI1) || \
                                  ((POS) == NVIC_IRQ_POS_EXTI2) || \
                                  ((POS) == NVIC_IRQ_POS_EXTI3) || \
                                  ((POS) == NVIC_IRQ_POS_EXTI4) || \
                                  ((POS) == NVIC_IRQ_POS_EXTI9_5) || \
                                  ((POS) == NVIC_IRQ_POS_EXTI15_10) || \
                                  ((POS) == NVIC_IRQ_POS_I2C1_EV) || \
                                  ((POS) == NVIC_IRQ_POS_I2C1_ER) || \
                                  ((POS) == NVIC_IRQ_POS_SPI1) || \
                                  ((POS) == NVIC_IRQ_POS_USART1))
    
    #define IS_NVIC_INTR_STATUS(STATUS) (((STATUS) == INTR_STAT_DISABLE) || \
                                  ((STATUS) == INTR_STAT_ENABLE) || \
                                  ((STATUS) == INTR_STAT_PENDING))

    #define IS_NVIC_INTR_PRIORITY(PRIORITY) (((PRIORITY) <= 15u))

  // Khai báo các hàm thành phần

    // >> Hàm cấu hình trạng thái kích hoạt của ngắt cấp hệ thống

      stinl void NVIC_INTR_Activation_Enable(NVIC_INTR_Param *intr_param);
      stinl void NVIC_INTR_Activation_Disable(NVIC_INTR_Param *intr_param);

    // >> Hàm cấu hình trạng thái pending của ngắt cấp hệ thống

      stinl void NVIC_INTR_Pending_Enable(NVIC_INTR_Param *intr_param);
      stinl void NVIC_INTR_Pending_Disable(NVIC_INTR_Param *intr_param);

    // >> Hàm cấu hình mức ưu tiên của ngắt cấp hệ thống

      stinl void NVIC_INTR_Priority_Config(NVIC_INTR_Param *intr_param);

    // >> Hàm cấu hình tổng quát trạng thái ngắt cấp hệ thống kích hoạt

      RETR_STAT NVIC_INTR_Config(NVIC_INTR_Param *intr_param);

    // >> Hàm kiểm tra trạng thái ngắt cấp hệ thống

      stinl ui32 NVIC_INTR_GetActivation(IRQ_POS_Enum_Type position);

    // >> Hàm kiểm tra trạng thái pending của ngắt cấp hệ thống

      stinl ui32 NVIC_INTR_GetPending(IRQ_POS_Enum_Type position);

#endif /* LIB_NVIC_HAL_H_ */
