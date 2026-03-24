/*
 * header_dependency.h
 *
 *  Created on: Jan 21, 2026
 *      Author: shanghuang
 */

#ifndef IWDG_HEADER_DEPENDENCY_H_
  #define IWDG_HEADER_DEPENDENCY_H_

  // Khai báo các thư viện chung cho unit test

    #include "lib_keyword_def.h"
    #include "lib_condition_def.h"
  
  // Khai báo các dependency definition từ các file header cần thiết

    // >> Từ lib_clock_def

      # define RCC_IWDG_SOURCE_LSI 0xFul

    // >> Từ lib_clock_hal

      tdf_strc RCC_CLK_Init_Param{
        ul CLK_Source; // Chọn nguồn clock hệ thống
      } RCC_CLK_Init_Param;

      tdf_strc RCC_RDYFLG_Typdef {
        volatile ui HSI_RDY_FLG : 1;
        volatile ui HSE_RDY_FLG : 1;
        volatile ui LSI_RDY_FLG : 1;
      } RCC_RDYFLG_Typdef;

    // >> Từ lib_iwdg_hal

      #ifndef IWDG_INIT_PARAM_TYPE
        #define IWDG_INIT_PARAM_TYPE
        tdf_strc IWDG_Init_Param {
          ui Prescaler; // Chọn bộ chia tần số của IWDG
          ui Reload;    // Giá trị nạp lại cho bộ đếm giảm
        } IWDG_Init_Param;
      #endif

  // Khai báo các dependency function từ các file header cần thiết ngoài module đang test

    RETR_STAT RCC_IsLSIReady(void);
    RETR_STAT RCC_CLK_Init(RCC_CLK_Init_Param *init_param, RCC_RDYFLG_Typdef *rdy_flg);

#endif /* IWDG_HEADER_DEPENDENCY_H_ */
