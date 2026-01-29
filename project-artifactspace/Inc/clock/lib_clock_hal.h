/*
 * lib_clock_hal.h
 *
 *  Created on: Jan 14, 2026
 *      Author: shanghuang
 */

#ifndef LIB_CLOCK_HAL_H_
  #define LIB_CLOCK_HAL_H_

  // Khai báo các thư viện sử dụng chung

  	#include "lib_keyword_def.h"
		#include "lib_clock_def.h"

  // Khai báo cấu trúc tham số hàm khởi tạo

		tdf_strc RCC_CLK_Init_Param{
			ul CLK_Source; // Chọn nguồn clock hệ thống
		} RCC_CLK_Init_Param;

	// Khai báo các cờ trả về có thời hạn 

		tdf_strc RCC_RDYFLG_Typdef {
			ui HSI_RDY_FLG : 1;
			ui HSE_RDY_FLG : 1;
			ui LSI_RDY_FLG : 1;
		} RCC_RDYFLG_Typdef;

		/**
		 * Ghi chú:
		 * Do các cờ trạng thái này sẽ được thiết lập về 0 trong 
		 * N chu kỳ hoạt động tiếp theo của hệ thống,
		 * Do đó cần có biến lưu trữ trạng thái này
		 * để người dùng có thể kiểm tra trạng thái khởi tạo nguồn clock
		 */

  // Khai báo các kiểm tra tham số đầu vào nội bộ

		#define IS_RCC_SYSCLK_SOURCE(SOURCE) (((SOURCE) == RCC_SYSCLK_SOURCE_HSI) || \
																					((SOURCE) == RCC_SYSCLK_SOURCE_HSE))
		#define IS_RCC_IWDG_SOURCE(SOURCE) (((SOURCE) == RCC_IWDG_SOURCE_LSI))

  // Khai báo IQRHandler

		#define RCC_NMI_IRQ_Handler NMI_Handler
		#define RCC_GNR_IRQ_Handler RCC_IRQHandler

  // Khai báo các hàm thành phần

		// >> Hàm khởi tạo clock hệ thống
		RETR_STAT RCC_CLK_Init(RCC_CLK_Init_Param *init_param, RCC_RDYFLG_Typdef *rdy_flg);

		// >> Hàm chuyển đổi SYSCLK
		RETR_STAT RCC_SYSCLK_Switch(ul sysclk_source);

		// >> Hàm de-initialize clock hệ thống
		RETR_STAT RCC_CLK_DeInit(RCC_CLK_Init_Param *init_param, RCC_RDYFLG_Typdef *rdy_flg);

		// >> Hàm bật CSS
		void RCC_CSS_Enable(void);

		// >> Hàm tắt CSS
		void RCC_CSS_Disable(void);

		// >> Hàm xử lý ngắt NMI do CSS
		void RCC_NMI_IRQ_Handler(void);

		// >> Hàm xử lý ngắt chung RCC
		void RCC_GNR_IRQ_Handler(void);

		// >> Hàm callback CSS weak
		__weak void RCC_CSS_Callback(void);

		// >> Hàm kiểm tra clock HSI sẵn sàng
		RETR_STAT RCC_IsHSIReady(void);

		// >> Hàm kiểm tra clock HSE sẵn sàng
		RETR_STAT RCC_IsHSEReady(void);

		// >> Hàm kiểm tra clock LSI sẵn sàng
		RETR_STAT RCC_IsLSIReady(void);

#endif /* LIB_CLOCK_HAL_H_ */
