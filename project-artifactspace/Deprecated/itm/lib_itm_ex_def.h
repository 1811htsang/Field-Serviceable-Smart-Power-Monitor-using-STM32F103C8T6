/*
 * lib_itm_ex_def.h
 *
 *  Created on: Mar 10, 2026
 *      Author: shanghuang
 */

/**
 * Ghi chú:
 * Header này nhằm khai báo các thanh ghi hoặc các ngoại vi cần sử dụng với ITM
 * nhưng không nằm trong phạm vi của lib_itm_def.h để đảm bảo tính tách biệt giữa các module 
 * và tránh việc khai báo thừa hoặc không cần thiết trong lib_itm_def.h.
 * Thư viện này và ITM không nằm trong danh mục cần thực hiện unit test nên sẽ không có các cấu hình liên quan đến unit test.
 */

#ifndef LIB_ITM_EX_DEF_H_
  #define LIB_ITM_EX_DEF_H_

  // Khai báo các thư viện sử dụng chung

    #include <stdint.h>
    #include <stdio.h>
    #include "generic/lib_keyword_def.h"
    #include "generic/lib_condition_def.h"
    #include "itm/lib_itm_def.h"

  // Khai báo địa chỉ thanh ghi

    #define CRDBG_DEMCR    (*((__vo ui32 *)0xE000EDFCul))
    #define TPIU_ACPR      (*((__vo ui32 *)0xE0040010ul))
    #define TPIU_SPPR      (*((__vo ui32 *)0xE00400F0ul))

#endif /* LIB_ITM_EX_DEF_H_ */
