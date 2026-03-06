/*
 * lib_reset_hal.h
 *
 *  Created on: Jan 29, 2026
 *      Author: shanghuang
 */

#ifndef LIB_RESET_HAL_H_
	#define LIB_RESET_HAL_H_

	// Khai báo các thư viện sử dụng chung

		#ifndef UNIT_TEST
			#include "generic/lib_keyword_def.h"
			#include "generic/lib_condition_def.h"
			#include "reset/lib_reset_def.h"
		#else
			#include "lib_keyword_def.h"
			#include "lib_condition_def.h"
			#include "lib_reset_def.h"
		#endif

	// Khai báo cấu trúc catch reset

		#ifndef RCC_RSTFLG_TYPEDEF
			#define RCC_RSTFLG_TYPEDEF
				tdf_strc RCC_RSTFLG_Typedef {
					ui IsPinReset;    // Cờ đánh dấu reset bởi tín hiệu bên ngoài (PIN NRST)
					ui IsPorReset;    // Cờ đánh dấu reset bởi nguồn điện (POR)
					ui IsSftReset;    // Cờ đánh dấu reset bởi phần mềm (SFT)
					ui IsIwdgReset;   // Cờ đánh dấu reset bởi watchdog độc lập (IWDG)
					ui IsWwdgReset;   // Cờ đánh dấu reset bởi watchdog cửa sổ (WWDG)
					ui IsLowPwrReset; // Cờ đánh dấu reset bởi chế độ điện áp thấp (Low Power)
				} RCC_RSTFLG_Typedef;
		#endif

	// Khai báo các hàm thành phần

		// >> Hàm thực hiện reset phần mềm hệ thống
		void RST_SYS_SW_Reset(void);

		// >> Hàm catch reset event sau khi khởi động lại hệ thống
		void RST_SRC_Capture(RCC_RSTFLG_Typedef *reset_source);

#endif /* LIB_RESET_HAL_H_ */
