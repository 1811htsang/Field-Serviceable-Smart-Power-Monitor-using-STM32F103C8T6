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
  
  // Khai báo các dependency definition từ các file header cần thiết

    // >> Từ lib_clock_def

      # define RCC_IWDG_SOURCE_LSI 0xFul

    // >> Từ lib_clock_hal

      tdf_strc RCC_CLK_Init_Param{
        ul CLK_Source; // Chọn nguồn clock hệ thống
      } RCC_CLK_Init_Param;

  // Khai báo các dependency function từ các file header cần thiết

    RETR_STAT RCC_IsLSIReady(void);
    RETR_STAT RCC_CLK_Init(RCC_CLK_Init_Param *init_param);

#endif /* IWDG_HEADER_DEPENDENCY_H_ */
