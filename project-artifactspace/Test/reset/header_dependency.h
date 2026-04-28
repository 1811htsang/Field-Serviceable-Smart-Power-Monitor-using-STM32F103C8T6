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

          /*
           * Cấu trúc lưu các cờ nguồn reset của RCC.
           *
           * Trường:
           *   IsPinReset - Cờ reset bởi tín hiệu ngoài.
           *   IsPorReset - Cờ reset bởi nguồn điện.
           *   IsSftReset - Cờ reset bởi phần mềm.
           *   IsIwdgReset - Cờ reset bởi watchdog độc lập.
           *   IsWwdgReset - Cờ reset bởi watchdog cửa sổ.
           *   IsLowPwrReset - Cờ reset bởi chế độ điện áp thấp.
           */
          tdf_strc RCC_RSTFLG_Typedef {
            ui IsPinReset;
            ui IsPorReset;
            ui IsSftReset;
            ui IsIwdgReset;
            ui IsWwdgReset;
            ui IsLowPwrReset;
          } RCC_RSTFLG_Typedef;
			#endif

  /**
   * Ghi chú:
   * Do toàn bộ driver reset không sử dụng các định nghĩa hay hàm ngoài module reset,
   * nên không cần khai báo thêm dependency nào khác.
   */

#endif /* RESET_HEADER_DEPENDENCY_H_ */