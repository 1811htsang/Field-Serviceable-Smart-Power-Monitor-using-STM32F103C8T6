/*
 * header_dependency.h
 *
 *  Created on: Feb 7, 2026
 *      Author: shanghuang
 */

#ifndef RESET_HEADER_DEPENDENCY_H_
  #define RESET_HEADER_DEPENDENCY_H_

  // Khai báo các thư viện chung cho unit test

    #include "lib_keyword_def.h"
    #include "lib_condition_def.h"

  // Khai báo các dependency definition từ các file header cần thiết

    // >> Từ lib_reset_hal

      #ifndef RCC_RSTFLG_TYPEDEF
				#define RCC_RSTFLG_TYPEDEF
          tdf_strc RCC_RSTFLG_Typedef {
            ui IsPinReset;  // Cờ đánh dấu reset bởi tín hiệu bên ngoài (PIN NRST)
            ui IsPorReset;  // Cờ đánh dấu reset bởi nguồn điện (POR)
            ui IsSftReset;  // Cờ đánh dấu reset bởi phần mềm (SFT)
            ui IsIwdgReset; // Cờ đánh dấu reset bởi watchdog độc lập (IWDG)
            ui IsWwdgReset; // Cờ đánh dấu reset bởi watchdog cửa sổ (WWDG)
            ui IsLowPwrReset; // Cờ đánh dấu reset bởi chế độ điện áp thấp (Low Power)
          } RCC_RSTFLG_Typedef;
			#endif

  /**
   * Ghi chú:
   * Do toàn bộ driver reset không sử dụng các định nghĩa hay hàm ngoài module reset,
   * nên không cần khai báo thêm dependency nào khác.
   */

#endif /* RESET_HEADER_DEPENDENCY_H_ */