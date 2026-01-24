/*
 * lib_iwdg_hal.h
 *
 *  Created on: Jan 14, 2026
 *      Author: shanghuang
 */


#ifndef LIB_IWDG_HAL_H_
	#define LIB_IWDG_HAL_H_

	// Khai báo các thư viện sử dụng chung

		#include "lib_keyword_def.h"
		#include "lib_iwdg_def.h"

	// Khai báo cấu trúc tham số hàm khởi tạo

		#ifndef IWDG_INIT_PARAM_TYPE
			#define IWDG_INIT_PARAM_TYPE
			tdf_strc IWDG_Init_Param {
				ui Prescaler; // Chọn bộ chia tần số của IWDG
				ui Reload;    // Giá trị nạp lại cho bộ đếm giảm
			} IWDG_Init_Param;
		#endif

	// Khai báo các kiểm tra tham số đầu vào nội bộ

		#define IS_IWDG_PRESCALER(PRESCALER) (((PRESCALER) == IWDG_PRESCALER_4)   || \
											((PRESCALER) == IWDG_PRESCALER_8)   || \
											((PRESCALER) == IWDG_PRESCALER_16)  || \
											((PRESCALER) == IWDG_PRESCALER_32)  || \
											((PRESCALER) == IWDG_PRESCALER_64)  || \
											((PRESCALER) == IWDG_PRESCALER_128) || \
											((PRESCALER) == IWDG_PRESCALER_256))

		#define IS_IWDG_RELOAD(RELOAD) ((RELOAD) >= IWDG_RLR_REG_RL_MIN && (RELOAD) <= IWDG_RLR_REG_RL_MAX)

	// Khai báo các hàm thành phần

		// >> Hàm khởi tạo IWDG
		RETR_STAT IWDG_Init(IWDG_Init_Param *init_param);

		// >> Hàm de-initialize IWDG
		RETR_STAT IWDG_DeInit(IWDG_Init_Param *init_param);

		// >> Hàm khởi động IWDG
		void IWDG_Start(void);

		// >> Hàm tải lại bộ đếm IWDG
		void IWDG_Reload(void);

		// >> Hàm kiểm tra trạng thái bộ nạp lại giá trị của IWDG
		RETR_STAT IWDG_IsReloadValueUpdated(void);

		// >> Hàm kiểm tra trạng thái bộ chia tần số của IWDG
		RETR_STAT IWDG_IsPrescalerUpdated(void);

		// >> Hàm bật IWDG write access
		void IWDG_EnableWriteAccess(void);

		// >> Hàm tắt IWDG write access
		void IWDG_DisableWriteAccess(void);

		// >> Hàm catch IWDG reset event
		void IWDG_ResetEvent_Catch(void);

#endif /* LIB_IWDG_HAL_H_ */
