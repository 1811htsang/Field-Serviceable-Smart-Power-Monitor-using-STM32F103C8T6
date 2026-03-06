/*
 * header_dependency.h
 *
 *  Created on: Mar 5, 2026
 *      Author: shanghuang
 */

#ifndef NVIC_HEADER_DEPENDENCY_H_
  #define NVIC_HEADER_DEPENDENCY_H_

  // Khai báo các thư viện sử dụng chung cho unit test

    #include "lib_keyword_def.h"
    #include "lib_condition_def.h"

  // Khai báo các dependency definition từ các file header cần thiết

    // >> Từ lib_nvic_hal

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

      #ifndef NVIC_INTR_STATUS_TYPE
        #define NVIC_INTR_STATUS_TYPE
        tdf_enum NVIC_INTR_Status_Type {
          INTR_STAT_DISABLE = (ui16)0xB0,
          INTR_STAT_ENABLE  = (ui16)0xB1,
          INTR_STAT_PENDING = (ui16)0xB2,
          INTR_STAT_UNF     = (ui16)0xFF
        } NVIC_INTR_Status_Type;
      #endif

      #ifndef NVIC_INTR_PARAM_TYPE
        #define NVIC_INTR_PARAM_TYPE
          tdf_strc NVIC_INTR_Param { // Cấu trúc tham số để cấu hình ngắt cấp hệ thống trong NVIC
            IRQ_POS_Enum_Type Position;      // Chọn IRQ_POS cần xử lý
            ui16 Priority;                   // Chọn mức ưu tiên ngắt (0-15, 0 là ưu tiên cao nhất). Lưu ý rằng trong thiết kế tài liệu, mức ưu tiên chỉ sử dụng loại preempt và bỏ qua sub-preempt 
            NVIC_INTR_Status_Type Status;    // Chọn trạng thái kích hoạt ngắt (0: Disable, 1: Enable, 2: Pending)
          } NVIC_INTR_Param;
      #endif

#endif /* NVIC_HEADER_DEPENDENCY_H_ */
