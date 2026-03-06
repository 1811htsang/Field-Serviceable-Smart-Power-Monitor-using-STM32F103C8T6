/*
 * header_dependency.h
 *
 *  Created on: Feb 2, 2026
 *      Author: shanghuang
 */

#ifndef CLOCK_HEADER_DEPENDENCY_H_
	#define CLOCK_HEADER_DEPENDENCY_H_

	// Khai báo các thư viện chung cho unit test

		#include "lib_keyword_def.h"
		#include "lib_condition_def.h"
  
  // Khai báo các dependency definition từ các file header cần thiết

    // >> Từ lib_iwdg_def

			#define IWDG_PR_REG_PR_DIV_4 0x000ul
			#define IWDG_RLR_REG_RL_MAX  0xFFFul

    // >> Từ lib_iwdg_hal

			tdf_strc IWDG_Init_Param {
				ui Prescaler; // Chọn bộ chia tần số của IWDG
				ui Reload;    // Giá trị nạp lại cho bộ đếm giảm
			} IWDG_Init_Param;

		// >> Từ lib_clock_hal
			
			#ifndef RCC_CLK_INIT_PARAM_TYPE
				#define RCC_CLK_INIT_PARAM_TYPE
					tdf_strc RCC_CLK_Init_Param{
						ul CLK_Source; // Chọn nguồn clock hệ thống
					} RCC_CLK_Init_Param;
			#endif

			#ifndef RCC_RDYFLG_TYPEDEF
				#define RCC_RDYFLG_TYPEDEF
					tdf_strc RCC_RDYFLG_Typedef {
						__vo ui HSI_RDY_FLG : 1; // Cờ trạng thái sẵn sàng HSI
						__vo ui HSE_RDY_FLG : 1; // Cờ trạng thái sẵn sàng HSE
						__vo ui LSI_RDY_FLG : 1; // Cờ trạng thái sẵn sàng LSI
					} RCC_RDYFLG_Typedef;
			#endif

  // Khai báo các dependency function từ các file header cần thiết ngoài module đang test

		RETR_STAT IWDG_Init(IWDG_Init_Param *init_param);
		void IWDG_Start(void);
		void IWDG_Reload(void);

#endif /* CLOCK_HEADER_DEPENDENCY_H_ */
