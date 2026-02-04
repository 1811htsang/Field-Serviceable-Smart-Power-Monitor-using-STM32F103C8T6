/*
 * lib_reset_hal.h
 *
 *  Created on: Jan 29, 2026
 *      Author: shanghuang
 */

#ifndef LIB_RESET_HAL_H_
	#define LIB_RESET_HAL_H_

	// Khai báo các thư viện sử dụng chung

		#include <stdint.h>
		#include <stdio.h>
		#include "generic/lib_keyword_def.h"
		#include "generic/lib_condition_def.h"
		#include "reset/lib_reset_def.h"

	// Khai báo cấu trúc catch reset

		tdf_strc RST_FLG_Typdef {
			ui IsPinReset;  // Cờ đánh dấu reset bởi tín hiệu bên ngoài (PIN NRST)
			ui IsPorReset;  // Cờ đánh dấu reset bởi nguồn điện (POR)
			ui IsSftReset;  // Cờ đánh dấu reset bởi phần mềm (SFT)
			ui IsIwdgReset; // Cờ đánh dấu reset bởi watchdog độc lập (IWDG)
			ui IsWwdgReset; // Cờ đánh dấu reset bởi watchdog cửa sổ (WWDG)
			ui IsLowPwrReset; // Cờ đánh dấu reset bởi chế độ điện áp thấp (Low Power)
		} RST_FLG_Typdef;

	// Khai báo các hàm thành phần

		// >> Hàm thực hiện reset phần mềm hệ thống
		void RST_SYS_SW_Reset(void);

		// >> Hàm catch reset event sau khi khởi động lại hệ thống
		void RST_SRC_Capture(RST_FLG_Typdef *reset_source);

#endif /* LIB_RESET_HAL_H_ */
